#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
	void* data;
	void** ahead;
	void** behind;
};

struct queue {
	/* TODO Phase 1 */
	struct node* front;
	struct node* back;
	unsigned size;
};

typedef struct queue* queue_t;

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	queue_t q1 = malloc(sizeof(queue_t));
	q1->size = 0;
	return q1;

}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL || queue->size != 0)
		return -1;
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if (queue == NULL || data == NULL)

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
		queue_element_next->behind = NULL;
		queue_element_next->ahead = queue->back;
		queue->back->behind = queue_element_next;
		queue->back = queue_element_next;
	}

	queue->size++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
	if (queue == NULL || data == NULL || queue->size == 0){
		return -1;
	}

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
		queue->front = temp->behind;
		queue->front->ahead = NULL;
		queue->size--;
	}

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
}

