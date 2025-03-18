#include <stm32g030xx.h>

#include "lib-rcc.h"
#include "lib-board.h"
#include "lib-comms.h"

// Servos
#include "lib-pwm-servo.h"

// Motors
#include "lib-driver-gpio.h"
#include "lib-driver-spi.h"

// Encoders
#include "lib-quadrature-enc.h"

// Switches
#include "lib-limit-switch.h"

#ifdef SYSTEM_DSS

// Main motor direction
#define DSS_UP 		1
#define DSS_DOWN 	0

// Homing
#define HOME_PWM 400
uint8_t dss_homed = 0; 
void dss_home(int feedback); 

// Moving
#define MOVE_CONVERSION 623 // Factor: number of pulses per mm
#define MOVE_DOWN_LIMIT 152 // Moving down limit in mm
#define MOVE_TOLERANCE 100	// Tolerance in encoder pulses
#define MOVE_PWM 400				// PWM when moving
void dss_move_to(int position); 

#define DRILL_CW	1
#define DRILL_CCW	0
void dss_drill(int power); 

int launch(void) {
	// Resources used: 
	// Elevation: Driver 2, Encoder 2, LMSW 1,2
	// Camera: PWM servo 1
	
	DriverGPIO_Init(); 
	DriverSPI_Init(); 
	QEncoder_Init(); 
	LMSW_Init(); 
	
	// Power-on Temporization
	Sys_BlockingDelayM(100); 
	
	// Enable motor driver
	// 1. Exit sleep mode
	DriverGPIO_SetEnable(1); 
	Sys_BlockingDelayM(1); 
	// 2. Clear POR flag
	DriverSPI_WriteDev1(DRV8245_REG_CMD, 0x80); 
	DriverSPI_WriteDev2(DRV8245_REG_CMD, 0x80); 
	// 3. Set max slew rate
	DriverSPI_WriteDev1(DRV8245_REG_CFG3, 0x1C); 
	DriverSPI_WriteDev2(DRV8245_REG_CFG3, 0x1C); 
	// 4. Release inhibit
	DriverGPIO_SetDrvoff(0); 
	
	// Automatic homing
//	Sys_BlockingDelayM(100); 
//	dss_home(0); 
	
	// Initialization done
	Board_SetPanicLed(0); 
	
	// Main loop
	while(1) {
		// Commit last command
		Board_SetRunLed(0); 
		Comms_CommandRetirement(); 
		
		// Check for new command
		while(!Comms_CommandValid()); 
		Board_SetRunLed(1); 
		Sys_BlockingDelayM(10); 
		// Check for command match
		if(!Comms_ArgCompare(0, "DSS")) continue; 
		// Matching commands
		if(Comms_ArgCompare(1, "HOME")) {
			dss_home(1); 
			continue; 
		}
		if(Comms_ArgCompare(1, "MOVE")) {
			int position; 
			if(Comms_ArgScanInt(2, &position)) {
				Comms_BlockingTxString("Parameter 2 should be an integer.\n\n"); 
				continue; 
			}
			dss_move_to(position); 
			continue; 
		}
		if(Comms_ArgCompare(1, "DRILL")) {
			int power; 
			if(Comms_ArgScanInt(2, &power)) {
				Comms_BlockingTxString("Parameter 2 should be an integer.\n\n"); 
				continue; 
			}
			dss_drill(power); 
			continue; 
		}
		Comms_BlockingTxString("Command not found\n\n"); 
	}
}

void dss_home(int feedback) {
	if(LMSW_Read(1)) {
		// If limit switch already closed:
		// Set motor to move DOWN
		if(feedback)
			Comms_BlockingTxString("Homing down...\n"); 
		DriverGPIO_Set1Direction(DSS_DOWN); 
		DriverGPIO_Set1PWM(HOME_PWM); 
		while(LMSW_Read(1)); 
		Sys_BlockingDelayM(10); 
		DriverGPIO_Set1PWM(0); 
		Sys_BlockingDelayM(50); 
	}
	else if(feedback) 
		Comms_BlockingTxString("Already down.\n"); 
	// Set motor to move UP
	if(feedback) 
		Comms_BlockingTxString("Homing up...\n"); 
	DriverGPIO_Set1Direction(DSS_UP); 
	DriverGPIO_Set1PWM(HOME_PWM); 
	while(!LMSW_Read(1)); 
	Sys_BlockingDelayM(10); 
	DriverGPIO_Set1PWM(0); 
	Sys_BlockingDelayM(200); 
	QEncoder_Reset1(); 
	dss_homed = 1; 
	if(feedback) 
		Comms_BlockingTxString("Homing complete.\n\n"); 
}

#define read_enc() (-QEncoder_Read1())

void dss_move_to(int position) {
	if(!dss_homed) {
		Comms_BlockingTxString("Please home drill first.\n\n"); 
		return; 
	}
	if(position < 0) {
		Comms_BlockingTxString("Cannot move beyond top limit.\n\n"); 
		return; 
	}
	if(position > MOVE_DOWN_LIMIT) {
		Comms_BlockingTxString("Cannot move beyond bottom limit.\n\n"); 
		return; 
	}
	position *= MOVE_CONVERSION; 
	int current = read_enc(); 
	int diff = position - current; 
	if(diff < 0) diff = -diff; 
	if(diff < MOVE_TOLERANCE) {
		Comms_BlockingTxString("Already close enough to target position.\n\n"); 
		return; 
	}
	Comms_BlockingTxString("Moving...\n"); 
	DriverGPIO_Set1Direction(position > current ? DSS_DOWN : DSS_UP); 
	DriverGPIO_Set1PWM(MOVE_PWM); 
	if(position > current) {
		// We are moving down
		while(read_enc() < position); 
	}
	else {
		// We are moving up
		while(read_enc() > position); 
	}
	Sys_BlockingDelayM(1); 
	DriverGPIO_Set1PWM(0); 
	Comms_BlockingTxString("Moved to target position.\n\n"); 
}

void dss_drill(int power) {
	static int8_t prevsign; 
	if(power < 0) {
		DriverGPIO_Set2Direction(DRILL_CW); 
		power = -power; 
		if(prevsign > 0) {
			DriverGPIO_Set2PWM(0); 
			Sys_BlockingDelayM(500); 
		}
		prevsign = -1; 
	}
	else {
		DriverGPIO_Set2Direction(DRILL_CCW); 
		if(prevsign < 0) {
			DriverGPIO_Set2PWM(0); 
			Sys_BlockingDelayM(500); 
		}
		prevsign = 1; 
	}
	DriverGPIO_Set2PWM(power); 
	Comms_BlockingTxString("Drill commanded.\n\n"); 
}

#endif
