/*
 * ultrasonic.c
 *
 * Created: 2024-05-08 오전 10:55:57
 *  Author: kccistc
 */ 

#include "ultrasonic.h"
#include "extern.h"

volatile int ultrasonic_distance=0;
char scm[50];
extern void I2C_LCD_write_string_XY(uint8_t row, uint8_t col, char *string);
extern void I2C_LCD_clear(void);

// PE4 : 외부 INT4 초음파센서 상승.하강에지 둘다 이곳으로 들어 온다
// 결국은 상승에지에서 1번 / 하강에지에서 1번 2번 들어 온다.
ISR(INT4_vect)
{
	//1. 상승 에지 일때
	if (ECHO_PIN & (1 << ECHO))
	{
		TCNT1 = 0;   
	} 
	else  //2. 하강 에지 일때 
	{
		// 에코핀에 들어 오는 펄스 길이를 us단위로 환산 
	     ultrasonic_distance = 1000000.0 * TCNT1 * 1024 / F_CPU;
		// 예) TCNT1에 10이 들어 있다고 가정 하자
		// 15.625KHZ의 1주기가 64us 이다.
		// 0.000064 * 10 : 0.00064sec(640us)
		// 640 / 58 ==> 11cm
		// 1cm : 58us소요 
		sprintf(scm, "dis: %dcm", ultrasonic_distance/58);  // cm로 환산 
	}
}

void init_ultrasonic()  // InitUltrasonic
{
	TRIG_DDR |= 1 << TRIG;   // output mode로 설정 
	ECHO_DDR  &= ~(1 << ECHO);   // input mode로 설정
	
	// P289 표12-6
	// 0 1 : 상승 하강에지 둘다 INT를 띄우도록 설정 
	EICRB |= 0 << ISC41 | 1 << ISC40;
	// 16bit timer 1번을 설정을 해서 사용 한다. 
	// 16: 65535
	// 16MHZ : clock 
	// 16MHZ를 1024분주 16000000/1024 ==> 15625HZ ==> 15.625KHZ
	// 1주기(1개의 펄스길이) T=1/f 1/15625 => 0.000064sec ==> 64us
	TCCR1B |= 1 << CS12 | 1 << CS10;   // 1024로 분주 설정 
	
	EIMSK |= 1 << INT4;   // 외부 INT4(ECHO)사용 
}

void ultrasonic_trigger()
{
	TRIG_PORT &= ~(1 << TRIG);   // low
	_delay_us(1);
	TRIG_PORT |= 1 << TRIG;    // high
	_delay_us(15);
	TRIG_PORT &= ~(1 << TRIG);   // low
}

void ultrasonic_distance_check()
{
	if (ultrasonic_trigger_timer >= 1000)   // 1초
	{
		ultrasonic_trigger_timer=0;
		printf("%s", scm);
		I2C_LCD_clear();
		I2C_LCD_write_string_XY(0,0, "Hello !!!");
		I2C_LCD_write_string_XY(1,0, scm);
		ultrasonic_trigger();
	}
}