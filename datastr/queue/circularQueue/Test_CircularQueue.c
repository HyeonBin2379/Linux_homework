#include "CircularQueue.h"

int main(void)
{
	int i;
	CircularQueue * queue;

	CQ_createQueue(&queue, 10);

	CQ_enqueue(queue, 1);
	CQ_enqueue(queue, 2);
	CQ_enqueue(queue, 3);
	CQ_enqueue(queue, 4);

	for (i = 0; i < 3; i++)
	{
		printf("Dequeue: %d, ", CQ_dequeue(queue));
		printf("Front: %d, Rear: %d\n", queue->front, queue->rear);
	}

	i = 100;
	while (CQ_isFull(queue) == 0)
	{
		CQ_enqueue(queue, i++);
	}

	printf("Capacity: %d, Size: %d\n\n", queue->capacity, CQ_getSize(queue));

	while (CQ_isEmpty(queue) == 0)
	{
		printf("Dequeue: %d, ", CQ_dequeue(queue));
		printf("Front: %d, Rear: %d\n", queue->front, queue->rear);
	}

	CQ_destroyQueue(queue);

	exit(0);
}
