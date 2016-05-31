#include "statistics.h"

#define MEMORY_ACCESS_TIME      100 /* 100 ns */
#define DISK_ACCESS_TIME   10000000 /* 10 million ns = 10 ms */

/*	Available variables from statistics that you may find useful
 *    count_pagefaults   - the number of page faults that occurred (includes required)
 *    count_tlbhits      - the number of tlbhits that occurred
 *    count_writes       - the number of stores/writes that occurred
 *    count_reads        - the number of reads that occurred
 * 	  disk_accesses		 - the number of disk accesses NOT including required faults
 * Any other values you might need are composites of the above values.
 */

double compute_emat_all() {
   /* FIX ME - Compute the average memory access time, including required page faults
    * that occur when loading a new process.
    */
	int mem_access_freq = count_writes + count_reads;
	//condition 1: page fault
	int pagefault_total_time = (3 * MEMORY_ACCESS_TIME + DISK_ACCESS_TIME) * count_pagefaults;
	//condition 2: no page fault, not in tlb, but in ram
	int inram_total_time = (2 * MEMORY_ACCESS_TIME) * (mem_access_freq - count_pagefaults - count_tlbhits);
	//condition 3: already in tlb
	int tlbhit_total_time = MEMORY_ACCESS_TIME * count_tlbhits;

	int total = pagefault_total_time + inram_total_time + tlbhit_total_time;
	int avg_time = total / mem_access_freq;

   return avg_time;
}

double compute_emat_unforced() {
   /* FIX ME - Compute the average memory access time NOT including required faults
    */
	int mem_access_freq = count_writes + count_reads;
	//condition 1: page fault
	int pagefault_total_time = (3 * MEMORY_ACCESS_TIME + DISK_ACCESS_TIME) * count_diskaccesses;
	//condition 2: no page fault, not in tlb, but in ram
	int inram_total_time = (2 * MEMORY_ACCESS_TIME) * (mem_access_freq - count_pagefaults - count_tlbhits);
	//condition 3: already in tlb
	int tlbhit_total_time = MEMORY_ACCESS_TIME * count_tlbhits;

	int total = pagefault_total_time + inram_total_time + tlbhit_total_time;
	int avg_time = total / mem_access_freq;
	return avg_time;
}
