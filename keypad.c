/*
 * keypad.c
 *
 * Created: 2024-05-16 오전 11:13:04
 *  Author: kccistc
 */ 
#include "keypad.h"

void init_keypad(void);
uint8_t get_keypad(int row, int col);
uint8_t scan_keypad();

void init_keypad(void)
{
	KEYPAD_DDR = 0xf0;   //  col : 출력 row: 입력
	// KEYPAD_DDR = 0x0f;   // row: 입력 col : 출력 
	KEYPAD_PORT = 0xff;
}

// 주기적으로 호출 한다. 약 60ms마다 
uint8_t scan_keypad()
{
	uint8_t data=0;   // key값을 저장할 변수 
	
	for (int row=3; row >= 0; row--)
	{
		for (int col=0; col < 4; col++)
		{
			data = get_keypad(row, col);
			if (data)   // if (data >= 1) 
			{
				
			    return data;
			}
		}
	}
	return 0;
}

uint8_t get_keypad(int row, int col)
{
	uint8_t keypad_char[4][4] = {
		{' ', '0', '=', '+'},
		{'7', '8', '9', '*'},
		{'4', '5', '6', '-'},
		{'1', '2', '3', '/'}
	};
	
	static int8_t prev_keystate[4][4] =    // 초기 keypad상태 flag
	{   
		{1, 1, 1, 1},  // 1: idle(키가 눌려지지 않은 상태
		{1, 1, 1, 1},
		{1, 1, 1, 1},
		{1, 1, 1, 1}
	};	
	
	uint8_t current_state=1;   // 버튼 값 
	
	 KEYPAD_PORT=0xff;
	 KEYPAD_PORT &= ~(1 << col+4);   // 해당 col에 전류를 흘린다. LOW로 만든다. 
	 // 반드시 아래의 코드를 집어 넣는다 
	 for(int delay=0; delay < 20; delay++)  // keypad check delay 
		;   // NOP noperation
	 // 7654 3  2  1  0
	 //     R0 R1 R2 R3
	 // 예) row0 : PA3에 연결  row0가 눌려 지면 
	 // 1111 0  1  1  1
	 // 0001 1  1  1  0
	 // 0000 0  0  0    ===>  current_state
	 // current_state = ((KEYPAD_PIN & (1 << (row-3))) >> (row-3)) & 0b00000001;
	 current_state = (KEYPAD_PIN >> row) & 0b00000001;
	 // current_state == 0 이면 버튼이 눌려진 상태 (pull-up이기 떄문에 active시 LOW)
	 // (1) 처음 버튼이 눌려진 상태 인가 ?
	 if (current_state == 0 && prev_keystate[col][row] == 1)  // 처음 버튼이 눌려진 상태
	 {
		 prev_keystate[col][row]=0;
		 return 0;   // 버튼이 눌려진 상태 이나 완전한 상태로 보지 않고 
		             // prev_keystate[col][row]만 눌려진 상태로 set
	 }
	       // 이전에 버튼이 눌려졌고 현재는 버튼을 뗀 상태이면 버튼을 눌렀다 뗀 상태로 인정 
	 else if (current_state == 1 && prev_keystate[col][row] == 0)
	 {
		 prev_keystate[col][row]=1;  // 다음 버튼 체크를 위해서 초기화 
printf("key:%c\n", keypad_char[col][row]);
		return (keypad_char[col][row]); 
	 }
	 return 0;
}