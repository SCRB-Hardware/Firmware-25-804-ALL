#include <stm32g030xx.h>
#include "lib-rcc.h"


/* ------ Clock and board config ------ */

void Sys_ClockInit(void) {
// Clock frequency: 64MHz
// Clock source: HSE12
	// Update VOS to high performance
	RCC->APBENR1 |= RCC_APBENR1_PWREN; 
	__DSB(); 
	PWR->CR1 = PWR->CR1 & 0xFFFFD8FF | 0x00000300; 
	while(PWR->SR2 & 0x00000400); 
	
	// Update flash wait state
	RCC->AHBENR |= RCC_AHBENR_FLASHEN; 
	__DSB(); 
	FLASH->ACR |= 0x00000602; // TODO: enable prefetch?
	
	// Enable HSE
	RCC->CR |= 0x00010000; 
	while(!(RCC->CR & 0x00020000)); 
	
	// Configure PLL: 12M / 6(M) * 64(N) / 2(R)
	RCC->PLLCFGR = 0x30004053; 
	RCC->CR |= 0x01000000; 
	while(!(RCC->CR & 0x02000000)); 
	
	// Switch clock to higher frequency
	RCC->CFGR = 0x00000002; 
	while((RCC->CFGR & 0x00000038) != 0x00000010);
}

void Sys_BlockingDelayU(uint32_t us) {
	SysTick->CTRL = 0x0; 
	SysTick->VAL = 0x0; 
	SysTick->LOAD = us << 3; // 8MHz reference clock
	SysTick->CTRL = 0x1; 
	while(!(SysTick->CTRL & 0x10000)); // Blocking wait
	SysTick->CTRL = 0x0; 
}

void Sys_BlockingDelayM(uint32_t ms) {
	for(uint32_t i = 0; i < ms; i++)
		Sys_BlockingDelayU(1000); 
}
