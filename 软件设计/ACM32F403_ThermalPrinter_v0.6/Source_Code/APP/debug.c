#include "debug.h"

UART_HandleTypeDef hDbgUart;


void DebugInit(void)
{
	hDbgUart.Instance        = UART1;
	hDbgUart.Init.BaudRate   = 115200;
	hDbgUart.Init.WordLength = UART_WORDLENGTH_8B;
	hDbgUart.Init.StopBits   = UART_STOPBITS_1;
	hDbgUart.Init.Parity     = UART_PARITY_NONE;
	hDbgUart.Init.Mode       = UART_MODE_TX_RX_DEBUG;
	hDbgUart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;

	HAL_UART_Init(&hDbgUart);

	// UART_DEBUG_ENABLE control printfS
	DbgPrintf("MCU is running, HCLK=%dHz, PCLK=%dHz\n", System_Get_SystemClock(), System_Get_APBClock());
}

void DbgPrintfBuf(uint8_t *buf, uint32_t len, const char *name)
{
	uint32_t i;
	
	DbgPrintf("%s\r\n", name);

	for(i = 0; i < len;)
	{
		DbgPrintf("%02X ", buf[i]);
		i++;

		if(i & 0x0F) continue;

		DbgPrintf("\r\n");
	}

	if(len & 0x0F)
		DbgPrintf("\r\n");

}