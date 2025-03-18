#ifndef LIB_SMART_SERVO_H__
#define LIB_SMART_SERVO_H__

#include <compile-options.h>

#ifdef LSS_USE

#define LSS_SERVO_NUMBER 0

extern void LSS_Init(void); 
extern void LSS_Stop(void); 
extern void LSS_SetPosition(int position); 

#endif

#endif
