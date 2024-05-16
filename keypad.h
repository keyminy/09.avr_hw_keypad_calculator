/*
 * keypad.h
 *
 * Created: 2024-05-16 오전 11:01:02
 *  Author: kccistc
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#define F_CPU 16000000UL
#include <avr/io.h>   // PORTA DDRA 등의 symbol이 정의 되어 있다.'

#define KEYPAD_DDR DDRA
#define KEYPAD_PIN PINA
#define KEYPAD_PORT PORTA


#endif /* KEYPAD_H_ */