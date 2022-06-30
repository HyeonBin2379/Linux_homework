#include "LinkedListQueue.h"

void LQ_createQueue(LinkedListQueue ** queue)
{
	(*queue) = (LinkedListQueue *)malloc(sizeof(LinkedListQueue));
	(*queue)->front = NULL;
	(*queue)->rear = NULL;
	(*queue)->count = 0;
}
void LQ_destroyQueue(LinkedListQueue * queue)
{
	while (!LQ_isEmpty(queue))
	{
		Node * popped = LQ_dequeue(queue);
		LQ_destroyNode(popped);
	}

	free(queue);
}

Node * LQ_createNode(ElementType newData)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = (ElementType)malloc(strlen(newData) + 1);

	strcpy(newNode->data, newData);
	newNode->nextNode = NULL;

	return newNode;
}
void LQ_destroyNode(Node * node)
{
	free(node->data);
	free(node);
}

void LQ_enqueue(LinkedListQueue * queue, Node * newNode)
{
	if (queue->front == NULL)
	{
		queue->front = newNode;
		queue->rear = newNode;
		queue->count++;
	}
	else
	{
		queue->rear->nextNode = newNode;
		queue->rear = newNode;
		queue->count++;
	}
}
Node * LQ_dequeue(LinkedListQueue * queue)
{
	Node * prev = queue->front;

	if (queue->front->nextNode == NULL)
	{
		queue->front = NULL;
		queue->rear = NULL;
	}
	else
	{
		queue->front = queue->front->nextNode;
	}
	queue->count--;

	return prev;
}

int LQ_isEmpty(LinkedListQueue * queue)
{
	return (queue->front == NULL);
}
