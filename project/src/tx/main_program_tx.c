#include <stm32f10x.h>
#include "keyscan.h"
#include "control.h"
#include "usart_tx.h"

int main(void) {
	RCC->APB2ENR = 0x0;
	
	init_keyscan();
	init_USART_TX ();
	
	while(1) {__WFI();}
}
