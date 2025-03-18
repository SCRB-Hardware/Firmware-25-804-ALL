#ifndef LIB_QUADRATURE_ENC_H__
#define LIB_QUADRATURE_ENC_H__
#include <compile-options.h>

#if defined(ENCODER_USE_1) || defined(ENCODER_USE_2)

extern void QEncoder_Init(void); 

#ifdef ENCODER_USE_1
extern void QEncoder_Reset1(void); 
extern int QEncoder_Read1(void); 
#endif

#ifdef ENCODER_USE_2
extern void QEncoder_Reset2(void); 
extern int QEncoder_Read2(void); 
#endif

#endif
#endif
