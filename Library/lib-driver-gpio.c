#include <stm32g030xx.h>
#include "lib-driver-gpio.h"


/* ------ Brushed motor driver IOs ------ */

#if defined(DRIVER_USE_DRV1) || defined(DRIVER_USE_DRV2)

void DriverGPIO_Init(void) {

	GPIOA->BSRR = (1 << 5);										// DRVOFF (PA5) HI
	GPIOB->BRR = (1 << 14);										// nSLEEP (PB14) default LO
	GPIOA->MODER = GPIOA->MODER & 0xFFFFF3FF | 0x00000400;		// PA5 set as output
	GPIOB->MODER = GPIOB->MODER & 0xCFFFFFFF | 0x10000000;		// PB14 set as output

	GPIOB->MODER = GPIOB->MODER & 0x3CFFFFFF;					// PB12 (nFAULT1) and PB15 (nFAULT2) as input

#ifdef DRIVER_USE_DRV1

	GPIOB->BSRR = (1 << 1);										// PB1 (PH for DRV1) Hi for FWD direction
	GPIOB->AFR[0] = GPIOB->AFR[0] & ~(0xF << 0) | (1 << 0);		// PB0 as AF1 (TIM3_CH3)
	GPIOB->MODER = GPIOB->MODER & ~(3 << 2) | (1 << 2);			// PB1 as GPoutput
	GPIOB->MODER = GPIOB->MODER & ~(3 << 0) | (2 << 0);			// PB0 as AF mode
	GPIOA->MODER = GPIOA->MODER | (3 << (3*2));					// PA3 as analog mode

	// set up ADC for PA3 for current monitoring in the future ??

#ifdef DRIVER_USE_SPI
	GPIOB->BSRR = (1 << 13);											// SPI_CS1 (PB13) set
	GPIOB->OSPEEDR = GPIOB->OSPEEDR & ~(3 << (13*2)) | (2 << (13*2));	// Hi Speed
	GPIOB->MODER = GPIOB->MODER & ~(3 << (13*2)) | (1 << (13*2));		// PB13 as GPoutput
#endif

	RCC->APBENR1 |= RCC_APBENR1_TIM3EN;									// enable TIM3
	__DSB();
	TIM3->PSC = 3-1;													// PSC: set prescaler
	TIM3->ARR = 1024-1;													// // ARR: set frequency 20.8kHz, 10 bits
	// CCMR2:	set channel 3 as output
	//			capture done ever 4 events
	//			IC3F = 0110
	TIM3->CCMR2 = (3 << 0) | (2 << 2) |  (6 << 4);
	// CCER:	CC3E = 1
	//			CC3P = 0
	TIM3->CCER = (0x01 << 8);
	TIM3->CCR3 = 0x00;													// set duty cycle: 	CCR3 = 0
	TIM3->BDTR = 0x00;													// BDTR = 0
	TIM3->EGR = 0x1;													// EGR:		UG = 1
	TIM3->CR1 = 0X1;													// CR1:		CEN = 1

#endif
#ifdef DRIVER_USE_DRV2

	GPIOA->BSRR = (1 << 6);										// PA6 (PH for DRV2) Hi for FWD direction
	GPIOA->AFR[0] = GPIOA->AFR[0] & ~(0xF << 28) | (5 << 28);	// PA7 as AF5 (TIM17_CH1)
	GPIOA->MODER = GPIOA->MODER & ~(3 << 12) | (1 << 12);		// PA6 as GPoutput
	GPIOB->MODER = GPIOB->MODER & ~(3 << 14) | (2 << 14);		// PA7 as AF mode
	GPIOA->MODER = GPIOA->MODER | (3 << (2*2));					// PA2 as analog mode


#ifdef DRIVER_USE_SPI
	GPIOA->BSRR = (1 << 4);												// SPI_CS2 (PA4) set
	GPIOA->OSPEEDR = GPIOA->OSPEEDR & ~(3 << (4*2)) | (2 << (4*2));		// Hi speed
	GPIOA->MODER = GPIOA->MODER & ~(3 << (4*2)) | (1 << (4*2));			// PA4 as GPoutput
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

// ENABLE DRIVER (1 = en)
void DriverGPIO_SetEnable(int state) {

	if(state)	GPIOB->BSRR = (1 << 14);
	else		GPIOB->BRR  = (1 << 14);

#ifdef DRIVER_USE_DRV1
	TIM3->BDTR = state ? 0x8000 : 0;
#endif
#ifdef DRIVER_USE_DRV2
	TIM17->BDTR = state ? 0x8000 : 0; 
#endif

}


// TURN DRIVERS OFF
void DriverGPIO_SetDrvoff(int state) {

	if(state)	GPIOA->BSRR = (1 << 5);
	else 		GPIOB->BRR  = (1 << 5);
}


// CHECKS IF ANY OF THE DRIVERS HAS A FAULT
int DriverGPIO_HasFault(void) {
	return (!(GPIOB->IDR & (1 << 12)) || !(GPIOB->IDR & (1 << 15)));		// returns 1 if DRV1 has fault OR DRV2 has fault
}


#ifdef DRIVER_USE_DRV1

// SET DIRECTION OF DRIVER 1 (1 = FWD; 0 = BKWD)
void DriverGPIO_Set1Direction(int state) {
	if(state)	GPIOB->BSRR = (1 << 1);		// set PB1 (direction) for FWD
	else		GPIOB->BRR  = (1 << 1);		// reset PB1 (direction) for FWD
}


// SET PWM (SPEED) OF DRIVER 1
void DriverGPIO_Set1PWM(int pwm) {
	if(pwm < 0)	pwm = 0;
	if(pwm > 1024) pwm = 1024; 
	TIM3->CCR3 = pwm; 				// TIM3_CH3
}


#ifdef DRIVER_USE_SPI
// SET SPI OF DRIVER 1
void DriverGPIO_Set1NSS(int state) {
	if(state)	GPIOB->BSRR = (1 << 13);		// set SPI_CS1 (PB13)
	else		GPIOB->BRR  = (1 << 13);
}
#endif

#endif


#ifdef DRIVER_USE_DRV2

// SET DIRECTION OF DRIVER 2
void DriverGPIO_Set2Direction(int state) {
	if(state)	GPIOA->BSRR = (1 << 6); 		// set PA6 --> FWD
	else		GPIOA->BRR  = (1 << 6); 		// BKWD
}


// SET SPEED OF DRIVER 2
void DriverGPIO_Set2PWM(int pwm) {
	if(pwm < 0) pwm = 0; 
	if(pwm > 1024) pwm = 1024; 
	TIM17->CCR1 = pwm; 
}


#ifdef DRIVER_USE_SPI
// SET SPI OF DRIVER 2
void DriverGPIO_Set2NSS(int state) {
	if(state)	GPIOA->BSRR = (1 << 4);
	else		GPIOA->BRR  = (1 << 4);
}
#endif

#endif


#endif
