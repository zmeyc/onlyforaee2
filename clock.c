#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int hand; // clock hand, index of the oldest page

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	int i = hand;

	for (; i < memsize; i = (i + 1) % memsize) {
		if (coremap[i].reference == 0) { // reference = 0: evicted
			coremap[i].reference = 1; // new page insert here
			hand = (i + 1) % memsize;
			return i;
		} else { // reference = 1: set reference 0, advance hand
			coremap[i].reference = 0;
		}
	}

	return 0;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	coremap[p->frame >> PAGE_SHIFT].reference = 1;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	hand = 0;

	for (int i = 0; i < memsize; i++) {
		coremap[i].reference = -1; // initially, no page in, set reference -1
	}
}

