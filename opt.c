#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

extern int memsize;

extern int debug;

extern struct frame *coremap;

#define INF_TIME         -1

// Define data structures
// Hash table (node)
struct nlist {
		addr_t id; // Last 20 bits of virtual address
		// Linked list store the length of time between each call of this page
		node *t_interval;
		int last_occur; // Time that this page last time been called.
		page *next_page;
} page;

/*
 * Initialize a new page with given page id, return a pointer to the new page.
 */
page *init_hash(addr_t vaddr, int o_time, page *head) {
	page *new = malloc(sizeof(struct page));
	new->id = vaddr >> PAGE_SHIFT;
	node *time_interval = init_node();
	new->t_interval = time_interval;
	last_occur = o_time;
	next = head;
	return new;
}

/*
 * Look up the time interval for the given virtual address. Return a pointer to
 * the page for the given virtual address, NULL if such page does not existed.
 */
page *lookup_hash(addr_t vaddr, page* head) {
	page *cur = head;
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
void add_time(page *cur, int cur_time) {
	node *interval = cur->t_interval;

	while (interval->next != NULL) {
		interval = interval->next;
	}

	interval->distance = cur_time - cur->last_occur;
	cur->last_occur = cur_time;
	node *new = init_node();
	interval->next = new;
}

/*
 * Delete time from time interval inside current page.
 */
int del_time(page *cur) {
	distance = (cur->t_interval)->distance;
	if (distance == 0) { // For debug only.
		return -1;
	} else if (distance == INF_TIME) {
		return 0;
	} else {
		distance = 0;
		node *head = cur->t_interval;
		cur->t_interval = (cur->t_interval)->next;
		free(head);
	}
	return 0;
}

// #define HASHSIZE 101
// static struct nlist *hashtab[HASHSIZE]; /* pointer table */

// Double linked list
typedef struct node_t {
	int distance;
	struct node *next;
} node;

/*
 * Initialize a new page with given page id, return a pointer to head.
 */
node *init_node() {
	node *head = malloc(sizeof(struct node));
	head->distance = INF_TIME;
	head->next = NULL;
	return head;
}

page *head = NULL;

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int distance = 0;
	int index;

	for (int i = 0; i < memsize; i++) {
		addr_t vaddr = coremap[i]->vaddr;
		page *cur_page = lookup_hash(vaddr, head);
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

	addr_t vaddr = coremap[p->frame >> PAGE_SHIFT]->vaddr;
	page *cur_page = lookup_hash(vaddr, head);
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

	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}

	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;

	while(fgets(buf, MAXLINE, infp) != NULL) {

		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			if(debug)  {
				printf("%c %lx\n", type, vaddr);
			}

			page *cur_page = lookup();
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

	if((tfp = fclose(tracefile)) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}
}
