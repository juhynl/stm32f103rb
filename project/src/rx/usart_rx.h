#ifndef USART_H
#define USART_H

#include <stm32f10x.h>
#include "control.h"
#include "flash.h"

void init_usart(void);
void USART2_IRQHandler(void);

void init_usart(void) {
	RCC->APB2ENR |= 0x00004004;  
	GPIOA->CRH &= ~(0xFFu << 4); 
	GPIOA->CRH |= (0x04B << 4); 	
	
	USART1->BRR = 0x0EA6;		
	USART1->CR1 = 0x00000000;		
	USART1->CR2 = 0x00000000;
	USART1->CR3 = 0x00000000;		
	USART1->CR1 |= 0x00000004;	
	USART1->CR1 |= 0x00002000;
	
	NVIC->ISER[1] |= (1 << 5);
	USART1->CR1 |= 0x00000020;
}

void USART1_IRQHandler(void) {
	if (USART1->SR & 0x20) {
		keyscan.data = USART1->DR;
		
		if(mode == 0) {
			if(keyscan.data < 0xA) {
				if(data.timeset_flag == 1)
					start_mode(1);
				else
					start_mode(2);
			}
			else if(passcode.wrong_cnt < 3) {
				if(keyscan.data == 0xC) {
					start_mode(5);
				}
				else if(keyscan.data == 0xD) {
					start_mode(4);
				}
			}
		}
		else if((mode == 1 || mode == 2) && keyscan.data == 0xA) {
			end_mode1_5();
			start_mode(3);
		}
		else if(mode == 3) {
			if(keyscan.data < 0xA && buffer.idx < 6) {
				scroll.cnt = 0;
				buffer.idx++;
				buffer.item[buffer.idx] = (int)keyscan.data;
			}
			else if(buffer.idx == 6 && keyscan.data == 0xA) {
				end_mode1_5();
				set_time(buffer.item[1]*10 + buffer.item[2], buffer.item[3]*10 + buffer.item[4], buffer.item[5]*10 + buffer.item[6]);
			}
		}
		else if(mode == 4) {
			if(keyscan.data < 0xA && buffer.idx < 8) {
				scroll.cnt = 0;
				buffer.idx++;
				buffer.item[buffer.idx] = (int)keyscan.data;
			}
			else if(keyscan.data == 0xC || keyscan.data == 0xD) {
				end_mode1_5();
				int is_right = 1;
				if(buffer.idx != passcode.length)
						is_right = 0;
				else {
					for(int i=1; i<=buffer.idx; i++) {
						if(passcode.item[i] != buffer.item[i]) {
							is_right = 0;
							break;
						}
					}
				}
				
				if(is_right) {
					passcode.wrong_cnt = 0;
					set_sign(16);
				}
				else{
					passcode.wrong_cnt++;
					if(passcode.wrong_cnt >= 3)
						set_sign(18);
					else
						set_sign(17);
				}
			}
		}
		else if(mode == 5) {
			if(keyscan.data < 0xA && buffer.idx < 8) {
				scroll.cnt = 0;
				buffer.idx++;
				buffer.item[buffer.idx] = (int)keyscan.data;
			}
			else if((keyscan.data == 0xC || keyscan.data == 0xD) && buffer.idx >= 4) {
				end_mode1_5();
				for(int i=1; i<=buffer.idx; i++) {
					passcode.item[i] = buffer.item[i];
				}
				passcode.length = buffer.idx;
				save_passcode(passcode.length, passcode.item);
			}
		}		
	}
}

#endif
