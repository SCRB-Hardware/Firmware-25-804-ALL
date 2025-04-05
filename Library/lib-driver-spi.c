#include <stm32g030xx.h>
#include "lib-driver-spi.h"

#include "lib-driver-gpio.h"

#ifdef DRIVER_USE_SPI

void DriverSPI_Init(void) {
	//*************New Board**************
	// PB10: SPI2_SCK  (AF5)
	// PB2: SPI2_MISO (AF1)
	// PB11: SPI2_MOSI (AF0)
	//*************Old board**************
	// PB13: SPI2_SCK  (AF0)
	// PB14: SPI2_MISO (AF0)
	// PB15: SPI2_MOSI (AF0)
	GPIOB->AFR[0] = GPIOB->AFR[0] & 0xFFFFF0FF | 0x0000100; 
	GPIOB->AFR[1] = GPIOB->AFR[1] & 0xFFFF00FF | 0x00000500; 
	GPIOB->OSPEEDR = GPIOB->OSPEEDR & 0xFF0FFFCF | 0x00A00020; // 30MHz SPI pins
	GPIOB->MODER = GPIOB->MODER & 0xFF0FFFCF | 0x00A00020; 
	// SPI2 initialization, 8MHz @ 64MHz
	RCC->APBENR1 |= RCC_APBENR1_SPI2EN; 
	SPI2->CR2 = 0x0F00; // 16-bit
	SPI2->CR1 = 0x0355; // Mode 1, 8MHz
}

#ifdef DRIVER_USE_DRV1
uint8_t DriverSPI_ReadDev1(uint8_t reg, uint8_t * val) {
	uint16_t word = 0x4000 | (reg & 0x3F) << 8; 
	while(SPI2->SR & SPI_SR_RXNE) {
		volatile uint16_t d = *((uint16_t *)&SPI2->DR); 
	}
	DriverGPIO_Set1NSS(0); 
	while(!(SPI2->SR & SPI_SR_TXE)); 
	*((uint16_t *)&SPI2->DR) = word; 
	while(!(SPI2->SR & SPI_SR_RXNE)); 
	word = *((uint16_t *)&SPI2->DR); 
	DriverGPIO_Set1NSS(1); 
	*val = word & 0xFF; 
	return word >> 8 & 0x3F; 
}
uint8_t DriverSPI_WriteDev1(uint8_t reg, uint8_t val) {
	uint16_t word = (reg & 0x3F) << 8 | val; 
	while(SPI2->SR & SPI_SR_RXNE) {
		volatile uint16_t d = *((uint16_t *)&SPI2->DR); 
	}
	DriverGPIO_Set1NSS(0); 
	while(!(SPI2->SR & SPI_SR_TXE)); 
	*((uint16_t *)&SPI2->DR) = word; 
	while(!(SPI2->SR & SPI_SR_RXNE)); 
	word = *((uint16_t *)&SPI2->DR); 
	DriverGPIO_Set1NSS(1); 
	return word >> 8 & 0x3F; 
}
#endif

#ifdef DRIVER_USE_DRV2
uint8_t DriverSPI_ReadDev2(uint8_t reg, uint8_t * val) {
	uint16_t word = 0x4000 | (reg & 0x3F) << 8; 
	while(SPI2->SR & SPI_SR_RXNE) {
		volatile uint16_t d = *((uint16_t *)&SPI2->DR); 
	}
	DriverGPIO_Set2NSS(0); 
	while(!(SPI2->SR & SPI_SR_TXE)); 
	*((uint16_t *)&SPI2->DR) = word; 
	while(!(SPI2->SR & SPI_SR_RXNE)); 
	word = *((uint16_t *)&SPI2->DR); 
	DriverGPIO_Set2NSS(1); 
	*val = word & 0xFF; 
	return word >> 8 & 0x3F; 
}
uint8_t DriverSPI_WriteDev2(uint8_t reg, uint8_t val) {
	uint16_t word = (reg & 0x3F) << 8 | val; 
	while(SPI2->SR & SPI_SR_RXNE) {
		volatile uint16_t d = *((uint16_t *)&SPI2->DR); 
	}
	DriverGPIO_Set2NSS(0); 
	while(!(SPI2->SR & SPI_SR_TXE)); 
	*((uint16_t *)&SPI2->DR) = word; 
	while(!(SPI2->SR & SPI_SR_RXNE)); 
	word = *((uint16_t *)&SPI2->DR); 
	DriverGPIO_Set2NSS(1); 
	return word >> 8 & 0x3F; 
}
#endif

#endif
