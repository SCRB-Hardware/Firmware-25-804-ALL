#include <stm32g030xx.h>
#include "lib-driver-gpio.h"


/* ------ Brushed motor driver IOs ------ */

#if defined(DRIVER_USE_DRV1) || defined(DRIVER_USE_DRV2)

void DriverGPIO_Init(void) {
	// PB0: DRVOFF, Output Default High
	// PB1: nFault, Input Active Low
	// PB2: nSLEEP, Output Default Low
	GPIOB->BSRR = 0x0001; 
	GPIOB->BRR = 0x0004; 
	GPIOB->MODER = GPIOB->MODER & 0xFFFFFFC0 | 0x00000011; 
#ifdef DRIVER_USE_DRV1
	// PA0: Driver 1 current feedback (ADC IN0)
	// PA4: Driver 1 direction (High for FWD)
	// PA6: Driver 1 PWM (TIM16_CH1, AF5)
	// PB12: Driver 1 NSS
	GPIOA->BSRR = 0x0010; 
	GPIOA->AFR[0] = GPIOA->AFR[0] & 0xF0FFFFFF | 0x05000000; 
	GPIOA->MODER = GPIOA->MODER & 0xFFFFCCFC | 0x00002103; 
#ifdef DRIVER_USE_SPI
	GPIOB->BSRR = 0x1000; 
	GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xFCFFFFFF | 0x02000000; // 30MHz SPI pins
	GPIOB->MODER = GPIOB->MODER & 0xFCFFFFFF | 0x01000000; 
#endif
	// Initialize Timer 16 @ 64MHz
	RCC->APBENR2 |= RCC_APBENR2_TIM16EN; 
	__DSB(); 
	TIM16->PSC = 3 - 1; 
	TIM16->ARR = 1024 - 1; // 20.8kHz, 10 bits
	TIM16->CCMR1 = 0x68; 
	TIM16->CCER = 0x01; 
	TIM16->CCR1 = 0x00; 
	TIM16->BDTR = 0x00; 
	TIM16->EGR = 0x1; 
	TIM16->CR1 = 0x1; 
#endif
#ifdef DRIVER_USE_DRV2
	// PA1: Driver 2 current feedback ADC IN1)
	// PA5: Driver 2 direction (High for FWD)
	// PA7: Driver 2 PWM (TIM17_CH1, AF5)
	// PA15: Driver 2 NSS
	GPIOA->BSRR = 0x0020; 
	GPIOA->AFR[0] = GPIOA->AFR[0] & 0x0FFFFFFF | 0x50000000; 
	GPIOA->MODER = GPIOA->MODER & 0xFFFF33F3 | 0x0000840C; 
#ifdef DRIVER_USE_SPI
	GPIOA->BSRR = 0x8000; 
	GPIOA->OSPEEDR = GPIOA->OSPEEDR & 0x3FFFFFFF | 0x80000000; // 30MHz SPI pins
	GPIOA->MODER = GPIOA->MODER & 0x3FFFFFFF | 0x40000000; 
#endif
	// Initialize Timer 17 @ 64MHz
	RCC->APBENR2 |= RCC_APBENR2_TIM17EN; 
	__DSB(); 
	TIM17->PSC = 3 - 1; 
	TIM17->ARR = 1024 - 1; // 20.8kHz, 10 bits
	TIM17->CCMR1 = 0x68; 
	TIM17->CCER = 0x01; 
	TIM17->EGR = 0x1; 
	TIM17->CR1 = 0x1; 
#endif
}

void DriverGPIO_SetEnable(int state) {
	if(state) GPIOB->BSRR = 0x0004; 
	else			GPIOB->BRR  = 0x0004; 
#ifdef DRIVER_USE_DRV1
	TIM16->BDTR = state ? 0x8000 : 0; 
#endif
#ifdef DRIVER_USE_DRV2
	TIM17->BDTR = state ? 0x8000 : 0; 
#endif
}

void DriverGPIO_SetDrvoff(int state) {
	if(state) GPIOB->BSRR = 0x0001; 
	else			GPIOB->BRR  = 0x0001; 
}


int DriverGPIO_HasFault(void) {
	return !(GPIOB->IDR & 0x0002); 
}

#ifdef DRIVER_USE_DRV1

void DriverGPIO_Set1Direction(int state) {
	if(state) GPIOA->BSRR = 0x0010; 
	else			GPIOA->BRR  = 0x0010; 
}

void DriverGPIO_Set1PWM(int pwm) {
	if(pwm < 0) pwm = 0; 
	if(pwm > 1024) pwm = 1024; 
	TIM16->CCR1 = pwm; 
}

#ifdef DRIVER_USE_SPI
void DriverGPIO_Set1NSS(int state) {
	if(state) GPIOB->BSRR = 0x1000; 
	else			GPIOB->BRR  = 0x1000; 
}
#endif
#endif

#ifdef DRIVER_USE_DRV2

void DriverGPIO_Set2Direction(int state) {
	if(state) GPIOA->BSRR = 0x0020; 
	else			GPIOA->BRR  = 0x0020; 
}

void DriverGPIO_Set2PWM(int pwm) {
	if(pwm < 0) pwm = 0; 
	if(pwm > 1024) pwm = 1024; 
	TIM17->CCR1 = pwm; 
}

#ifdef DRIVER_USE_SPI
void DriverGPIO_Set2NSS(int state) {
	if(state) GPIOA->BSRR = 0x8000; 
	else			GPIOA->BRR  = 0x8000; 
}
#endif
#endif

#endif
