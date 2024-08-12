#include <stm32f10x.h>
#include "update_data.h"
#include "display.h"
#include "scroll.h"
#include "control.h"
#include "usart_rx.h"
#include "flash.h"

int main(void) {
	RCC->APB2ENR = 0x0;
	
	init_data();
	
	init_data_update();
	init_display();
	init_scroll();
	init_passcode();
	init_usart();
	
	passcode.length = load_passcode(passcode.item);
	start_mode(0);
	
	while(1) {__WFI();}
}
