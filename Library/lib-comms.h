#ifndef LIB_COMMS_H__
#define LIB_COMMS_H__

#define COMMS_MAX_CMD_LENGTH 64 // Excluding SOF & EOL
#define COMMS_MAX_CMD_ARGS 8

typedef struct {
	uint8_t offset; 
	uint8_t length; 
} Comms_ArgDesc_t; 

extern uint8_t Comms_Argc; 
extern Comms_ArgDesc_t Comms_Argv[COMMS_MAX_CMD_ARGS]; 
extern uint8_t Comms_CommandBuf[COMMS_MAX_CMD_LENGTH]; 


extern void Comms_Init(void); 

extern int Comms_CommandValid(void); 
extern void Comms_CommandRetirement(void); 
extern void Comms_BlockingTxString(const char * str); 

extern int Comms_ArgCompare(int argindex, const char * str); 
extern int Comms_ArgScanInt(int argindex, int * val); 

#endif
