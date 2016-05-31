#include <stdlib.h>

#include "types.h"
#include "pagetable.h"
#include "global.h"
#include "process.h"

/*******************************************************************************
 * Finds a free physical frame. If none are available, uses a clock sweep
 * algorithm to find a used frame for eviction.
 *
 * @return The physical frame number of a free (or evictable) frame.
 */
pfn_t get_free_frame(void) {
   int i;

   /* See if there are any free frames */
   for (i = 0; i < CPU_NUM_FRAMES; i++)
      if (rlt[i].pcb == NULL)
         return i;

   /* FIX ME : Bonus */
   /*
   * IMPLEMENT A CLOCK SWEEP ALGORITHM HERE
   * Note you can access whether a page is used or not with the following
   * rlt[i].pcb->pagetable[rlt[i].vpn].used
   * but it's a little ridiculous, so you should probably add a used field
   * to the rlt itself instead and update it appropriately!
   */
	for (i = 0; i < CPU_NUM_FRAMES; i++) {
		pcb_t *cur_pcb = (rlt + i)->pcb;
		pte_t *page_table = cur_pcb->pagetable;
		pte_t *cur_page_entry = page_table + rlt[i].vpn;
		if (!cur_page_entry->valid) {
			return i;
		}
	}

	for (i = 0; i < CPU_NUM_FRAMES; i++) {
		pcb_t *cur_pcb = (rlt + i)->pcb;
		pte_t *page_table = cur_pcb->pagetable;
		pte_t *cur_page_entry = page_table + rlt[i].vpn;
		if (!cur_page_entry->used) {
			return i;
		} else {
			cur_page_entry->used = 0;
		}
	}

   /* If all else fails, return a random frame */
   return 0;
}
