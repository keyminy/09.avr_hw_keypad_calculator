/*
 * cal.h
 *
 * Created: 2024-05-16 오후 3:50:30
 *  Author: HARMAN-29
 */ 


#ifndef CAL_H_
#define CAL_H_

typedef enum {
	leftparen
	,rightparen
	,plus
	,minus
	,multiply
	,divide
	,eos
	,operand
} words;

double change_value(char* num, int* index);
words classification(char* ch);
double eval(char* postfix);
int priority(char op);
void infix_to_postfix(char* infix, char* postfix);



#endif /* CAL_H_ */