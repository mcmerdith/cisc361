#include "mp3.h"

extern node_t *head;

void delete(int data) {
	// The address of the start node (head)
	node_t **curr = &head, *temp;

	while (*curr) {
		if ((*curr)->data != data) {   // Check if the current node should be deleted
			curr = &(*curr)->next; // Move the address pointer to the address of the next element pointer
			continue;              // Move to the next item
		}

		// Remove this node
		
		temp = *curr;          // Store a reference to the node to be deleted
		*curr = (*curr)->next; // Update the pointer to the next item

		// Free the blocks
		
		free(temp->name);
		free(temp);
	}
}

