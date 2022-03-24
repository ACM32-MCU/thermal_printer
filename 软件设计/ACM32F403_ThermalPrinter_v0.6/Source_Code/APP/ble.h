#include "ACM32Fxx_HAL.h"

#define BLE_Receive_TimeOut 11000

extern UART_HandleTypeDef bleUart;

void BleInit(void);

/* BLE_UART_Receive */
HAL_StatusTypeDef BLE_UART_Receive(UART_HandleTypeDef *huart, uint8_t *fu8_Data, uint32_t fu32_Timeout);

