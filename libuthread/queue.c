#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

// the queue will consist of nodes, each pointing ahead and behind in the queue
// the data member is the actual data that the queue is supposed to contain
// the ahead pointer is currently unused, but may become useful later
struct node {
	void* data;
	struct node* ahead;
	struct node* behind;
};

// the queue keeps track of which nodes are at the front and back
// the ahead and behind pointers on each node keep track of the rest
struct queue {
	struct node* front;
	struct node* back;
	unsigned size;
};

typedef struct queue* queue_t;

queue_t queue_create(void)
{
	queue_t q1 = malloc(sizeof(queue_t));
	q1->size = 0;
	return q1;

}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->size != 0)
		return -1;
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
	if (queue->size == 0){
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

	queue->size++;

	return 0;
}

// remove element from front of queue and store it in the data parameter
int queue_dequeue(queue_t queue, void **data)
{
	// null checker
	if (queue == NULL || data == NULL || queue->size == 0){
		return -1;
	}

	// if the queue will become empty, set the front and back to null
	if (queue->size == 1){
		*data = queue->front;
		free(queue->front);
		queue->front = NULL;
		queue->back = NULL;
		queue->size = 0;
	} else {
		*data = queue->front;
		struct node* temp = queue->front;
		free(queue->front);

		// reattach pointers to account for the deleted node
		queue->front = temp->behind;
		queue->front->ahead = NULL;
		queue->size--;
	}

	return 0;
}

// remove arbitrary element from the queue
int queue_delete(queue_t queue, void *data)
{
	// null checker
	if (queue == NULL || data == NULL){
		return -1;
	}

	// use current to iterate through the queue, from front to back
	// since the node behind the back is always null, the back is always the last element to be iterated through, ending right after
	struct node* current = queue->front;
	while (current != NULL){
		if (current->data == data){
			struct node* temp = current;
			free(current);
			temp->behind->ahead = temp->ahead;
			temp->ahead->behind = temp->behind;
			queue->size--;
			return 0;
		}

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

	return queue->size;
}