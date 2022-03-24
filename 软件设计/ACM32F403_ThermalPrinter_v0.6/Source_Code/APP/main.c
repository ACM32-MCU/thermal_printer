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
#include "ble.h"
#include "debug.h"
#include "led.h"
#include "middle.h"

extern UART_HandleTypeDef hDbgUart;
extern uint32_t flag_usb_ep1_int;
extern EXTI_HandleTypeDef EXTI_Line13_Handle;
extern UART_HandleTypeDef bleUart;
extern unsigned char Demo_24[89];
extern unsigned char Demo_QC1[1584];
extern unsigned char Demo_Aisino[47];

//#define UART1_Printer
#define BLE_Printer

/*********************************************************************************
* Function    : main
* Description : 
* Input       : 
* Outpu       : 
* Author      : Chris_Kyle                         Data : 2020年
**********************************************************************************/
int main(void)
{
		uint8_t First_batch = 0;
		uint8_t gu8_UartBuffer[256];
		uint16_t Rec_Count,timeout=0;
    HardwareInitialize();
		gu8_PrintDemo = 0;
	
    while(1)
    {
        if((gu8_PowerState == POWER_OFF) | (timeout >= 0x2000))						//下电、超时判断
        {
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_CLEAR); 				//VDD33 OFF
            SystemPowerSwitch(POWER_OFF);
        }
				if(gu8_PrintDemo == 1)																						//双击打印Demo
				{
						printing_test_flow_24(Demo_Aisino,47,2400);
						printing_test_flow_QC(Demo_QC1,1584);
						Vomit_Paper();
						gu8_PrintDemo = 0;
						timeout	= 0;
				}
#ifdef	UART1_Printer
        HAL_UART_Receive(&hDbgUart, gu8_UartBuffer, 512, 200);						//UART1：串口接收数据
				if(hDbgUart.lu32_RxCount > 0)
				{
					if((gu8_UartBuffer[0]==0x55) & (gu8_UartBuffer[1]==0x00))				//UART接收数据，下载到SPI flash中
					{
						uart_test_flow(gu8_UartBuffer,hDbgUart.lu32_RxCount);
					}
					else																														//UART接收数据，打印接口打印
					{
						printing_test_flow_24(gu8_UartBuffer,hDbgUart.lu32_RxCount,480);
						Vomit_Paper();
					}
					timeout=0;
				}
#endif

#ifdef	BLE_Printer
				Rec_Count = 0;
				BLE_UART_Receive(&bleUart, gu8_UartBuffer, 500);							//UART3：BLE接收数据
				Rec_Count = bleUart.lu32_RxCount;
				while(bleUart.lu32_RxCount > 0)
				{
						BLE_UART_Receive(&bleUart, &gu8_UartBuffer[Rec_Count], BLE_Receive_TimeOut);
						Rec_Count += bleUart.lu32_RxCount;
						timeout=0;
				}
				if(Rec_Count > 0)
				{
						if(First_batch ==0)
						{
							First_batch = 1;
//							DbgPrintfBuf(gu8_UartBuffer, Rec_Count, "receive First_batch:");
						}
						else
						{
							printing_test_flow_24(gu8_UartBuffer,Rec_Count,480);
							Vomit_Paper();
						}
				}
				timeout++;
#endif
		}
}
