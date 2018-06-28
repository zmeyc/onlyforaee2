#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;
unsigned *framepages;
int *references;


/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	while (references[clock_hand] != 0){
		references[clock_hand] = 0;
		clock_hand = (clock_hand + 1) % memsize;
	}
	int frame = (int) framepages[clock_hand];
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	unsigned frame_temp = PGTBL_INDEX(p->frame);
	int i;
	for (i = 0; i < memsize; i++){
		if (frame_temp == framepages[i]){
			references[i] = 1;
		}
	}
	references[clock_hand] = 1;
	framepages[clock_hand] = frame_temp;
	clock_hand += 1;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	int * references = malloc(sizeof(int)*memsize);
	unsigned * framepages = malloc(sizeof(unsigned)*memsize);
	clock_hand = 0;
	int j = 0;
	for (j = 0; j<memsize; j++){
		framepages[j] = 0;
		references[j] = 0;
	}
}
