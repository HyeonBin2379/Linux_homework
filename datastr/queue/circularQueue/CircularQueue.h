#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;

typedef struct tagNode
{
	ElementType data;
} Node;
typedef struct tagCircularQueue
{
	int capacity;
	int front;
	int rear;
	Node * nodes;
} CircularQueue;

void CQ_createQueue(CircularQueue ** queue, int capacity);
void CQ_destroyQueue(CircularQueue * queue);

void CQ_enqueue(CircularQueue * queue, ElementType data);
ElementType CQ_dequeue(CircularQueue * queue);

int CQ_getSize(CircularQueue * queue);
int CQ_isEmpty(CircularQueue * queue);
int CQ_isFull(CircularQueue * queue);

#endif
