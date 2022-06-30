#include "LinkedListStack.h"

void LLS_createStack(LinkedListStack ** stack)
{
	(*stack) = (LinkedListStack *)malloc(sizeof(LinkedListStack));
	(*stack)->list = NULL;
	(*stack)->top = NULL;
}
void LLS_destroyStack(LinkedListStack * stack)
{
	while (!LLS_isEmpty(stack))
	{
		Node * popped = LLS_pop(stack);
		free(popped);
	}

	free(stack);
}

Node * LLS_createNode(ElementType newData)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = (ElementType)malloc(strlen(newData) + 1);

	strcpy(newNode->data, newData);
	newNode->nextNode = NULL;

	return newNode;
}

void LLS_destroyNode(Node * node)
{
	free(node->data);
	free(node);
}

void LLS_push(LinkedListStack * stack, Node * newNode)
{
	if (stack->list == NULL)
	{
		stack->list = newNode;
	}
	else
	{
		Node * oldTop = stack->list;

		while(oldTop->nextNode != NULL)
		{
			oldTop = oldTop->nextNode;
		}

		oldTop->nextNode = newNode;
	}

	stack->top = newNode;
}

Node * LLS_pop(LinkedListStack * stack)
{
	Node * topNode = stack->top;

	if (stack->list == stack->top)
	{
		stack->list = NULL;
		stack->top = NULL;	
	}
	else
	{
		Node * currentTop = stack->list;

		while (currentTop != NULL && currentTop->nextNode != stack->top)
		{
			currentTop = currentTop->nextNode;
		}

		stack->top = currentTop;
		currentTop->nextNode = NULL;
	}

	return topNode;
}

Node * LLS_top(LinkedListStack * stack)
{
	return stack->top;
}

int LLS_getSize(LinkedListStack * stack)
{
	int count = 0;
	Node * current = stack->list;

	while (current != NULL) 
	{
		count++;
		current = current->nextNode;
	}

	return count;
}

int LLS_isEmpty(LinkedListStack * stack)
{
	return (stack->list == NULL);
}
