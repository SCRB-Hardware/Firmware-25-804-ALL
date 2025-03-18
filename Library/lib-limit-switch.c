#include <stm32g030xx.h>
#include "lib-limit-switch.h"


/* ------ Limit switches ------ */

void LMSW_Init(void) {
	// PD0~3: Limit switches 1~4
	GPIOD->MODER = GPIOD->MODER & 0xFFFFFF00; 
}

int LMSW_Read(int index) {
	// Returns whether the switch is closed (active)
	if(index < 1 || index > 4) return 0; 
	return !(GPIOD->IDR & (1 << (index - 1))); 
}

int LMSW_ReadAll(void) {
	// Returns all switches as a bitmap
	return ~GPIOD->IDR & 0xF; 
}
