#include "Calculator.h"
/* 숙제: 음수에 대해서도 계산 가능하도록 변환하기 */
/* 	 백준 스택 관련 문제 풀기 */
// 숫자 판별용 표본(단, 양수만 가능)
char NUMBER[] = {'0','1','2','3','4','5','6','7','8','9','.'};

// 입력한 식에서 현재 문자형이 숫자 혹은 소수점인지 확인 
int isNumber(char cipher)
{
	int i = 0;
	int arrayLength = sizeof(NUMBER);

	for (i = 0; i < arrayLength; i++)
	{
		if (cipher == NUMBER[i])
			return 1;
	}

	return 0;
}

// 입력한 수식에서 토큰 생성
unsigned int getNextToken(char * exp, char * token, int * TYPE)
{
	unsigned int i = 0;

	for (i = 0; exp[i] != 0; i++)
	{
		token[i] = exp[i];

		if (isNumber(exp[i]) == 1)	// 숫자, 소수점인 경우
		{
			*TYPE = OPERAND;
			if (isNumber(exp[i+1]) != 1)
				break;
		}
		else	// 연산 기호인 경우
		{
			*TYPE = exp[i];		// 연산기호에 해당하는 아스키값으로 변환
			break;
		}
	}

	token[++i] = '\0';	// 한 토큰의 끝 지정
	return i;		// 한 토큰의 길이값 리턴
}

// 우선순위 판별 기준
int getPriority(char operator, int inStack)
{
	int priority = -1;

	switch(operator)
	{
	// 스택 내에서는 최하위, 토큰 내에서는 최우선
	case LEFT_PARENTHESIS:
		if (inStack)
			priority = 3;
		else
			priority = 0;
		break;
	case MULTIPLY:	// 곱셈/나눗셈 우선순위
	case DIVIDE:
		priority = 1;
		break;
	case PLUS:	// 덧셈/뺄셈 우선순위
	case MINUS:
		priority = 2;
		break;
	}

	return priority;
}

// 스택 최상단의 연산자, 한 토큰의 연산자 간 우선순위 비교
int isPrior(char operatorInStack, char operatorInToken)
{
	return (getPriority(operatorInStack, 1) > getPriority(operatorInToken, 0));
}

// 중위표기법을 후위표기법으로 변환
void getPostfix(char * infixExp, char * postfixExp)
{
	LinkedListStack * stack;
	
	char token[32];
	int type = -1;
	unsigned int position = 0;	// offset
	unsigned int length = strlen(infixExp);

	LLS_createStack(&stack);

	/* 중위 표기식을 다 읽을 때까지 */
	while (position < length) 
	{
		// 토큰의 길이만큼 offset 증가
		position += getNextToken(&infixExp[position], token, &type);

		if (type == OPERAND) {	
			// 토큰이 피연산자이면 후위표기식에 출력 
			strcat(postfixExp, token);
			strcat(postfixExp, " ");
		}
		else if (type == RIGHT_PARENTHESIS) {
			// 토큰이 오른쪽 괄호이면 왼쪽 괄호가 top이 될 때까지 스택에서 pop
			// 괄호가 한 짝만 있는 경우에는 괄호 무시
			while (!LLS_isEmpty(stack)) {
				Node * popped = LLS_pop(stack);

				// data[0] 사용: 각 token은 문자열 => 끝에 null문자 보유
				if (popped->data[0] == LEFT_PARENTHESIS)
				{
					// pop된 토큰이 왼쪽 괄호인 경우 스택에서 제거
					LLS_destroyNode(popped);
					break;
				}
				else
				{
					// pop된 토큰이 왼쪽 괄호 외 사칙연산자면 출력 후 제거
					strcat(postfixExp, popped->data);
					LLS_destroyNode(popped);
				}
			}
		}
		else {	
			// 토큰이 오른쪽 괄호 이외의 연산자인 경우
			// 연산자들 간 우선순위 검사
			while (!LLS_isEmpty(stack) && !isPrior(LLS_top(stack)->data[0], token[0]))
			{
				Node * popped = LLS_pop(stack);

				if (popped->data[0] != LEFT_PARENTHESIS)
					strcat(postfixExp, popped->data);

				LLS_destroyNode(popped);
			}

			LLS_push(stack, LLS_createNode(token));
		}
	}

	/* 중위표기식을 다 읽으면 스택에 저장된 모든 연산자를 후위표기식에 출력 */
	// 괄호가 한 짝만 있는 경우 그냥 제거, 없는 것처럼 취급
	while (!LLS_isEmpty(stack))
	{
		Node * popped = LLS_pop(stack);

		if (popped->data[0] != LEFT_PARENTHESIS)
			strcat(postfixExp, popped->data);

		LLS_destroyNode(popped);
	}
	LLS_destroyStack(stack);
}

/* 후위표기식 계산 알고리즘 */
// 괄호는 모두 제거된 상태
double calculate(char * postfixExp)
{
	LinkedListStack * stack;
	Node * resultNode;

	double result;
	char token[32];
	int type = -1;
	unsigned int read = 0;
	unsigned int length = strlen(postfixExp);

	LLS_createStack(&stack);

	while (read < length)
	{
		// 토큰 단위로 후위표기식 읽어들이기
		read += getNextToken(&postfixExp[read], token, &type);

		// 공백 무시
		if (type == SPACE)
			continue;

		// 현재 토큰이 피연산자인 경우 
		if (type == OPERAND)
		{
			// 스택에 push
			Node * newNode = LLS_createNode(token);
			LLS_push(stack, newNode);
		}
		else	// 현재 토큰이 사칙연산자인 경우
		{
			char resultString[32];
			double operator1, operator2, tempResult;
			Node * operatorNode;

			// 스택에 push된 최상위 피연산자 2개를 pop
			operatorNode = LLS_pop(stack);
			operator2 = atof(operatorNode->data);
			LLS_destroyNode(operatorNode);

			operatorNode = LLS_pop(stack);
			operator1 = atof(operatorNode->data);
			LLS_destroyNode(operatorNode);

			// 사칙연산 수행
			switch(type)
			{
			case PLUS: tempResult = operator1 + operator2; break;
			case MINUS: tempResult = operator1 - operator2; break;
			case MULTIPLY: tempResult = operator1 * operator2; break;
			case DIVIDE: tempResult = operator1 / operator2; break;
			}

			// 연산 결과를 문자열로 재변환 후 스택에 push
			gcvt(tempResult, 10, resultString);
			LLS_push(stack, LLS_createNode(resultString));
		}
	}

	// 최종 결과값을 스택에서 pop하여 실수로 변환
	resultNode = LLS_pop(stack);
	result = atof(resultNode->data);
	LLS_destroyNode(resultNode);

	LLS_destroyStack(stack);
	return result;	// 최종 결과값 리턴
}
