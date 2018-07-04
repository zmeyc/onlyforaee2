#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

typedef struct node_t {
	unsigned int frame; // PFN which is the index of the coremap
	struct node_t *previous, *next;
} Node;

Node *head; // Most recent used page (new node inserts to here)
Node *tail; // Least recent used page (victim node evicts from here)
Node **hash_table; // Array of pointers to the nodes


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int lru_evict() {
	Node *evict_node = tail;
	int evict_PFN = evict_node->frame;

	// Remove the pointer to the evicted node from the hash_table
	hash_table[evict_PFN] = NULL;

	// Updated the double linked list
	if (head == tail) { // case1: Only one node in the double linked list
		head = tail = NULL;
	} else { // case2: At least 2 nodes in the double linked list
		Node *new_tail = evict_node->previous;
		new_tail->next = NULL;
		tail = new_tail;
	}

	// free the evict node
	free(evict_node);

	return evict_PFN;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	// Construct a new node, and get the PFN of the accessed page
	Node *new_node = (Node *) malloc(sizeof(Node));
	unsigned int frame_index = p->frame >> PAGE_SHIFT;

	// Initialize the attributes of the new node
	new_node->frame = frame_index;
	new_node->previous = new_node->next = NULL;

	// Get the pointer to the accessed page in the hash_table
	Node *target = hash_table[frame_index];

	// Check the accessed page hit or miss
	if (target != NULL) { // Hit, need to be removed from the list
		if ((head == target) && (tail == target)) {
			head = tail = NULL;
		} else if (tail == target) {
			Node *new_tail = target->previous;
			new_tail->next = NULL;
			tail = new_tail;
		} else if (head == target) {
			Node *new_head = target->next;
			new_head->previous = NULL;
			head = new_head;
		} else {
			Node *target_prev = target->previous;
			Node *target_next = target->next;
			target_prev->next = target_next;
			target_next->previous = target_prev;
		}

		free(target);
	}

	// Insert new_node into the head of double linked list
	if (tail == NULL) {
		head = tail = new_node;
	} else {
		Node *old_head = head;
		old_head->previous = new_node;
		new_node->next = old_head;
		head = new_node;
	}

	// Update the pointer in the hash_table to point to the new_node
	hash_table[frame_index] = new_node;
}


/* Initialize any data structures needed for this
 * replacement algorithm
 */
void lru_init() {
	// Initialize head and tail
	head = tail = NULL;

	// Allocate the space on the heap for the hash_table
	hash_table = (Node **) malloc(sizeof(Node *) * memsize);

	// Initialize all the pointers in the hash_table to NULL
	for (int i = 0; i < memsize; i++) {
		hash_table[i] = NULL;
	}
}

