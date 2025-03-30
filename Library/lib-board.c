#include <stm32g030xx.h>
#include "lib-board.h"

void Board_Init(void)
{
	//Enable all GPIOs
	RCC->IOPENR |= 0x1F;

	//Enable PANIK LED (PB4) & RUN LED (PB5)
	GPIOB->BSRR = 0x30;
	GPIOC->OTYPER |= 0x30;

	//Enables Key
	//GPIOC->PUPDR = GPIOC->PUPDR & 0x3FFFFFFF | 0x40000000;

	//PB4 (LED_PANIK) and PB5 (LED_RUN) as General purpose output mode
	GPIOB->MODER = GPIOB->MODER & 0xFFFFF0FF | 0x500;

	//Enables C5V_PV, M5V_PV, S12V_PV
	//GPIOB->MODER = GPIOB->MODER & 0xFFFFF03F;
	
	
	//Debug
	GPIOA->MODER |= (2<<28)  | (2<<26);
	GPIOA->AFR[1] = GPIOA->AFR[1] & (0xF00FFFF) ;
}

//state = 1 --> LED_PANIK turns ON
//state = 0 --> LED_PANIK turns OFF
void Board_SetPanicLed(int state)
{
	if (state)
		GPIOB->BRR = 0x10;
	else
		GPIOB->BSRR = 0x10;
}

//state = 1 --> LED_RUN turns ON
//state = 0 --> LED_RUN turns OFF
void Board_SetRunLed(int state)
{
	if (state)
		GPIOB->BRR = 0x20;
	else
		GPIOB->BSRR = 0x20;
}


//Read from KEY, SENSOR, SERVO and SMART SERVO
//
//int Board_ReadButton(void)
//{
//	return !(GPIOC->IDR & 0x8000);
//}
//
//int Board_ReadSensorPV(void)
//{
//	return !!(GPIOB->IDR & 0x0008);
//}
//
//int Board_ReadServoPV(void)
//{
//	return !!(GPIOB->IDR & 0x0010);
//}
//
//int Board_ReadSmartServoPV(void)
//{
//	return !!(GPIOB->IDR & 0x0020);
//}
