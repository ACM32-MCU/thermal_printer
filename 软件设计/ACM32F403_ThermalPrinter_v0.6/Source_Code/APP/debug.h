#ifndef __DEBUG__H_H
#define __DEBUG__H_H

#include "stdint.h"
#include "ACM32Fxx_HAL.h"

extern UART_HandleTypeDef hDbgUart;

#define DbgPrintf  printfS

void DebugInit(void);
void DbgPrintfBuf(uint8_t *buf, uint32_t len, const char *name);


#endif