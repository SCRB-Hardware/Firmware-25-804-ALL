#ifndef COMPILE_OPTIONS_H__
#define COMPILE_OPTIONS_H__

// Which subsystem?
//#define SYSTEM_ELV
#define SYSTEM_DSS
//#define SYSTEM_SSS

/* ------ Preset defines ------ */

// Mutual exclusive check
#if defined(SYSTEM_ELV) + defined(SYSTEM_DSS) + defined(SYSTEM_SSS) == 0
// Custom rules
// ...
#else

#if defined(SYSTEM_ELV) + defined(SYSTEM_DSS) + defined(SYSTEM_SSS) != 1
#error "More than one subsystem defined."
#endif

// Elevation control
#ifdef SYSTEM_ELV
//efine DRIVER_USE_DRV1
#define DRIVER_USE_DRV2		// Elevation motor
#define DRIVER_USE_SPI		// SPI-enabled driver
//efine ENCODER_USE_1
#define ENCODER_USE_2			// Elevation motor encoder
#define SERVO_USE					// Camera servo (only 1)
													// Smart servo: not used
													// Limit switch: 1&2 for payload box, 3&4 for manual control
#endif

// Deep sampling
#ifdef SYSTEM_DSS
#define DRIVER_USE_DRV1		// Lifting motor
#define DRIVER_USE_DRV2		// Drilling motor
#define DRIVER_USE_SPI		// SPI-enabled driver
#define ENCODER_USE_1			// Lifting motor
//efine ENCODER_USE_2			// Drilling motor
//efine SERVO_USE
													// Smart servo: not used
													// Limit switch: 1&2 for drill head
#endif

// Surface sampling
#ifdef SYSTEM_SSS
#define DRIVER_USE_DRV1		// Vibration motor (small)
//efine DRIVER_USE_DRV2
//efine DRIVER_USE_SPI
//efine ENCODER_USE_1
//efine ENCODER_USE_2
#define SERVO_USE					// Payload servos
													// Smart servo: used
													// Limit switch: none
#endif

#endif

/* ------ Static checks ------ */

// Encoder 1 conflicts with all servos
#ifdef ENCODER_USE_1
#if defined(SERVO_USE_12) || defined(SERVO_USE_34)
#error "PWM servos cannot be used with encoder 1. They share TIM1."
#endif
#endif

#endif
