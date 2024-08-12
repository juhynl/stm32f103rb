#ifndef DISPLAY_H
#define DISPLAY_H

#include <stm32f10x.h> 	

#include "control.h"

void init_display(void);
void TIM1_UP_IRQHandler (void);
void reset_display(void);

#include <stm32f10x.h>

void init_display(void) {
	RCC->APB2ENR |= 0x0000281D;
	AFIO->MAPR |= 0x02000000;
	GPIOC->CRL = 0x33333333;
	GPIOB->CRL = 0x33333333;
	GPIOB->CRH = 0x33333333;
	
	TIM1->CR1 = 0x00;
	TIM1->CR2 = 0x00;
	
	TIM1->PSC = 0x007F;
	TIM1->ARR = 0x0200;
	
	TIM1->DIER = 0x0001;
	NVIC->ISER[0] = 0x02000000;
	
	TIM1->CR1 |= 0x0001;
}

void TIM1_UP_IRQHandler (void) {
	if ((TIM1->SR & 0x0001) != 0) {
		GPIOC->BSRR = (~(display.row) & 0x00FF) | (display.row << 16);
		display.row = display.row << 1;
		
		GPIOB->ODR = display.col[display.idx];
		
		display.idx++;
		if(display.row == 0x100) {display.row = 1; display.idx = 0;}

		TIM1->SR &= ~(1<<0);	// clear UIF
	}
}
#endif
