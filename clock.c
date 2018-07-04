#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// Define the global variable
int hand; // Clock hand, the index of the oldest page in the coremap


/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int clock_evict() {
	int i = hand;

	for (; i < memsize; i = (i + 1) % memsize) {
		if (coremap[i].reference == 0) { // reference = 0: evict this page
			// New page insert here
			coremap[i].reference = 1;

			// Update hand, and advance the hand in one position
			hand = (i + 1) % memsize;

			return i;

		} else { // reference = 1: set reference 0
			coremap[i].reference = 0;
		}
	}

	// Should not reach here
	return 0;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	// Reference to the page
	coremap[p->frame >> PAGE_SHIFT].reference = 1;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	hand = 0;

	// Initially, no page in the coremap, set reference -1
	for (int i = 0; i < memsize; i++) {
		coremap[i].reference = -1;
	}
}

