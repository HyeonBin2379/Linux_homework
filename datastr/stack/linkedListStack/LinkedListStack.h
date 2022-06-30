#ifndef LINKEDLIST_STACK_H
#define LINKEDLIST_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* ElementType;

typedef struct tagNode
{
	ElementType data;
	struct tagNode* nextNode;	
} Node;

typedef struct tagLinkedListStack
{
	Node * list;
	Node * top;
} LinkedListStack;

void LLS_createStack(LinkedListStack ** stack);
void LLS_destroyStack(LinkedListStack * stack);

Node * LLS_createNode(ElementType data);
void LLS_destroyNode(Node * node);

void LLS_push(LinkedListStack * stack, Node * newNode);
Node * LLS_pop(LinkedListStack * stack);

Node * LLS_top(LinkedListStack * stack);
int LLS_getSize(LinkedListStack * stack);
int LLS_isEmpty(LinkedListStack * stack);

#endif
