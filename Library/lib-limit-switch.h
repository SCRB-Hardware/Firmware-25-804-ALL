#ifndef LIB_LIMIT_SWITCH_H__
#define LIB_LIMIT_SWITCH_H__

#include <compile-options.h>

extern void LMSW_Init(void); 
extern int LMSW_Read(int index); 
extern int LMSW_ReadAll(void); 

#endif
