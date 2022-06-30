#include "LinkedListStack.h"

int main(void)
{
	int i = 0;
	int count = 0;
	Node * popped;

	LinkedListStack * stack;

	LLS_createStack(&stack);

	LLS_push(stack, LLS_createNode("abc"));
	LLS_push(stack, LLS_createNode("def"));
	LLS_push(stack, LLS_createNode("efg"));
	LLS_push(stack, LLS_createNode("hij"));

	count = LLS_getSize(stack);
	printf("Size: %d, Top: %s\n\n", count, LLS_top(stack)->data);

	for (i = 0; i < count; i++)
	{
		if (LLS_isEmpty(stack))
			break;

		popped = LLS_pop(stack);
		printf("Popped: %s, ", popped->data);

		LLS_destroyNode(popped);

		if (!LLS_isEmpty(stack))
			printf("Current Top: %s\n", LLS_top(stack)->data);
		else
			printf("Stack is empty.\n");
	}

	LLS_destroyStack(stack);
	exit(0);
}
