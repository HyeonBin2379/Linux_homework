#include "ArrayStack.h"

void AS_createStack(ArrayStack ** stack, int capacity)
{
	(*stack) = (ArrayStack *)malloc(sizeof(ArrayStack));
	(*stack)->nodes = (Node *)malloc(sizeof(Node)*capacity);

	(*stack)->capacity = capacity;
	(*stack)->top = 0;
}
void AS_destroyStack(ArrayStack * stack)
{
	free(stack->nodes);
	free(stack);
}

void AS_push(ArrayStack * stack, ElementType data)
{
	int position = stack->top;

	stack->nodes[position].data = data;
	(stack->top)++;
}

ElementType AS_pop(ArrayStack * stack)
{
	int position = --(stack->top);

	return stack->nodes[position].data;
}

ElementType AS_top(ArrayStack * stack)
{
	return stack->nodes[stack->top].data;
}

int AS_getSize(ArrayStack * stack)
{
	return stack->top;
}

int AS_isEmpty(ArrayStack * stack)
{
	return (stack->top == 0);
}

int AS_isFull(ArrayStack * stack)
{
	return (stack->top == stack->capacity);
}
