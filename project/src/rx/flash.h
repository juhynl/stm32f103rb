#ifndef FLASH_H
#define FLASH_H

#include <stm32f10x.h>

void flash_unlock(void);
void flash_erase(u32 page_addr);
void flash_write(u32 addr, u16 data);
u16 flash_read(uint32_t addr);
void save_passcode(int length, int * list);
int load_passcode(int * list);

u32 flash_page_addr = 0x0801F800;

void flash_unlock(void) {
    if (FLASH->CR & 0x00000080) {  
        FLASH->KEYR = 0x45670123; 
        FLASH->KEYR = 0xCDEF89AB;
    }
}

void flash_erase(u32 page_addr) {
    while (FLASH->SR & 0x00000001) {}
    FLASH->CR |= 0x00000002;
    FLASH->AR = page_addr; 
    FLASH->CR |= 0x00000040; 

    while (FLASH->SR & 0x00000001) {}
    FLASH->CR &= ~0x00000002;  
}

void flash_write(u32 addr, u16 data) {
    while (FLASH->SR & 0x00000001) {}
    FLASH->CR |= 0x00000001;  

    *(u16*)addr = data;  
    while (FLASH->SR & 0x00000001) {}
    FLASH->CR &= ~0x00000001; 
}

u16 flash_read(uint32_t addr) {
    return *(u16*)addr;
}

void save_passcode(int length, int * list) {
	flash_unlock();
	flash_erase(flash_page_addr);
	
	flash_write(flash_page_addr, (u16)length);
	for(u32 i=1; i<=length; i++) {
		flash_write(flash_page_addr + i*2, (u16)list[i]);
	}
}

int load_passcode(int * list) {
	int length = (int)flash_read(flash_page_addr);
	for(u32 i=1; i<=length; i++) {
		list[i] = flash_read(flash_page_addr + i*2);
	}
	return length;
}

#endif
