#include <stdio.h>
#include <stdlib.h>
#include "Calculator.h"

int main(void)
{
	char infixExp[100];
	char postfixExp[100];

	double result = 0.0;

	memset(infixExp, 0, sizeof(infixExp));
	memset(postfixExp, 0, sizeof(postfixExp));

	printf("Enter infix Expression: ");
	scanf("%s", infixExp);

	getPostfix(infixExp, postfixExp);

	printf("Infix: %s\nPostfix: %s\n", infixExp, postfixExp);

	result = calculate(postfixExp);
	printf("Calculation Result: %f\n", result);

	exit(0);
}
