#ifndef SCROLL_H
#define SCROLL_H

#include <stm32f10x.h>
#include "control.h"

void init_scroll(void);
void scroll_data(void);
void scroll_buffer_data(void);
void TIM2_IRQHandler(void);

void init_scroll(void) {
	RCC->APB1ENR |= 0x00000001;
	
	TIM2->CR1 = 0x00;
	TIM2->CR2 = 0x00;
	
	TIM2->DIER = 0x0001;
	NVIC->ISER[0] = (1 << 28);
}

void scroll_data(void) {
	for(int i=0; i<8; i++) {
			display.col[i] = 0x00;
		if(mode == 1 || scroll.data_shift % 2 == 0) {
			display.col[i] |= font8x8[data.data[scroll.data_shift/8 - 1]][i] << (8 + (scroll.data_shift % 8));
			display.col[i] |= font8x8[data.data[scroll.data_shift/8]][i] << (scroll.data_shift % 8);
			display.col[i] |= font8x8[data.data[scroll.data_shift/8 + 1]][i] >> (8 - (scroll.data_shift % 8));
		}
	}
	if(scroll.data_shift == 96) {
		end_mode1_5();
	}
	scroll.data_shift++;
}

void scroll_buffer_data(void) {
	scroll.cnt++;
	if(scroll.cnt >= 301) {
		end_mode1_5();
		set_sign(15); // Display T
	}
	if(scroll.data_shift < buffer.idx * 8) {
		for(int i=0; i<8; i++) {
			display.col[i] = 0x00;
			display.col[i] |= font8x8[buffer.item[buffer.idx - 1]][i] << (scroll.data_shift % 8);
			display.col[i] |= font8x8[buffer.item[buffer.idx]][i] << (8 - (scroll.data_shift % 8));
		}
		scroll.data_shift++;
	}
}

void TIM2_IRQHandler(void) {
	if ((TIM2->SR & 0x0001) != 0 ) {
		if(mode == 0) {
			display.col[0] ^= 1;
			if(scroll.sign_flag == 1) {
				remove_sign();
			}
		}
		else if(mode == 1 || mode == 2) {scroll_data();}
		else {scroll_buffer_data();}
	}
	TIM2->SR &= ~(1<<0);	// clear UIF
}


#endif
