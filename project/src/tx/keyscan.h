#ifndef KEYSCAN_H
#define KEYSCAN_H

#include <stm32f10x.h>
#include "control.h"

void set_keyscan(void);
void TIM3_IRQHandler (void);

int mode;

void init_keyscan(void) {
	RCC->APB2ENR |= 0x00000018;
	RCC->APB1ENR |= 0x00000002;
	
	GPIOC->CRH = 0x00003333; // PC8-11: output 
	GPIOB->CRH = 0x00008888; // PB8-11: input
	GPIOB->BSRR = 0x0F00;
	
	TIM3->CR1 = 0x00;
	TIM3->CR2 = 0x00;
	TIM3->PSC = 0x07FF;
	TIM3->ARR = 0x0500;
	
	TIM3->DIER = 0x0001;
	NVIC->ISER[0] |= 0x20000000;
	
	keyscan.key_index = 0;
	keyscan.key_row = 0x01;
	keyscan.p_cnt = 0;
	
	TIM3->CR1 |= 0x0001;
}

void TIM3_IRQHandler (void) {
	if (( TIM3->SR & 0x0001) != 0 ) {
		GPIOC->BSRR = (~(keyscan.key_row << 8) & 0x0F00) | (keyscan.key_row << 24);
		
		keyscan.data = 0xF; // init data
		
		keyscan.key_col = GPIOB->IDR;
		keyscan.key_col = (keyscan.key_col >> 8) & 0x000F;	
		
		keyscan.col_scan = 0x01;
		for (int j = 0; j < 4; j++) {
			if ((keyscan.key_col & keyscan.col_scan) == 0)
				keyscan.data = keyscan.key_index;
			keyscan.col_scan = keyscan.col_scan << 1;
			keyscan.key_index += 1;
		}
		
		keyscan.key_row = keyscan.key_row << 1;
		if (keyscan.key_row == 0x10) {
			keyscan.key_row = 0x01;
			keyscan.key_index = 0;
		}
		if(keyscan.data == 0xB) {
            keyscan.p_cnt++;
            keyscan.key_row = 0x04;
            keyscan.key_index = 8;
            keyscan.data = 0xF;
         }
         else if(keyscan.p_cnt > 0 && keyscan.data == 0xF) {
            if(keyscan.p_cnt > 20) {
               keyscan.data = 0xC;
            }
            else {keyscan.data = 0xD;}
            keyscan.p_cnt = 0;
         }
         
      if(keyscan.data != 0xF){
            USART1->CR1 |= 0x000000080;
      }
		}
	TIM3->SR &= ~(1<<0);	// clear UIF
	}


#endif
