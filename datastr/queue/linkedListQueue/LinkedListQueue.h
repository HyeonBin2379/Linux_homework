#ifndef LINKEDLIST_QUEUE_H
#define LINKEDLIST_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char * ElementType;

typedef struct tagNode
{
	ElementType data;
	struct tagNode * nextNode;
} Node;
typedef struct tagLinkedListQueue
{
	Node * front;
	Node * rear;
	int count;
} LinkedListQueue;

void LQ_createQueue(LinkedListQueue ** queue);
void LQ_destroyQueue(LinkedListQueue * queue);

Node * LQ_createNode(ElementType newData);
void LQ_destroyNode(Node * node);

void LQ_enqueue(LinkedListQueue * queue, Node * newNode);
Node * LQ_dequeue(LinkedListQueue * queue);

int LQ_isEmpty(LinkedListQueue * queue);

#endif
