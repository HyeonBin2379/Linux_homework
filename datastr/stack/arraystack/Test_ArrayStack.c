#include "ArrayStack.h"

int main(void)
{
	int i = 0;
	ArrayStack * stack = NULL;

	AS_createStack(&stack, 4);

	AS_push(stack, 3);
	AS_push(stack, 37);
	AS_push(stack, 11);
	AS_push(stack, 12);

	printf("Capacity : %d, Size: %d, Top: %d\n", stack->capacity, AS_getSize(stack), AS_top(stack));
	if (AS_isFull(stack))
		printf("The Stack is full!\n");

	for (int i = 0; i < 4; i++) {
		if (AS_isEmpty(stack))
			break;

		printf("Popped: %d, ", AS_pop(stack));

		if (!AS_isEmpty(stack))
			printf("Current Top: %d\n", AS_top(stack));
		else
			printf("Stack is Empty.\n");
	}

	AS_destroyStack(stack);

	exit(0);
}
