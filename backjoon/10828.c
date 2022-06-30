#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int ElementType;

typedef struct tagNode
{
	ElementType data;
} Node;
typedef struct tagArrayStack
{
	int capacity;
	int top;
	Node * nodes;
} ArrayStack;

void createStack(ArrayStack ** stack, int capacity)
{
	(*stack) = (ArrayStack *)malloc(sizeof(ArrayStack));
	(*stack)->nodes = (Node *)malloc(sizeof(Node) * capacity);
	(*stack)->capacity = capacity;
	(*stack)->top = 0;
}
void destroyStack(ArrayStack * stack)
{
	free(stack->nodes);
	free(stack);
}
void push(ArrayStack * stack, ElementType newData)
{
	int position = stack->top;

	stack->nodes[position].data = newData;
	(stack->top)++;
}
ElementType pop(ArrayStack * stack)
{
	int position = --(stack->top);

	if (position < 0)
		return -1;
	else
		return stack->nodes[position].data;
}
ElementType top(ArrayStack * stack)
{
	int position = stack->top - 1;

	if (position < 0)
		return -1;
	else
		return stack->nodes[position].data;
}
int getSize(ArrayStack * stack)
{
	return stack->top;
}
int isEmpty(ArrayStack * stack)
{
	return (stack->top == 0);
} 

int main(void)
{
	ArrayStack * stack;
	int len;
	char order[6];
	int data;

	scanf("%d", &len);
	createStack(&stack, 10000);

	if (stack->nodes != NULL) {
		for (int i = 0; i < len; i++)
		{
			scanf("%s", order);
			if (strcmp(order, "push") == 0)
			{
				scanf("%d", &data);
				push(stack, data);
			}
			else if (strcmp(order, "pop") == 0)
			{
				if (!isEmpty(stack))
					printf("%d\n", pop(stack));
				else
					printf("%d\n", -1);
			}
			else if (strcmp(order, "top") == 0)
			{
				if (!isEmpty(stack))
					printf("%d\n", top(stack));
				else
					printf("%d\n", -1);
			}
			else if (strcmp(order, "size") == 0)
			{
				printf("%d\n", getSize(stack));
			}
			else if (strcmp(order, "empty") == 0)
			{
				printf("%d\n", isEmpty(stack));
			}
		}

		destroyStack(stack);
	}
	exit(0);
}
