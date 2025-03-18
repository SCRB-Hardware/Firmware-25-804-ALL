#ifndef LIB_DRIVER_GPIO_H__
#define LIB_DRIVER_GPIO_H__
#include <compile-options.h>

#if defined(DRIVER_USE_DRV1) || defined(DRIVER_USE_DRV2)

extern void DriverGPIO_Init(void); 

extern void DriverGPIO_SetDrvoff(int state); 
extern void DriverGPIO_SetEnable(int state); 
extern int DriverGPIO_HasFault(void); 

#ifdef DRIVER_USE_DRV1
extern void DriverGPIO_Set1Direction(int state); 
extern void DriverGPIO_Set1PWM(int pwm); 
#ifdef DRIVER_USE_SPI
extern void DriverGPIO_Set1NSS(int state); 
#endif
#endif

#ifdef DRIVER_USE_DRV2
extern void DriverGPIO_Set2Direction(int state); 
extern void DriverGPIO_Set2PWM(int pwm); 
#ifdef DRIVER_USE_SPI
extern void DriverGPIO_Set2NSS(int state); 
#endif
#endif

#endif
#endif
