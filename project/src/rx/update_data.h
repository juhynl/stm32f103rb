#ifndef UPDATE_DATA_H
#define UPDATE_DATA_H

#include <stm32f10x.h>
#include "control.h"

void init_data_update(void);
void init_data(void);
void TIM4_IRQHandler (void);


void init_data_update(void) {
	RCC->APB1ENR |= 0x00000004;
	
	RCC->APB2ENR |= 0x00000005;
	GPIOA->CRL &= 0x44404444;
	
	RCC->APB2ENR |= 0x00000200;
	ADC1->CR1 = 0x00000000;
	ADC1->CR2 = 0x001E0002;
	ADC1->SMPR2 = 0x00007000;
	ADC1->SQR1 = 0x00000000;
	ADC1->SQR2 = 0x00000000;
	ADC1->SQR3 = 0x00000004;
	
	ADC1->CR2 |= 0x00000001;
	ADC1->CR2 |= (1 << 22);
	
	TIM4->CR1 = 0x00;
	TIM4->CR2 = 0x00;
	TIM4->PSC = 7200 - 1;
	TIM4->ARR = 10000 - 1;
	
	TIM4->DIER = 0x0001;
	NVIC->ISER[0] = (1 << 30);
	TIM4->CR1 |= 0x0001;
}

void init_data(void) {
	data.hour = 12;
	data.min = 0;
	data.sec = 0;
	data.temperature = 0;
	data.data[0] = 1;			// h
	data.data[1] = 2;			// h
	data.data[2] = 0xC; 	// :
	data.data[3] = 0;			// m
	data.data[4] = 0;			// m
	data.data[5] = 0xC; 	// :
	data.data[6] = 0;			// s
	data.data[7] = 0;			// s
	data.data[8] = 0x0E; 	// (space)
	data.data[9] = 0;			// n	
	data.data[10] = 0;		// n
	data.data[11] = 0x0D; // .
	data.data[12] = 0;		// n
	data.timeset_flag = 0;
	data.temperature_flag = 0;
}

void TIM4_IRQHandler (void) {
	if (( TIM4->SR & 0x0001) != 0 ) {
		// update time
		data.sec++;
		if(data.sec == 60) {
			data.sec = 0;
			data.min++;
			if(data.min == 60) {
				data.min = 0;
				data.hour++;
				if(data.hour == 24) {
					data.hour = 0;
				}
			}
		}
		data.data[0] = data.hour / 10;
		data.data[1] = data.hour % 10;
		data.data[3] = data.min / 10;
		data.data[4] = data.min % 10;
		data.data[6] = data.sec / 10;
		data.data[7] = data.sec % 10;
	}
	
	// sampling temperature
	data.temperature_flag ^= 1;
	if(data.temperature_flag) {
		if(ADC1->SR & 0x02) {
			data.temperature = (float)ADC1->DR;
			data.temperature = (float)(((data.temperature * 5) / 4096) - 0.75) *100;
			data.data[9] = (char)(data.temperature / 10);
			data.data[10] = (char)(data.temperature) % 10;
			data.data[12] = (char)(data.temperature * 10) % 10;
		}
	}
	TIM4->SR &= ~(0x01); // clear UIF
}

#endif
