#include <stm32g030xx.h>
#include "lib-rcc.h"


/* ------ Clock and board config ------ */

void Sys_ClockInit(void) {

	// disable PLL before configuration
	if (RCC->CR & RCC_CR_PLLON) {
		RCC->CR &= ~RCC_CR_PLLON;
		while (RCC->CR & RCC_CR_PLLRDY);
	}
	
	
	// Clock frequency: 64MHz
	// Clock source: HSE12
	// Update VOS to high performance
	RCC->APBENR1 |= RCC_APBENR1_PWREN; 
	__DSB(); 
	PWR->CR1 |= PWR_CR1_VOS; 
	while(PWR->SR2 & PWR_SR2_VOSF); 
	
	// Update flash wait state
	RCC->AHBENR |= RCC_AHBENR_FLASHEN; 
	__DSB(); 
	FLASH->ACR |= 0x00000602; // TODO: enable prefetch?
	
	// enable HSI16
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY));
	
	// Configure PLL: 16M / 2(M) * 16(N) / 2(R)
	RCC->PLLCFGR = (2 << RCC_PLLCFGR_PLLSRC_Pos) | (1 << RCC_PLLCFGR_PLLM_Pos) | (16 << RCC_PLLCFGR_PLLN_Pos) | (1 << RCC_PLLCFGR_PLLR_Pos) | RCC_PLLCFGR_PLLREN; 
	RCC->CR |= RCC_CR_PLLON; 
	while(!(RCC->CR & RCC_CR_PLLRDY)); 
	
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
