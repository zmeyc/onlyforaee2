#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

extern unsigned memsize;

extern int debug;

extern struct frame *coremap;

// Define data structures

// Linked list to store time that such page been called.
typedef struct node {
	int ref_time;
	struct node *next;
} Node;

// Hash table (node)
typedef struct page {
		addr_t id; // Last 20 bits of virtual address
		// Linked list store the calling time of this page.
		Node *ref_time_head; // Pointer to the head of the linked list.
		Node *ref_time_tail; // Pointer to the tail of the linked list.
		struct page *next_page;
} Page;

// Global variables
Page *page_ref_list = NULL;
Page **hash_table = NULL;
int cur_time = 0; // Add up int opt_ref

/*
 * Initialize a new node with the given page id, return a pointer to head.
 */
Node *init_node(int ref_time) {
	Node *new = malloc(sizeof(Node));
	new->ref_time = ref_time;
	new->next = NULL;
	return new;
}

/*
 * Initialize a new page with given page id, return a pointer to the new page.
 */
Page *init_page(addr_t vaddr, int o_time) {
	Page *new = malloc(sizeof(Page));
	new->id = vaddr >> PAGE_SHIFT;
	new->ref_time_head = init_node(o_time);
	new->ref_time_tail = new->ref_time_head;
	new->next_page = NULL;
	return new;
}

/*
 * Look up the time interval for the given virtual address. Return a pointer to
 * the page for the given virtual address, NULL if such page does not exist.
 */
Page *lookup_pre(addr_t vaddr, Page *head) {
	Page *cur = head;
	while (cur != NULL) {
		if (cur->id == (vaddr >> PAGE_SHIFT)) {
			return cur;
		}
		cur = cur->next_page;
	}
	return NULL;
}

/*
 * Look up the time interval for the given index inside trace file. Return a
 * pointer to the page for the given virtual address, NULL if such page does not
 * exist.
 */
Page *lookup_hash(int c_time) {
	return hash_table[c_time];
}

/*
 * Set up calling time for the current page when this page is called.
 */
void add_time(Page *cur, int c_time) {
	Node *last_call = cur->ref_time_tail;
	Node *new = init_node(c_time);
	last_call->next = new;
	cur->ref_time_tail = new;
}

/*
 * Delete time from time interval inside current page.
 */
void del_time(Page *cur) {
	Node *time_l = cur->ref_time_head;
	cur->ref_time_head = time_l->next;
	free(time_l);
}

int check_ref_time(Page *cur_page) {
	if (cur_page->ref_time_head == NULL) {
		return -1;
	}
	return (cur_page->ref_time_head)->ref_time;
}

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int last_call = 0;
	int index;

	for (int i = 0; i < memsize; i++) {
		Page *cur_page = lookup_hash(coremap[i].in_time);

		if (cur_page == NULL) {
			printf("Current pages with index %d does not existed in hash.\n", i);
		}

		int r_time = check_ref_time(cur_page);

		if (r_time == -1) {
			return i;
		}

		if (r_time > last_call) {
			last_call = r_time;
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
	coremap[p->frame >> PAGE_SHIFT].in_time = cur_time;
	Page *cur_page = lookup_hash(cur_time);
	if (cur_page == NULL) {
		printf("Current pages with index %d does not existed in hash.\n", cur_time);
	}

	if (cur_time != check_ref_time(cur_page)) {
		printf("Page recorded incorrect calling time.\n");
	}

	del_time(cur_page);

	cur_time++;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	int distance = 0; // Record time (or line number inside tra)
	FILE *tfp = NULL;

	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}

	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;

	while(fgets(buf, MAXLINE, tfp) != NULL) {

		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			if(debug)  {
				printf("%c %lx\n", type, vaddr);
			}

			Page *cur_page = lookup_pre(vaddr, page_ref_list);
			if (cur_page == NULL) { // New vaddr (page), init new entry
				Page *new = init_page(vaddr, distance);
				new->next_page = page_ref_list;
				page_ref_list = new;
			} else { // If in hash table, add distance
				add_time(cur_page, distance);
			}

			distance++;
		} else {
			continue;
		}
	}

	hash_table = malloc(sizeof(Page *) * distance);
	Page *c_page = page_ref_list;
	while (c_page != NULL) {
		Node *c_time = c_page->ref_time_head;
		while (c_time != NULL) {
			hash_table[c_time->ref_time] = c_page;
			c_time = c_time->next;
		}
		c_page = c_page->next_page;
	}

	if(fclose(tfp) != 0) {
		perror("Error opening tracefile:");
		exit(1);
	}
}
