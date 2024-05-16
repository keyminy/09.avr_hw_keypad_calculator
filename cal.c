/*
 * cal.c
 *
 * Created: 2024-05-16 오후 3:50:05
 *  Author: HARMAN-29
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#include "cal.h"
#include "stack.h"

double change_value(char* num, int* index) {
	// 문자형을 -> 숫자로(double)
	double value;
	char temp[100];
	int i,k;
	for (i = 0; num[i] != ' '; i++) {
		temp[i] = num[i];
	}
	temp[i] = '\0';
	value = atof(temp);
	k = *index;
	*index = k + i;
	return value;
}


words classification(char* ch) {
	switch (*ch) {
	case '(':
		return leftparen;
	case ')':
		return rightparen;
	case '+':
		return plus;
	case '-':
		return minus;
	case '*':
		return multiply;
	case '/':
		return divide;
	case '\0':
		return eos;
	default:
		return operand;
	}
}


// 후위 표기 수식 계산 함수
double eval(char* postfix) {
	// postfix : 222 4 +55 *100 7 /5 *-5 10 *-
	words temp;
	double op1, op2,value;
	
	stack stk;
	createStack(&stk);
	for (int i = 0; postfix[i] != '\0'; i++) {
		if (postfix[i] == ' ') {
			continue;
		}
		temp = classification(&postfix[i]);
		if (temp == operand) {
			value = change_value(&postfix[i],&i);
			push(&stk, value);
		}
		else {
			op2 = pop(&stk);
			op1 = pop(&stk);
			switch (temp) {
			case plus: push(&stk, op1 + op2); break;
			case minus: push(&stk, op1 - op2); break;
			case multiply: push(&stk, op1 * op2); break;
			case divide: push(&stk, op1 / op2); break;
			}
		}
	}

	return pop(&stk); // 마지막 최종 계산 결과를 return
}
// 큰 숫자일수록 우선순위 높음
int priority(char op) {
	switch (op) {
	case '(':
	case ')':
		return 0;
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	}
	return -1;
}

void infix_to_postfix(char* infix, char* postfix) {
	stack stk;
	createStack(&stk);
	// char* s = "10+(3-2)*4";
	
	char* src = infix;
	char* dest = postfix;

	while (*src != '\0') {
		if (isdigit(*src)) {
			// 숫자이면
			while (isdigit(*src)) {
				*dest++ = *src++;
			}
			// operator가 나오면 
			/* 공백의 역할은 아주 중요합니다(두자리 수 이상 연산)
			 postfix -> 3 5 189 * + */
			*dest++ = ' ';
		}
		// 이제부터, 확실한 operator
		else if (*src == '(') {
			push(&stk, *src++);
		}
		else if (*src == ')') {
			while (top(&stk) != '(') {
				*dest++ = pop(&stk);
			}
			// '(' 제거
			pop(&stk);
			src++;
		}
		else if (*src == '+' || *src == '-' || *src == '/' || *src == '*') {
			while (!isEmpty(&stk)) {
				if (priority(top(&stk)) >= priority(*src)) {
					//stack에 있는거 빼서 dest에 넣는 작업
					*dest++ = pop(&stk);
				}
				else {
					break;
				}
			}
			push(&stk, *src++);
		}
		else {
			src++;
		}
	}
	//stack에 남아 있는 값들을 dest에 추가
	while (!isEmpty(&stk)) {
		*dest++ = pop(&stk);
	}
	*dest = '\0';
}
