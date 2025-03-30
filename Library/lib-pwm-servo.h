#ifndef LIB_PWM_SERVO_H__
#define LIB_PWM_SERVO_H__
#include <compile-options.h>

#ifdef SERVO_USE

extern void Servo_Init(void); 
extern void Servo_Start(void); 
extern void Servo_Stop(void); 
extern void Servo_Write(int motor, int pulse); 

#endif

#endif
