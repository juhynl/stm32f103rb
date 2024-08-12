#ifndef CONTROL_H
#define CONTROL_H

#include <stm32f10x.h>

#define PASSCODE_BASE_ADDR 0x0807F800 

void set_time(char hour, char min, char sec);

int mode = 0;
int * base_addr;

const int pw1 = 0;
int * pw = &pw1;

u8 font8x8[20] [8] = {
	{0x3c, 0x42, 0x46, 0x4a, 0x52, 0x62, 0x3c, 0x00}, // 0
	{0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x7c, 0x00}, // 1
	{0x3c, 0x42, 0x02, 0x0c, 0x30, 0x42, 0x7e, 0x00}, // 2
	{0x3c, 0x42, 0x02, 0x1c, 0x02, 0x42, 0x3c, 0x00}, // 3
	{0x08, 0x18, 0x28, 0x48, 0xfe, 0x08, 0x1c, 0x00}, // 4
	{0x7e, 0x40, 0x7c, 0x02, 0x02, 0x42, 0x3c, 0x00}, // 5
	{0x1c, 0x20, 0x40, 0x7c, 0x42, 0x42, 0x3c, 0x00}, // 6
	{0x7e, 0x42, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00}, // 7
	{0x3c, 0x42, 0x42, 0x3c, 0x42, 0x42, 0x3c, 0x00}, // 8
	{0x3c, 0x42, 0x42, 0x3e, 0x02, 0x04, 0x38, 0x00}, // 9
	{0x18, 0x24, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x00}, // *
	{0x7c, 0x22, 0x22, 0x3c, 0x22, 0x22, 0x7c, 0x00}, // #
	{0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, // :
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, // .
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // space
	{0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},  // T
	{0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0x66, 0x3C, 0x00},  // O
	{0xC3, 0xC3, 0x66, 0x3C, 0x66, 0xC3, 0xC3, 0x00},  // X
	{0xFC, 0x66, 0x63, 0x63, 0x63, 0x66, 0xFC, 0x00},  // D
	{0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xE0, 0x00},  // P
};

/*
DATA UPDATE
*/
typedef struct {
	char hour, min, sec;
	char data[13]; // hh:mm:ss nn.n
	float temperature;
	int timeset_flag;
	int temperature_flag;
} DATA;

DATA data;

void set_time(char hour, char min, char sec) {
		data.hour = hour;
		data.min = min;
		data.sec = sec;
		data.data[0] = hour / 10;
		data.data[1] = hour % 10;
		data.data[3] = min / 10;
		data.data[4] = min % 10;
		data.data[6] = sec / 10;
		data.data[7] = sec % 10;
		data.timeset_flag = 1;
	}

/*
DISPLAY
*/
typedef struct {
	unsigned int row;
	unsigned int col[8];
	int idx;
}DISPLAY;

/*
KEYSCAN
*/
typedef struct {
	unsigned int i, key_index, key_row, key_col, col_scan, data, p_cnt;
}KEYSCAN;

/*
SCROLL
*/
typedef struct {
	int data_shift;
	int cnt;
	int sign_flag;
}SCROLL;

/*
BUFFER
*/
typedef struct {
	int item[9];
	int idx;
}BUFFER;

/*
PASSCODE
*/
typedef struct {
	int item[9];
	int length;
	int wrong_cnt;
}PASSCODE;

DISPLAY display;
KEYSCAN keyscan;
SCROLL scroll;
BUFFER buffer;
PASSCODE passcode;

void init_passcode(void) {
	passcode.length = 0;
	passcode.wrong_cnt = 0;
}

void get_mem_addr(int *mem) {
	base_addr = mem;
}

void save_passcode_to_flashmem(void) {	
	base_addr[0] = passcode.length;
	for(int i=1; i<=passcode.length; i++) {
		base_addr[i] = passcode.item[i];
	}
}

void load_passcode_from_flashmem(void) {
	int *mem = (int *)PASSCODE_BASE_ADDR;
	passcode.length = mem[0];
	for(int i=1; i<=passcode.length; i++) {
		passcode.item[i] = mem[i];
	}
}

void disable_TIM1_TIM2(void) {
	TIM1->CR1 &= 0xFFFE;
	TIM1->CNT = 0x00;
	TIM2->CR1 &= 0xFFFE;
	TIM2->CNT = 0x00;
}

void enable_TIM1_TIM2(void) {
	TIM1->CR1 |= 0x0001;
	TIM2->CR1 |= 0x0001;
}

void reset_display(void) {
	for(int i=0; i<8; i++) {
		display.col[i] = 0;
	}
	display.idx = 0;
	display.row = 1;
}

void reset_buffer(void) {
	for(int i=0; i<8; i++) {
		buffer.item[i] = 0xE;
	}
	buffer.idx = 0;
}

void start_mode(int n) {
	mode = n;
	disable_TIM1_TIM2();
	reset_display();
	
	if(mode == 0) {
		TIM2->PSC = 7200 - 1;
		TIM2->ARR = 10000 - 1;
	}
	else if(mode == 1 || mode == 2) {
		scroll.data_shift = 8;
		TIM2->PSC = 7200 - 1;
		TIM2->ARR = 1000 - 1;
	}
	else {
		scroll.data_shift = 0;
		scroll.cnt = 0;
		
		reset_buffer();
		
		if(mode == 5) {
			for(int i=0; i<8; i++)
				display.col[i] |= font8x8[19][i] << 8;
		}
		
		TIM2->PSC = 7200 - 1;
		TIM2->ARR = 100 - 1;	
	}
	
	enable_TIM1_TIM2();
}

void end_mode1_5(void) {
	disable_TIM1_TIM2();
	if(mode == 5) {
		for(int i=0; i<8; i++)
			display.col[i] &= 0x00FF;
	}
	start_mode(0);
}

void set_sign(int n) {
	for(int i=0; i<8; i++) {
		display.col[i] |= font8x8[n][i] << 8;
	}
	scroll.sign_flag = 1;
}

void remove_sign(void) {
	for(int i=0; i<8; i++) {
		display.col[i] &= 0x00FF;
	}
	scroll.sign_flag = 0;
}


#endif