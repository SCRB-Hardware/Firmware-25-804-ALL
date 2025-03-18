#include <stm32g030xx.h>
#include "lib-pwm-servo.h"


/* ------ Regular PWM Servos ------ */

#ifdef SERVO_USE

void Servo_Init(void) {
	// PA8:  Servo 1, TIM1_CH1 (AF2)
	// PA9:  Servo 2, TIM1_CH2 (AF2)
	// PA10: Servo 3, TIM1_CH3 (AF2)
	// PA11: Servo 4, TIM1_CH4 (AF2)
	GPIOA->AFR[1] = GPIOA->AFR[1] & 0xFFFF0000 | 0x00002222; 
	GPIOA->MODER = GPIOA->MODER & 0xFF00FFFF | 0x00AA0000; 
	// Initialize Timer 1 @ 64MHz for standard PWM generation
	RCC->APBENR2 |= RCC_APBENR2_TIM1EN; 
	__DSB(); 
	TIM1->CR1 = 0x04; // Allow only overflow IRQ
	TIM1->PSC = 64 - 1; 
#define SERVO_MAX_PULSE 20000
	TIM1->ARR = SERVO_MAX_PULSE - 1; // 50Hz, 1us resolution
	TIM1->CCR1 = SERVO_MAX_PULSE; 
	TIM1->CCR2 = SERVO_MAX_PULSE; 
	TIM1->CCR3 = SERVO_MAX_PULSE; 
	TIM1->CCR4 = SERVO_MAX_PULSE; 
	TIM1->CCMR1 = 0x7878; 
	TIM1->CCMR2 = 0x7878; 
	TIM1->CCER = 0x1111; 
	TIM1->EGR = 0x1; 
	__DSB(); 
	TIM1->SR = 0x0; 
	// Enable NVIC IRQ but not in DIER
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn); 
}

void Servo_Start(void) {
	TIM1->CR1 |= 0x1; 
	TIM1->BDTR = 0x8000; // Enable output
}

void Servo_Stop(void) {
	TIM1->CR1 |= 0x9; // One pulse mode stops the timer afterwards
	TIM1->SR = 0x00; 
	__DSB(); 
	TIM1->DIER = 0x01; // Enable IRQ
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) { // IRQ handler
	TIM1->SR = ~0x1; // Clear flag
	if(!(TIM1->CR1 & 0x1)) { // If timer stopped
		TIM1->BDTR = 0x0000; // Disable output
		TIM1->DIER = 0x00; // Disable IRQ
		TIM1->CR1 &= ~0x8; // Clear OPM
	}
}

void Servo_Write(int motor, int pulse) {
	if(pulse > SERVO_MAX_PULSE) pulse = SERVO_MAX_PULSE; 
	if(pulse < 0) pulse = 0; 
	pulse = SERVO_MAX_PULSE - pulse; 
	switch(motor) {
		case 1: TIM1->CCR1 = pulse; break; 
		case 2: TIM1->CCR2 = pulse; break; 
		case 3: TIM1->CCR3 = pulse; break; 
		case 4: TIM1->CCR4 = pulse; break; 
		default: return; 
	}
}

#endif
