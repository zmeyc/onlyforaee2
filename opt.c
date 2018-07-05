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
    // addr_t id; // Last 20 bits of virtual address
	int next_ref; // Store index of the next reference of this page, -1 if none
} Page;

typedef struct node {
	addr_t id; // Last 20 bits of virtual address
	int ref_t; // Store reference time of id
	struct node *next; // Pointer to the next node
} Node; // Double linked list node inside hash table

typedef struct entry { // Each entry of hash_table
	Node *head;
	Node *tail;
} Entry;

// Define the global variables
Page *page_list;
Entry *hash_table;
int time = 0;
int size = 0; // Store the size of the tracefile to initialize the page_list

/*
 * Initialize the hash table and return it.
 */
Entry *init_hash(int size) {
	Entry *table = malloc(sizeof(Entry) * size);

	for (int i = 0; i < size; i++) {
		table[i].head = NULL;
		table[i].tail = NULL;
	}

	return table;
}

/*
 * Initialize each node with given virtual address vaddr
 * and the reference time r_time.
 */
Node *init_node(addr_t vaddr, int r_time) {
	Node *new_node = malloc(sizeof(Node));
	new_node->id = vaddr;
	new_node->ref_t = r_time;
	new_node->next = NULL;

	return new_node;
}

/*
 * Return the index of current page in hash table.
 */
int hash_fcn(addr_t vaddr) {
	return (int) (vaddr % size);
}

/*
 * Return the node with virtual address vaddr using
 * the hash function hash_fcn if this node exists.
 * Otherwise, return NULL.
 */
Node *lookup_vaddr(addr_t vaddr) {
	Node *head = hash_table[hash_fcn(vaddr)].head;

	while (head != NULL) {
		if (head->id == vaddr) {
			return head;
		}
		head = head->next;
	}

	return NULL;
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
	coremap[p->frame >> PAGE_SHIFT].next_ref_time = page_list[time].next_ref;
	time++;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
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
	hash_table = init_hash(size);

	// Read the lines
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			if (debug) {
				printf("%c %lx\n", type, vaddr);
			}

			// page_list[index].id = vaddr;
			page_list[index].next_ref = INF;

			Node *cur_node = lookup_vaddr(vaddr);
			Node *new_node = init_node(vaddr, index);
			if (cur_node == NULL) { // First time allocate current page
				if (hash_table[hash_fcn(vaddr)].head == NULL) { // No page before->head
					hash_table[hash_fcn(vaddr)].head = new_node;
				} else {
					(hash_table[hash_fcn(vaddr)].tail)->next = new_node;
				}
				hash_table[hash_fcn(vaddr)].tail = new_node;

			} else {
				page_list[cur_node->ref_t].next_ref = index;
				cur_node->ref_t = index;
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
