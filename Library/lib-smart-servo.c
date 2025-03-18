#include <stm32g030xx.h>
#include "lib-smart-servo.h"

/* ------ Smart servo ------ */

#ifdef LSS_USE

uint64_t stats_lss_blocking_wait_cycles = 0; 
uint64_t stats_lss_tx_bytes = 0; 
uint64_t stats_lss_tx_commands = 0; 

void LSS_Init(void) {
	// PA2: USART2_TX (AF1)
	// PA3: USART2_RX (AF1)
	RCC->APBENR1 |= RCC_APBENR1_USART2EN; 
	USART2->BRR = 556; // 115200 @ 64MHz
	USART2->CR1 = 0x9; // Tx only
}

static void LSS_BlockingPrintChar(uint8_t ch) {
	while(!(USART2->ISR & USART_ISR_TXE_TXFNF)) stats_lss_blocking_wait_cycles++; 
	USART2->TDR = ch; 
	stats_lss_tx_bytes++; 
}

static void LSS_BlockingPrintString(const uint8_t * str) {
	for(;;) {
		uint8_t ch = *str; 
		if(ch == 0) break; 
		LSS_BlockingPrintChar(*str++); 
	}
}

static void LSS_BlockingPrintInteger(int32_t num) {
	if(num < 0) {
		LSS_BlockingPrintChar('-'); 
		num = -num; 
	}
	int base = 1000000000; 
	int flag = 0; 
	for(;;) {
		int digit = num / base; 
		num = num - digit * base; 
		if(base == 1) break; 
		base = base / 10; 
		if(digit == 0 && !flag) continue; 
		flag = 1; 
		LSS_BlockingPrintChar('0' + digit); 
	}
	if(!flag) LSS_BlockingPrintChar('0'); 
}


void LSS_Stop(void) {
	LSS_BlockingPrintChar('#'); 
	LSS_BlockingPrintInteger(LSS_SERVO_NUMBER); 
	LSS_BlockingPrintString("L\r"); // Limp or hold
	stats_lss_tx_commands++; 
}

void LSS_SetPosition(int position) {
	// TODO: positional scaling
	LSS_BlockingPrintChar('#'); 
	LSS_BlockingPrintInteger(LSS_SERVO_NUMBER); 
	LSS_BlockingPrintString("WR"); 
	LSS_BlockingPrintInteger(position); // 1/10 degrees
	LSS_BlockingPrintChar('\r'); 
	stats_lss_tx_commands++; 
}

#endif
