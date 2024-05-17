/*
 * 04.timer_test.c
 *
 * Created: 2024-04-26 오후 3:23:30
 * Author : kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>   // printf scanf fgets 등이 정의 되어 있다. 

#include "def.h"
#include "extern.h"
#include "stack.h"
#include "cal.h"
#include "button.h"

void init_timer0();
void init_uart0(void);

// 1. for printf
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);


volatile uint32_t ms_count=0;   // ms count
                                // interrupt service routine 안에 쓰는 변수는
							    // 변수 type앞에 volatile이라고 기입 한다.
								// 이유는 최적화 방지를 위함이다. 
volatile uint32_t shift_timer=0; 
volatile uint32_t ultrasonic_trigger_timer=0;   // trigger를 동작시키는 주기 1000ms
// 256개의 pulse를 count(1ms)하면 이곳으로 자동적으로 진입 한다.
// 즉 256개의 pulse ==> 1ms
// ISR 루틴은 가능한 짧게 구현 한다.

uint8_t keydata=0; 
int key_flag = 0;

ISR(TIMER0_OVF_vect)
{
	TCNT0=6;  // 6~256 --> 1ms를 맞춰주기 위해서 TCNT0를 6으로 설정
	ms_count++;
	if (ms_count >= 60)
	{
		ms_count=0;
		if ( (keydata=scan_keypad()) )
		{
			insert_queue(keydata);
			key_flag = 1;
		}
		
	}
}



int main(void)
{	
	uint8_t readkey=0;

	init_timer0();
	init_uart0();
	init_keypad();
	queue_init();
	
	char postfix[50] = { 0 };
	
	stdout = &OUTPUT;   // 2. printf가 동작 되도록 stdout에 OUTPUT화일 포인터를 assign
	                    // fprintf(stdout,"test\n");  printf("test\n"); scanf("%s"buff);  ==> stdin
	                    // fgets(input,100,stdin); // stdin : keyboard
						
	I2C_LCD_init();
	sei();    // 전역적으로 인터럽트 허용

	char string[100] = "";
	int string_cnt = 0;
	
    while (1) 
    {
		if(get_button(BUTTON1PIN,BUTTON1)){
			key_flag = 1;
			insert_queue('(');
		}else if(get_button(BUTTON2PIN, BUTTON2))
		{
			key_flag = 1;
			insert_queue(')');
		}else if(get_button(BUTTON3PIN,BUTTON3)){
			I2C_LCD_clear();
			queue_init();
			int len = strlen(string);
			string_cnt = 0;
			for(int i=0; i < len; i++){
				string[i] = '\0';
			}
			for(int i=0; i< QUEUE_MAX; i++){
				queue[i] = 0;
			}
			printf("clear\n");
		}
		//if (queue_empty() != TRUE)
		if (key_flag == 1)
		{
			key_flag = 0;
			readkey=read_queue();
printf("readkey:%c\n", readkey);

			//LCD 출력 함수 호출
			string[string_cnt] = readkey;
			string_cnt++;
printf("string: %s\n", string);
printf("string_cnt: %d\n", string_cnt);

			//backspace 기능
			if(readkey == 'b'){
				printf("backspace\n");
				backspace_queue();//rear감소
				string[--string_cnt] = '\0';
				string[--string_cnt] = '\0';
				
				I2C_LCD_clear();
			}

			if(strlen(string) < 16){
				I2C_LCD_write_string_XY(0, 0, string);
			}else{
				I2C_LCD_write_string_XY(0, 0, string);
				I2C_LCD_write_string_XY(1, 0, &string[16]);
			}

			
			if(readkey == '='){
				//queue(uint8_t) -> postfix(char)
				
				infix_to_postfix((char*) queue, postfix);
printf("result: %.4lf\n", eval(postfix));

				//LCD 출력
				//string[string_cnt] = (char)eval(postfix);
				//double->string
				char s1[10] = "";
				sprintf(s1, "%.4lf", eval(postfix));
				strcat(string, s1);
printf("result string: %s\n", string);
				
				if(strlen(string) < 16){
					I2C_LCD_write_string_XY(0, 0, string);
				}else{
					I2C_LCD_write_string_XY(0, 0, string);
					I2C_LCD_write_string_XY(1, 0, &string[16]);
				}			
				break;
			}
		}
	}
	
	
	
}

// timer0를 초기화 한다. 8bit timer 0번과 2번중 0번을 사용한다. 
void init_timer0()
{
// 분주비를 64로 설정 
// 1.분주비 계산
// 16000000HZ/64 ==> 250,000 HZ
// 2. T(주기) pulse 1개가 잡아 먹는 시간: 1/f = 1/250000 => 0.000004sec (4us) : 0.004ms
// 3. 8bit timer OV(OV flow) : 0.004ms x 256 == 0.001024sec ==> 1.024ms
//                             0.004ms x 250 == 0.001sec ==> 1ms
  TCNT0 = 6;   // TCNT : 0~256 ==> 정확히 1ms마다 TIMER0_OVF_vect로 진입한다.
  TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00);  // (1) 분주비: 64로 설정
  TIMSK |= 1 << TOIE0;    // (2) timer0 overflow INT허용
}
