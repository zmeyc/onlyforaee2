#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

#define INF -1

extern unsigned memsize;

extern int debug;

extern struct frame *coremap;

// Define the data structure
typedef struct page {
	addr_t id; // Last 20 bits of virtual address
	int next_ref; // Store index of the next reference of this page, -1 if none
} Page;

// Define the global variables
Page *page_list;
int time = 0;


/*
 * Return the index of the previous reference of the page with
 * virtual address vaddr if it has the previous reference.
 * Otherwise, return INF.
 */
int find_pre(addr_t vaddr, int index) {
	int i = index - 1;
	while (i >= 0) {
		if (page_list[i].id == vaddr) {
			return i;
		}
		i--;
	}

	return INF;
}

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int last_call = 0;
	int index = 0;

	for (int i = 0; i < memsize; i++) {
		if (coremap[i].next_ref_time == INF) { // Never refer again, evict
			return i;
		} else if (coremap[i].next_ref_time > last_call) {
			// Evict the page that won't be used for the longest period of time
			last_call = coremap[i].next_ref_time;
			index = i;
		}
	}

	return index;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//
	coremap[p->frame >> PAGE_SHIFT].next_ref_time = page_list[time].next_ref;
	time++;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	int size = 0; // Store the size of the tracefile to initialize the page_list
	int index = 0; // Record the current index of the page inside tracefile
	FILE *tfp = NULL;

	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;

	// Open the tracefile for reading
	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}

	// Keep track of the size of the tracefile
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			size++;
		}
	}

	// Allocate the space for the page_list
	page_list = malloc(sizeof(Page) * size);

	// Read the lines
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			if (debug) {
				printf("%c %lx\n", type, vaddr);
			}

			// Store the information of each line
			page_list[index].id = vaddr;
			page_list[index].next_ref = INF;

			// Keep track of the next reference
			int pre_ref = find_pre(vaddr, index);
			if (pre_ref != INF) { // Not the first reference
				page_list[pre_ref].next_ref = index;
			}

			index++;
		}
	}

	// Close the tracefile after reading
	if(fclose(tfp) != 0) {
		perror("Error opening tracefile:");
		exit(1);
	}
}
