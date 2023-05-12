#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

// the queue will consist of nodes, each pointing ahead and behind in the queue
// the data member is the actual data that the queue is supposed to contain
struct node {
	void* data;
	struct node* ahead;
	struct node* behind;
};

// the queue keeps track of which nodes are at the front and back
// the ahead and behind pointers on each node keep track of the rest
// size is a signed int because that is the data type required by queue_length
struct queue {
	struct node* front;
	struct node* back;
	int* size;
};

// queue_t is a queue pointer, which is what will be passed in for every queue function
// this is to implement pass-by-reference instead of pass-by-value
typedef struct queue* queue_t;

// dynamically allocates a queue and initializes its size
// because it has no elements, the front and back remain null
queue_t queue_create(void)
{
	queue_t q = malloc(sizeof(queue_t));
	q->front = NULL;
	q->back = NULL;
	q->size = malloc(sizeof(q->size));
	*(q->size) = 0;
	return q;

}

// deallocates an empty queue
// does nothing if the queue is not empty
int queue_destroy(queue_t queue)
{
	// if (queue == NULL || queue->size == NULL || *(queue->size) != 0)
	// 	return -1;

	if (queue == NULL){
		return -1;
	}

	if (queue->size == NULL){
		return -1;
	}

	if (*(queue->size) != 0){
		return -1;
	}
	free(queue->size);
	free(queue);
	return 0;
}

// add element to back of queue
int queue_enqueue(queue_t queue, void *data)
{
	// null check
	if (queue == NULL || data == NULL){
		return -1;
	}

	// if the queue was empty, the new element must become both the front and back
	if (*(queue->size) == 0){
		struct node* queue_element1 = malloc(sizeof(struct node));
		queue_element1->data = data;
		queue_element1->ahead = NULL;
		queue_element1->behind = NULL;
		queue->front = queue_element1;
		queue->back = queue_element1;
	} else {
		struct node* queue_element_next = malloc(sizeof(struct node));
		queue_element_next->data = data;

		// reattach pointers to account for the new element
		queue_element_next->behind = NULL;
		queue_element_next->ahead = queue->back;
		queue->back->behind = queue_element_next;
		queue->back = queue_element_next;
	}

	*(queue->size) += 1;

	return 0;
}

// return the front of the queue without removing it
int queue_peek(queue_t queue, void** data)
{
	// null check
	if (queue == NULL || data == NULL){
		return -1;
	}

	*data = queue->front->data;

	return 0;
}

// remove element from front of queue and store it in the data parameter
int queue_dequeue(queue_t queue, void **data)
{
	// null checker
	if (queue == NULL || data == NULL || *(queue->size) == 0){
		return -1;
	}

	// if the queue will become empty, set the front and back to null
	if (*(queue->size) == 1){
		*data = queue->front->data;
		// printf("about to free %p in queue_dequeue\n", queue->front);
		free(queue->front);
		queue->front = NULL;
		queue->back = NULL;
		*(queue->size) = 0;
	} else {
		*data = queue->front->data;
		struct node* temp = queue->front;
		// printf("about to free %p in queue_dequeue\n", queue->front);
		free(queue->front);
		// printf("freed %p in queue_dequeue\n", temp);

		// reattach pointers to account for the deleted node
		queue->front = temp->behind;
		queue->front->ahead = NULL;
		*(queue->size) -= 1;
	}

	return 0;
}

// remove arbitrary element from the queue
int queue_delete(queue_t queue, void *data)
{
	// null checker
	if (queue == NULL || data == NULL || *(queue->size) == 0){
		return -1;
	}

	// if the queue only has one element, it has special behavior already defined in queue_dequeue
	// if it is the only element, it must be at the front, so queue_dequeue will definitely remove it
	if (*(queue->size) == 1){
		return queue_dequeue(queue, &data);
	}

	// use current to iterate through the queue, from front to back
	// since the node behind the back is always null, the back is always the last element to be iterated through, ending right after
	struct node* current = queue->front;
	while (current != NULL){
		if (current->data == data){
			// delete the node after setting a backup variable used to reset the pointers
			struct node* checked_node = current;
			free(current);

			// if the node is in the front, its ahead must be null
			if (checked_node == queue->front){
				queue->front = checked_node->behind;
				queue->front->ahead = NULL;
			}

			// likewise, if it's in the back, its behind must be null
			else if (checked_node == queue->back){
				queue->back = checked_node->ahead;
				queue->back->behind = NULL;
			}

			else {
				checked_node->behind->ahead = checked_node->ahead;
				checked_node->ahead->behind = checked_node->behind;
			}

			*(queue->size) -= 1;
			return 0;
		}

		// iterate by moving from front to back
		current = current->behind;
	}

	return -1;
}

// run function on every element in the queue
int queue_iterate(queue_t queue, queue_func_t func)
{
	// null checker
	if (queue == NULL || func == NULL){
		return -1;
	}
	
	// start at the front
	struct node* current = queue->front;

	// run the function on every element
	while (current != NULL){
		func(queue, current->data);
		current = current->behind;
	}

	return 0;
}

// return the queue size
int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL){
		return -1;
	}

	return *(queue->size);
}

// move element at front of queue to back of queue
void queue_move_front_to_back(queue_t queue)
{
	void* moved_element;
	queue_dequeue(queue, &moved_element);
	queue_enqueue(queue, moved_element);
}