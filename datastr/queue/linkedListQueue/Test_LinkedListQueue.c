#include "LinkedListQueue.h"

int main(void)
{
	Node * popped;
	LinkedListQueue * queue;

	LQ_createQueue(&queue);

	LQ_enqueue(queue, LQ_createNode("abc"));
	LQ_enqueue(queue, LQ_createNode("def"));
	LQ_enqueue(queue, LQ_createNode("efg"));
	LQ_enqueue(queue, LQ_createNode("hij"));

	printf("Queue Size: %d\n", queue->count);

	while (!LQ_isEmpty(queue))
	{
		popped = LQ_dequeue(queue);
		printf("Dequeue: %s\n", popped->data);
		
		LQ_destroyNode(popped);
	}

	LQ_destroyQueue(queue);
	exit(0);
}
