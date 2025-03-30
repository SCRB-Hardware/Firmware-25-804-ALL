#include <stm32g030xx.h>
#include "lib-limit-switch.h"


/* ------ Limit switches ------ */

void LMSW_Init(void) {
	// Input mode for PD1 and PD2
	GPIOD->MODER = GPIOD->MODER & 0xFFFFFFC3; 
}

int LMSW_Read(int index) {
	// Returns whether the switch is closed (active)
	// *********Need to change the argument in dss_home()***************
	if(index != 1 && index != 2) return 0; 
	return !(GPIOD->IDR & (1 << (index))); 
}

int LMSW_ReadAll(void) {
	// Returns all switches as a bitmap
	return ~GPIOD->IDR & 0x6; 
}
