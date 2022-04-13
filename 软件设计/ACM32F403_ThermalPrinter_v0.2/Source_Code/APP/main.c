/*
  ******************************************************************************
  * @file    main.c
  * @author  AisinoChip Firmware Team
  * @brief   main source File.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 AisinoChip.
  * All rights reserved.
  ******************************************************************************
*/
#include "printer.h"
#include "power_manage.h"
#include "word_stock.h"
#include "app.h"

extern UART_HandleTypeDef hDbgUart;
extern uint32_t flag_usb_ep1_int;
void printing_test_flow(uint8_t *fu8_Data, uint32_t fu32_Size);

/*********************************************************************************
* Function    : main
* Description : 
* Input       : 
* Outpu       : 
* Author      : Chris_Kyle                         Data : 2020年
**********************************************************************************/
int main(void)
{
		uint8_t gu8_UartBuffer[128];
    HardwareInitialize();
    
    while(1)
    {
        if(gu8_PowerState == POWER_OFF)
        {     
            SystemPowerSwitch(POWER_OFF);
        }
				
        HAL_UART_Receive(&hDbgUart, gu8_UartBuffer, 512, 200);
				if(hDbgUart.lu32_RxCount > 0)
				{
					if((gu8_UartBuffer[0]==0x55) & (gu8_UartBuffer[1]==0x00))				//UART接收数据，下载到SPI flash中
					{
						uart_test_flow(gu8_UartBuffer,hDbgUart.lu32_RxCount);
					}
					else																														//UART接收数据，打印接口打印
					{
						printing_test_flow(gu8_UartBuffer,hDbgUart.lu32_RxCount);
					}
				}
				if( flag_usb_ep1_int ==USB_EP1_OUT_PACKET)  											//USB接收数据，打印接口打印
				{
					usb_transfer_monitor();
				}
    }
}
