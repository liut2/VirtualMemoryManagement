#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "statistics.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, calls pagetable_lookup()
 * to get the entry from the page table instead
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t tlb_lookup(vpn_t vpn, int write) {
   // currently just skips tlb and goes to pagetable
   pfn_t pfn;
	int i;

   /*
    * FIX ME : Step 5
    * Note that tlb is an array with memory already allocated and initialized to 0/null
    * meaning that you don't need special cases for a not-full tlb, the valid field
    * will be 0 for both invalid and empty tlb entries, so you can just check that!
    */

   /*
    * Search the TLB - hit if find valid entry with given VPN
    * Increment count_tlbhits on hit.
    */
	int if_exist_invalid = 0;
	tlbe_t *target_entry = NULL;

	for (i = 0; i < tlb_size; i++) {
		tlbe_t *tlb_entry = tlb + i;
		if (tlb_entry->valid != 0) {
			if (tlb_entry->vpn == vpn) {
				count_tlbhits++;
				target_entry = tlb_entry;
				break;
			}
		} else {
			if_exist_invalid = 1;
		}
	}
   /*
    * If it was a miss, call the page table lookup to get the pfn
    * Add current page as TLB entry. Replace any invalid entry first,
    * then do a clock-sweep to find a victim (entry to be replaced).
    */
	if (target_entry == NULL) {
		pfn = pagetable_lookup(vpn, write);
	} else {
		pfn = target_entry->pfn;
	}

	if (if_exist_invalid) {
		for (i = 0; i < tlb_size; i++) {
			tlbe_t *tlb_entry = tlb + i;
			if (tlb_entry->valid == 0) {
				target_entry = tlb_entry;
				target_entry->vpn = vpn;
				target_entry->pfn = pfn;
				target_entry->valid = 1;
				break;
			}
		}
	} else {
		for (i = 0; i < tlb_size; i++) {
			tlbe_t *tlb_entry = tlb + i;
			if (!tlb_entry->used) {
				target_entry = tlb_entry;
				target_entry->vpn = vpn;
				target_entry->pfn = pfn;
				target_entry->valid = 1;
				break;
			} else {
				tlb_entry->used = 0;
			}
		}
	}

	if (target_entry == NULL) {
		target_entry = tlb;
		target_entry->vpn = vpn;
		target_entry->pfn = pfn;
		target_entry->valid = 1;
	}

   /*
    * In all cases perform TLB house keeping. This means marking the found TLB entry as
    * used and if we had a write, dirty. We also need to update the page
    * table entry in memory with the same data.
    */
	target_entry->used = 1;
	target_entry->dirty = write == 1? 1 : 0;

	pte_t *current_entry = current_pagetable + vpn;
	current_entry->used = 1;
	current_entry->dirty = target_entry->dirty;
   return pfn;
}
