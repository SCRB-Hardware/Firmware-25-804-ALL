#include <stm32g030xx.h>
#include "lib-comms.h"

uint8_t Comms_Argc;
Comms_ArgDesc_t Comms_Argv[COMMS_MAX_CMD_ARGS];
uint8_t Comms_CommandBuf[COMMS_MAX_CMD_LENGTH];

#define STATE_IDLE 0 // Searching for SOF
#define STATE_READ 1 // Reading entire length
#define STATE_EXEC 2 // Parsing, dispatching
uint8_t Comms_InternalState;

uint8_t Comms_InternalIndex;

void Comms_Init(void)
{
	Comms_InternalState = STATE_IDLE;
	Comms_InternalIndex = 0;

	// Configure UART GPIO
	// PB6:	UART_US (AF0)
	// PB7:	UART_DS (AF0)
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

	GPIOB->AFR[0] = GPIOB->AFR[0] & 0x00FFFFFF;
	GPIOB->MODER = GPIOB->MODER & 0xFFFF0FFF | 0x0000A000;

	// Enable clock access
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;

	// Configure clock
	// No DE, baud rate 115200
	USART1->CR1 = 0x20200020;
	USART1->CR2 = 0X00000000;
	USART1->CR3 = 0x00001000;
	USART1->BRR = 556;	// baud rate 115200
	USART1->CR1 |= 0x5; // enable RE and UART

	// Enable RX IRQ
	NVIC_EnableIRQ(USART1_IRQn);
}

static int Comms_InternalHandleExec(void)
{
	// Parse all descriptors
	int buflen = Comms_InternalIndex;
	if (buflen == 0)
		return 0; // Invalid if length = 0
	int argindex = 0;
	int bufindex = 0;
	// 1. Skip leading spaces
	for (; bufindex < Comms_InternalIndex; bufindex++)
	{
		if (Comms_CommandBuf[bufindex] != ' ')
			break;
	}
	for (; argindex < COMMS_MAX_CMD_ARGS; argindex++)
	{
		// 2. Find next argument
		int offset = bufindex; // Start of argument
		for (; bufindex < Comms_InternalIndex; bufindex++)
		{
			if (Comms_CommandBuf[bufindex] == ' ')
				break;
		}
		int length = bufindex - offset;
		if (length == 0)
			break; // We have no more
		Comms_Argv[argindex].offset = offset;
		Comms_Argv[argindex].length = length;
		// 3. Skip trailing spaces
		for (; bufindex < Comms_InternalIndex; bufindex++)
		{
			if (Comms_CommandBuf[bufindex] != ' ')
				break;
		}
	}
	if (argindex == 0)
		return 0; // Invalid if no valid args
	Comms_Argc = argindex;
	return 1; // Valid
}

static void Comms_InternalHandleRx(uint8_t ch)
{
	switch (Comms_InternalState)
	{
	case STATE_IDLE:
	{
		if (ch != '#')
			break;
		Comms_InternalState = STATE_READ;
		Comms_InternalIndex = 0;
		Comms_Argc = 0;
		break;
	}
	case STATE_READ:
	{
		if (ch == '\r' || ch == '\n')
		{
			int valid = Comms_InternalHandleExec();
			Comms_InternalState = valid ? STATE_EXEC : STATE_IDLE;
			break;
		}
		if (Comms_InternalIndex >= COMMS_MAX_CMD_LENGTH)
		{
			Comms_InternalState = STATE_IDLE;
			break;
		}
		Comms_CommandBuf[Comms_InternalIndex++] = ch;
		break;
	}
	case STATE_EXEC:
		break;
	default:
		break;
	}
}

void USART1_IRQHandler(void)
{
	uint32_t flags = USART1->ISR;
	if (flags & USART_ISR_RXNE_RXFNE)
	{
		uint8_t ch = USART1->RDR;
		if (ch == 0x03)
		{
			// EMERGENCY KILL
			SCB->AIRCR = 0x05FA0004; // Trigger system reset
			while (1)
				;
		}
		Comms_InternalHandleRx(ch);
	}
}

int Comms_CommandValid(void)
{
	return !!Comms_Argc;
}

void Comms_CommandRetirement(void)
{
	Comms_Argc = 0;
	Comms_InternalState = STATE_IDLE;
}

void Comms_BlockingTxString(const char *str)
{
	// switch RX to TX for transmission
	USART1->CR1 &= 0xFFFFFFF2; // disable TE, RE and UE
	USART1->CR2 |= 0x00008000; // swap
	USART1->CR1 |= 0x00000009; // enable TE and UE

	for (int i = 0; i < 100; i++)
	{ // Max TX length to limit array out of bounds
		char ch = str[i];
		if (ch == 0x0)
			return;
		while (!(USART1->ISR & USART_ISR_TXE_TXFNF))
			;
		USART1->TDR = ch;
	}

	// wait for transmission to finish
	while (!(USART1->ISR & USART_ISR_TC))
		;
	// switch back TX to RX for receive mode
	USART1->CR1 &= 0xFFFFFFF2; // disable TE, RE and UE
	USART1->CR2 &= 0xFFFF7FFF; // un-swap
	USART1->CR1 |= 0x00000005; // enable RE and UE
}

int Comms_ArgCompare(int argindex, const char *str)
{
	if (argindex >= Comms_Argc)
		return 0;
	int length = 0;
	for (; length < 100; length++)
	{
		if (!str[length])
			break;
	}
	if (length != Comms_Argv[argindex].length)
		return 0;
	int offset = Comms_Argv[argindex].offset;
	for (int i = 0; i < length; i++)
	{
		if (str[i] != Comms_CommandBuf[offset + i])
			return 0;
	}
	return 1;
}

int Comms_ArgScanInt(int argindex, int *val)
{
	if (argindex >= Comms_Argc)
		return -1; // Error
	int length = Comms_Argv[argindex].length;
	const uint8_t *ptr = Comms_CommandBuf + Comms_Argv[argindex].offset;
	if (length == 0)
		return -1;
	int negate = 0;
	if (ptr[0] == '-')
	{
		negate = 1;
		ptr++;
		length--;
	}
	if (length == 0)
		return -1;
	if (length > 9)
		return -1;
	int num = 0;
	for (int i = 0; i < length; i++)
	{
		int digit = ptr[i];
		if (digit > '9' || digit < '0')
			return -1;
		digit -= '0';
		num = num * 10 + digit;
	}
	if (negate)
		num = -num;
	*val = num;
	return 0; // Success
}
