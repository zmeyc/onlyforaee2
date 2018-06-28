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

#define INF_TIME         -1

// Define data structures

// Double linked list
typedef struct node {
	int distance;
	struct node *next;
} Node;

/*
 * Initialize a new page with given page id, return a pointer to head.
 */
Node *init_node() {
	Node *head = malloc(sizeof(Node));
	head->distance = INF_TIME;
	head->next = NULL;
	return head;
}

// Hash table (node)
typedef struct page {
		addr_t id; // Last 20 bits of virtual address
		// Linked list store the length of time between each call of this page
		Node *t_interval;
		int last_occur; // Time that this page last time been called.
		struct page *next_page;
} Page;

/*
 * Initialize a new page with given page id, return a pointer to the new page.
 */
Page *init_hash(addr_t vaddr, int o_time, Page *head) {
	Page *new = malloc(sizeof(Page));
	new->id = vaddr >> PAGE_SHIFT;
	Node *time_interval = init_node();
	new->t_interval = time_interval;
	new->last_occur = o_time;
	new->next_page = head;
	return new;
}

/*
 * Look up the time interval for the given virtual address. Return a pointer to
 * the page for the given virtual address, NULL if such page does not existed.
 */
Page *lookup_hash(addr_t vaddr, Page *head) {
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
 * Set up waiting time for the current page inside curret time interval.
 */
void add_time(Page *cur, int cur_time) {
	Node *interval = cur->t_interval;

	while (interval->next != NULL) {
		interval = interval->next;
	}

	interval->distance = cur_time - cur->last_occur;
	cur->last_occur = cur_time;
	Node *new = init_node();
	interval->next = new;
}

/*
 * Delete time from time interval inside current page.
 */
int del_time(Page *cur) {
	int distance = (cur->t_interval)->distance;
	if (distance == 0) { // For debug only.
		return -1;
	} else if (distance == INF_TIME) {
		return 0;
	} else {
		distance = 0;
		Node *head = cur->t_interval;
		cur->t_interval = (cur->t_interval)->next;
		free(head);
	}
	return 0;
}

Page *head = NULL;

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int distance = 0;
	int index;

	for (int i = 0; i < memsize; i++) {
		addr_t vaddr = coremap[i].vaddr;
		Page *cur_page = lookup_hash(vaddr, head);
		int cur_time = (cur_page->t_interval)->distance;

		if (cur_time == INF_TIME) {
			return i;
		} else if (cur_time > distance) {
			distance = cur_time;
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

	addr_t vaddr = coremap[p->frame >> PAGE_SHIFT].vaddr;
	Page *cur_page = lookup_hash(vaddr, head);
	int result = del_time(cur_page);

	if (result != 0) { // For debug only
		printf("Try to delete time from interval with zero content.\n");
	}
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	int distance = 0;
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

			Page *cur_page = lookup_hash(vaddr, head);
			if (cur_page == NULL) { // New vaddr (page), init new entry
				head = init_hash(vaddr, distance, head);
			} else { // If in hash table, add distance
				add_time(cur_page, distance);
			}

			distance++;
		} else {
			continue;
		}
	}

	if(fclose(tfp) != 0) {
		perror("Error opening tracefile:");
		exit(1);
	}
}
