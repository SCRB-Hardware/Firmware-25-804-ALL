#ifndef LIB_BOARD_H__
#define LIB_BOARD_H__

extern void Board_Init(void); 

extern void Board_SetPanicLed(int state); 
extern void Board_SetRunLed(int state); 

extern int Board_ReadButton(void); 

extern int Board_ReadSensorPV(void); 
extern int Board_ReadServoPV(void); 
extern int Board_ReadSmartServoPV(void); 

#endif
