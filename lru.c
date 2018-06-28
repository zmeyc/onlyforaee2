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
	unsigned int frame; // PFN which is the index of coremap
	struct node_t *previous, *next;
} Node;

typedef struct Hash {
	int capacity; // the capacity of the hash table
	Node **array; // array of nodes
} Hash;

Node *head; // most recent used page (insert)
Node *tail; // least recent used page (evict)
Hash *hash_table;


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	Node *evict_node = tail;
	int evict_PFN = evict_node->frame;

	// remove from hash table
	hash_table->array[evict_PFN] = NULL;

	// updated double linked list
	if (head == tail) { // case1: only one node
		head = tail = NULL;
	} else { // case2: at least 2 node
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
	// construct a new node
	Node *new_node = (Node *) malloc(sizeof(Node));
	unsigned int frame_index = p->frame >> PAGE_SHIFT;
	// initialize new node
	new_node->frame = frame_index;
	new_node->previous = new_node->next = NULL;

	// check hit ot miss
	Node *target = hash_table->array[frame_index];
	if (target != NULL) { // hit
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

	// insert new_node into the head of double linked list
	if (tail == NULL) { // first page
		head = tail = new_node;
	} else {
		Node *old_head = head;
		old_head->previous = new_node;
		new_node->next = old_head;
		head = new_node;
	}

	// update hash_table
	hash_table->array[frame_index] = new_node;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// initialize head and tail
	head = tail = NULL;

	// initialize hash_table
	hash_table = (Hash *) malloc(sizeof(Hash));
	hash_table->capacity = memsize;
	hash_table->array = (Node **) malloc(sizeof(Node *) * hash_table->capacity);

	for (int i = 0; i < hash_table->capacity; i++) {
		hash_table->array[i] = NULL;
	}
}

