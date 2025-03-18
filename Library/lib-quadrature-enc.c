#include <stm32g030xx.h>
#include "lib-quadrature-enc.h"


#if defined(ENCODER_USE_1) || defined(ENCODER_USE_2)

#ifdef ENCODER_USE_1
uint16_t QEncoder_Count_1; 
#endif
#ifdef ENCODER_USE_2
uint16_t QEncoder_Count_2; 
#endif

void QEncoder_Init(void) {
#ifdef ENCODER_USE_1
	// PA8, PA9 (AF2)
	GPIOA->AFR[1] = GPIOA->AFR[1] & 0xFFFFFF00 | 0x00000022; 
	GPIOA->MODER = GPIOA->MODER & 0xFFF0FFFF | 0x000A0000; 
	RCC->APBENR2 |= RCC_APBENR2_TIM1EN; 
	TIM1->CCER = 0x20; // Invert B
	TIM1->SMCR = 0x3; 
	TIM1->SR = 0; 
	TIM1->DIER = 0x1; // Enable IRQ
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); 
	TIM1->CR1 = 0x5; 
#endif
#ifdef ENCODER_USE_2
	// PC6, PC7 (AF1)
	GPIOC->AFR[0] = GPIOC->AFR[0] & 0x00FFFFFF | 0x11000000; 
	GPIOC->MODER = GPIOC->MODER & 0xFFFF0FFF | 0x0000A000; 
	RCC->APBENR1 |= RCC_APBENR1_TIM3EN; 
	TIM3->CCER = 0x20; // Invert B
	TIM3->SMCR = 0x3; 
	TIM3->SR = 0; 
	TIM3->DIER = 0x1; // Enable IRQ
	NVIC_EnableIRQ(TIM3_IRQn); 
	TIM3->CR1 = 0x5; 
#endif
}

#ifdef ENCODER_USE_1

void QEncoder_Reset1(void) {
	TIM1->CR1 &= ~0x1; 
	TIM1->CNT = 0; 
	TIM1->SR = 0; 
	QEncoder_Count_1 = 0; 
	TIM1->CR1 |= 0x1; // Enable timer
}

int QEncoder_Read1(void) {
	for(;;) {
		int __IE = __get_PRIMASK(); 
		__disable_irq(); 
		uint32_t cnt = TIM1->CNT; 
		if(cnt & 0x80000000) {
			__set_PRIMASK(__IE); 
			continue; 
		}
		cnt = cnt & 0xFFFF | QEncoder_Count_1 << 16; 
		__set_PRIMASK(__IE); 
		return (int)cnt; 
	}
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
	TIM1->SR = 0x0; 
	if(TIM1->CR1 & TIM_CR1_DIR) {
		// Counting down
		QEncoder_Count_1--; 
	}
	else {
		// Counting up
		QEncoder_Count_1++; 
	}
}

#endif

#ifdef ENCODER_USE_2

void QEncoder_Reset2(void) {
	TIM3->CR1 &= ~0x1; 
	TIM3->CNT = 0; 
	TIM3->SR = 0; 
	QEncoder_Count_2 = 0; 
	TIM3->CR1 |= 0x1; // Enable timer
}

int QEncoder_Read2(void) {
	for(;;) {
		int __IE = __get_PRIMASK(); 
		__disable_irq(); 
		uint32_t cnt = TIM3->CNT; 
		if(cnt & 0x80000000) {
			__set_PRIMASK(__IE); 
			continue; 
		}
		cnt = cnt & 0xFFFF | (uint32_t)QEncoder_Count_2 << 16; 
		__set_PRIMASK(__IE); 
		return (int)cnt; 
	}
}

void TIM3_IRQHandler(void) {
	TIM3->SR = 0x0; 
	if(TIM3->CR1 & TIM_CR1_DIR) {
		// Counting down
		QEncoder_Count_2--; 
	}
	else {
		// Counting up
		QEncoder_Count_2++; 
	}
}

#endif

#endif
