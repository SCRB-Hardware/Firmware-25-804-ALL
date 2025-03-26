#include <stm32g030xx.h>

#include "lib-rcc.h"
#include "lib-board.h"

#include "lib-comms.h"

/* ------ Main application ------ */

extern void launch(void); 

int opa; 

int main(void) {
	Board_Init();
	Sys_ClockInit(); 
	
	Comms_Init(); 
	
	Board_SetPanicLed(1); 
	
	launch(); 
	
	while(1); 
}

