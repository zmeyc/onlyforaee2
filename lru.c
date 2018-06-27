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
	unsigned int frame; // PFN which is also the index of the coremap
	struct Node *previous, *next;
} Node;

typedef struct Queue {
	Node *head; // most recent used page (insert)
	Node *tail; // least recent used page (evict)
} List;

typedef struct Hash {
	int capacity; // the capacity of the hash table which is memsize
	Node **array; // array of nodes
} Hash;

List *llist;
Hash *hash_table;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	// get evicted node and its PFN
	Node *evict_node = llist->tail;
	int evict_PFN = evict_node->frame;

	// remove from hash table
	hash_table->array[evict_PFN] = NULL;

	// updated double linked list
	if (llist->head == llist->tail) { // case1: only one node
		llist->head = llist->tail = NULL;
	} else { // case2: at least 2 node
		Node *new_tail = (Node *) evict_node->previous;
		new_tail->next = NULL;
		llist->tail = new_tail;
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
	// construct a new node, get its PFN
	Node *new_node = (Node *) malloc(sizeof(Node));
	unsigned int frame_index = p->frame >> PAGE_SHIFT;

	// initialize new node
	new_node->frame = frame_index;
	new_node->previous = new_node->next = NULL;

	if (hash_table->array[frame_index] != NULL) { // hit
		Node *target = hash_table->array[frame_index];

		if (llist->tail == target) { // target is the tail of the llist
			llist->tail = (Node *) target->previous;
		} else { // target is in the middle of the llist
			Node *target_prev = (Node *) target->previous;
			Node *target_next = (Node *) target->next;
			target_prev->next = target_next;
			target_next->previous = target_prev;
		}

		// free the hit node 
		free(target);
	}

	// insert new node to the head of llist
	new_node->next = llist->head;
	llist->head = new_node;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// initialize llist
	llist = (List *) malloc(sizeof(List));
	llist->head = llist->tail = NULL;

	// initialize hash_table
	hash_table = (Hash *) malloc(sizeof(Hash));
	hash_table->capacity = memsize;
	hash_table->array = (Node **) malloc(sizeof(Node *));

	// initialize each entry of the hash_table
	for (int i = 0; i < hash_table->capacity; i++) {
		hash_table->array[i] = NULL;
	}
}
