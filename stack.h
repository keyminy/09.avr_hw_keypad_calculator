/*
 * stack.h
 *
 * Created: 2024-05-16 오후 3:38:44
 *  Author: HARMAN-29
 */ 


#ifndef STACK_H_
#define STACK_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MAX_SIZE 100

typedef struct stack {
	double arr[MAX_SIZE];
	int top;
} stack;

void createStack(stack* p);
void push(stack* p, double elemenet);
double pop(stack* p);
double top(stack* p);
void displayStack(stack* p);
void clearStack(stack* p);
int isEmpty(stack* p);



#endif /* STACK_H_ */