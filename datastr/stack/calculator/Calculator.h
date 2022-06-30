#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdlib.h>
#include "LinkedListStack.h"

typedef enum
{
	LEFT_PARENTHESIS = '(',
	RIGHT_PARENTHESIS = ')',
	PLUS = '+',
	MINUS = '-',
	MULTIPLY = '*',
	DIVIDE = '/',
	SPACE = ' ',
	OPERAND
} SYMBOL;

int isNumber(char Chiper);
unsigned int getNextToken(char * exp, char * token, int * TYPE);
int isPrior(char operator1, char operator2);
void getPostfix(char * infixExp, char * postfixExp);
double calculate(char * postfixExp);

#endif
