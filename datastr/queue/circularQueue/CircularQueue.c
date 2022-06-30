#include "CircularQueue.h"

void CQ_createQueue(CircularQueue ** queue, int capacity)
{
	(*queue) = (CircularQueue *)malloc(sizeof(CircularQueue));
	(*queue)->nodes = (Node *)malloc(sizeof(Node) * (capacity+1));
	(*queue)->capacity = capacity;
	(*queue)->front = 0;
	(*queue)->rear = 0;
}
void CQ_destroyQueue(CircularQueue * queue)
{
	free(queue->nodes);
	free(queue);
}

void CQ_enqueue(CircularQueue * queue, ElementType data)
{
	int position = 0;

	if (queue->rear == queue->capacity)
	{
		position = queue->rear;
		queue->rear = 0;
	}
	else
		position = queue->rear++;

	queue->nodes[position].data = data;
}
ElementType CQ_dequeue(CircularQueue * queue)
{
	int position = queue->front;

	if (queue->front == queue->capacity)
		queue->front = 0;
	else
		queue->front++;

	return queue->nodes[position].data;
}

int CQ_getSize(CircularQueue * queue)
{
	if (queue->front <= queue->rear)
		return (queue->rear - queue->front);
	else
		return queue->rear + (queue->capacity - queue->front) + 1;
}
int CQ_isEmpty(CircularQueue * queue)
{
	return (queue->rear == queue->front);
}
int CQ_isFull(CircularQueue * queue)
{
	if (queue->front < queue->rear)
		return (queue->rear - queue->front) == queue->capacity;
	else
		return (queue->rear + 1) == queue->front;
}
