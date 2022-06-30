#ifndef ARRAYSTACK_H
#define ARRAYSTACK_H

#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;

typedef struct tagDataNode
{
	ElementType data;
} Node;

typedef struct tagArrayStack
{
	int capacity;
	int top;
	Node * nodes;
} ArrayStack;

void AS_createStack(ArrayStack ** stack, int capacity);
void AS_destroyStack(ArrayStack * stack);
void AS_push(ArrayStack * stack, ElementType data);
ElementType AS_pop(ArrayStack * stack);
ElementType AS_top(ArrayStack * stack);
int AS_getSize(ArrayStack * stack);
int AS_isEmpty(ArrayStack * stack);
int AS_isFull(ArrayStack * stack);

#endif
