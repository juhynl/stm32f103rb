#ifndef USART_TX
#define USART_TX

#include <stm32f10x.h>
#include "control.h"
//int test;

void init_USART_TX (void){

	RCC->APB2ENR |= 0x00004004;
	GPIOA->CRH &= ~(0xFFu << 4);
	GPIOA->CRH |= (0x04B << 4);
	
	USART1->BRR = 0xEA6;
	USART1->CR1 = 0x00000000;
	USART1->CR2 = 0x00000000;
	USART1->CR3 = 0x00000000;
	USART1->CR1 |= 0x00000008;
	USART1->CR1 |= 0x00002000;
	
	NVIC->ISER[1] |= (1 << 5);
	
	
}


void USART1_IRQHandler (void){
	
	if (USART1->SR & 0x80){	
		if (USART1->CR1 & 0x80){
		//test = keyscan.data;
		USART1->DR = keyscan.data;
		}
	}
	USART1->CR1 &= ~0x80;
		
	
}
#endif
