#include "ble.h"
#include "debug.h"

UART_HandleTypeDef bleUart;

/*********************************************************************************
* Function    : HAL_UART_MspInit
* Description : Initialize the UART MSP.
* Input       : huart: UART handle.
* Output      : 
**********************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

    GPIO_InitTypeDef    GPIO_InitStruct ={0};
    
    if (huart->Instance == UART1) 
    {
        /* Enable Clock */
        System_Module_Enable(EN_UART1);
       
        /* Initialization GPIO */
        /* A9:Tx  A10:Rx */
        GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Alternate = GPIO_FUNCTION_2;       
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
        /* NVIC Config */
        NVIC_ClearPendingIRQ(UART1_IRQn);
        NVIC_SetPriority(UART1_IRQn, 5);
        NVIC_EnableIRQ(UART1_IRQn);
    }
		else if (huart->Instance == UART3) 
    {
        /* Enable Clock */
        System_Module_Enable(EN_UART3);
       
        /* Initialization GPIO */
        /* B10:Tx  B11:Rx */
        GPIO_InitStruct.Pin       = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Alternate = GPIO_FUNCTION_2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* NVIC Config */
        NVIC_ClearPendingIRQ(UART3_IRQn);
        NVIC_SetPriority(UART3_IRQn, 5);
        NVIC_EnableIRQ(UART3_IRQn);
			
    }
}

void BleInit(void)
{
	bleUart.Instance        = UART3;
	bleUart.Init.BaudRate   = 115200;
	bleUart.Init.WordLength = UART_WORDLENGTH_8B;
	bleUart.Init.StopBits   = UART_STOPBITS_1;
	bleUart.Init.Parity     = UART_PARITY_NONE;
	bleUart.Init.Mode  			= UART_MODE_TX_RX;
	bleUart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	HAL_UART_Init(&bleUart);
}

/*********************************************************************************
* Function    : BLE_UART_Receive
* Description : Receive an amount of data in blocking mode.
* Input       : huart:        UART handle.
* Input       : fu8_Data:     Pointer to data buffer.
* Input       : fu32_Size:    Amount of data elements to be receive.
* Input       : fu32_Timeout: Timeout duration.
* Output      : HAL status
* Author      : Chris_Kyle                         Data : 2020 
**********************************************************************************/
HAL_StatusTypeDef BLE_UART_Receive(UART_HandleTypeDef *huart, uint8_t *fu8_Data, uint32_t fu32_Timeout)
{
		uint32_t lu32_Timeout;
    huart->lu32_RxCount = 0;
	
		while(1)
		{
				lu32_Timeout = fu32_Timeout * 256;
			
				while(huart->Instance->FR & UART_FR_RXFE)
				{
						if (lu32_Timeout-- == 0) 
						{
								/* Clear Half duplex */
								huart->Instance->CR2 &= ~UART_CR2_RX_SEL;
												
								return HAL_TIMEOUT;
						}
				}

				*fu8_Data++ = huart->Instance->DR;

				huart->lu32_RxCount++;
		}
	
    /* Clear Half duplex */
    huart->Instance->CR2 &= ~UART_CR2_RX_SEL;
    
    return HAL_OK;
}

