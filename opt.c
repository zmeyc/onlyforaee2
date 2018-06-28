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

#define INF_TIME -1

// Define data structures

// Linked list to store time that such page been called.
typedef struct node {
	int call_time;
	struct node *next;
} Node;

/*
 * Initialize a new node with the given page id, return a pointer to head.
 */
Node *init_node() {
	Node *head = malloc(sizeof(Node));
	head->call_time = INF_TIME;
	head->next = NULL;
	return head;
}

// Hash table (node)
typedef struct page {
		addr_t id; // Last 20 bits of virtual address
		// Linked list store the calling time of this page.
		Node *ctime_list; // Pointer to the head of the linked list.
		struct page *next_page;
} Page;

/*
 * Initialize a new page with given page id, return a pointer to the new page.
 */
Page *init_hash(addr_t vaddr, int o_time, Page *head) {
	Page *new = malloc(sizeof(Page));
	new->id = vaddr >> PAGE_SHIFT;
	Node *ctime_list = init_node();
	ctime_list->call_time = o_time;
	Node *next = init_node();
	ctime_list->next = next;
	new->ctime_list = ctime_list;
	new->next_page = head;
	return new;
}

/*
 * Look up the time interval for the given virtual address. Return a pointer to
 * the page for the given virtual address, NULL if such page does not exist.
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
 * Set up calling time for the current page when this page is called.
 */
void add_time(Page *cur, int cur_time) {
	Node *interval = cur->ctime_list;

	while (interval->next != NULL) {
		interval = interval->next;
	}

	interval->call_time = cur_time;
	Node *new = init_node();
	interval->next = new;
}

/*
 * Delete time from time interval inside current page.
 */
int del_time(Page *cur) {
	int c_time = (cur->ctime_list)->call_time;

	if (c_time == INF_TIME) {
		return -1;
	} else {
		Node *head = cur->ctime_list;
		cur->ctime_list = (cur->ctime_list)->next;
		free(head);
	}
	return 0;
}

int check_next_call_time(Page *cur_page) {
	return (cur_page->ctime_list)->call_time;
}

Page *head = NULL;
int cur_time = 0;

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int last_call = 0;
	int index;

	for (int i = 0; i < memsize; i++) {
		addr_t vaddr = coremap[i].vaddr;
		Page *cur_page = lookup_hash(vaddr, head);

		if (cur_page == NULL) {
			printf("Current pages with virtual address %lu does not existed in hash.\n", vaddr);
		}

		int c_time = check_next_call_time(cur_page);

		if (c_time == INF_TIME) {
			return i;
		} else if (c_time > last_call) {
			last_call = c_time;
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
	if (cur_page == NULL) {
		printf("Current pages with virtual address %lu does not existed in hash.\n", vaddr);
	}

	if (cur_time != check_next_call_time(cur_page)) {
		printf("Page recorded incorrect calling time.\n");
	}

	int result = del_time(cur_page);

	if (result != 0) { // For debug only
		printf("Try to delete time from interval with zero content.\n");
	}

	cur_time++;
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
