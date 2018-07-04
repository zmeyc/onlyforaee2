#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int evict; // Keep track of the oldest PFN which need to be evicted

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
	int evict_PFN = evict;

	// Update global variable evict
	evict = (evict + 1) % memsize;

	return evict_PFN;	
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
	// Nothing need to be updated
}

/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void fifo_init() {
	evict = 0;
}
