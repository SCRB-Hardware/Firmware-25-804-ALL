#ifndef LIB_DRIVER_SPI_H__
#define LIB_DRIVER_SPI_H__
#include <compile-options.h>

#define DRV8245_REG_DEVID 	0x00
#define DRV8245_REG_FAULT		0x01
#define DRV8245_REG_STA1		0x02
#define DRV8245_REG_STA2		0x03
#define DRV8245_REG_CMD			0x08
#define DRV8245_REG_SPI_IN	0x09
#define DRV8245_REG_CFG1		0x0A
#define DRV8245_REG_CFG2		0x0B
#define DRV8245_REG_CFG3		0x0C
#define DRV8245_REG_CFG4		0x0D

#ifdef DRIVER_USE_SPI

extern void DriverSPI_Init(void); 

#ifdef DRIVER_USE_DRV1
extern uint8_t DriverSPI_ReadDev1(uint8_t reg, uint8_t * val); 
extern uint8_t DriverSPI_WriteDev1(uint8_t reg, uint8_t val); 
#endif

#ifdef DRIVER_USE_DRV2
extern uint8_t DriverSPI_ReadDev2(uint8_t reg, uint8_t * val); 
extern uint8_t DriverSPI_WriteDev2(uint8_t reg, uint8_t val); 
#endif

#endif

#endif
