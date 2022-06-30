#include <stdio.h>
#include <stdlib.h>

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

	return stack->nodes[position].data;
}
ElementType top(ArrayStack * stack)
{
	int position = stack->top-1;
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
	int cnt;
	int sum = 0;

	createStack(&stack, 100000);

	scanf("%d", &cnt);

	for (int i = 0; i < cnt; i++)
	{
		int num;

		scanf("%d", &num);
		if (num != 0) {
			sum += num;
			push(stack, num);
		}
		else {
			sum -= pop(stack);
		}
		//printf("top: %d\n", top(stack));
	}

	printf("%d\n", sum);

	destroyStack(stack);
	exit(0);
}
