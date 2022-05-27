/***********************************************************************
 * Filename    : app.c
 * Description : app source file
 * Author(s)   : xwl  
 * version     : V1.0
 * Modify date : 2020-04-07  
 ***********************************************************************/
#include  "app.h"
#include "debug.h"
void printing_test_flow(uint8_t *fu8_Data, uint32_t fu32_Size);

BULK_IO_REQUEST bulk_io_request;
uint32_t g_bulk_ack = 0x53425355;  
uint8_t g_usb_data_buffer[1024];   

void USB_CMD_proc(void)
{
	uint32_t length;
	uint8_t  g_io_phase;

  length = HAL_FSUSB_Get_FIFO_Length(out_ep_index);  

	/*-----------cmd stage-- CMD with 8 bytes ------------*/

 	if(length!=8)	
	{
		printfS("fifo length==%d \n", length);

		//usb_clear_fifo(in_ep_index, EP_DIR_IN);  

		USBCTRL->EPxCSR[out_ep_index] |= 1<<11; 										//set rx ready 	
		return;
	}	
	else
	{ 
		HAL_FSUSB_Read_EP_MEM8( (uint8_t *)(&bulk_io_request), 8, 0, out_ep_index );   	
		USBCTRL->EPxCSR[out_ep_index] |= 1<<11; 										//set rx ready 	       
	}

	length= (bulk_io_request.Length[2]<<16) + (bulk_io_request.Length[1]<<8) + (bulk_io_request.Length[0]);
	g_io_phase = bulk_io_request.BulkIoPhase;

	/*-----------data stage--------------*/
	if(g_io_phase==BulkIoPhase_Out)	  
	{	   
		printfS("usb data out \n");
		HAL_FSUSB_Receive_Data(g_usb_data_buffer,length,out_ep_index); 
		
		printing_test_flow(g_usb_data_buffer,length);								//Êý¾Ý´òÓ¡
		
		DbgPrintfBuf(g_usb_data_buffer, length, "USB Receive data:");
  }
	else if(g_io_phase==BulkIoPhase_In)	  
	{
		printfS("usb data in \n");
	  HAL_FSUSB_Send_Data(g_usb_data_buffer,length,in_ep_index);
		DbgPrintfBuf(g_usb_data_buffer, length, "USB Send data:");
	}
	
	/*-----------ACK stage--------------*/
	printfS("return ACK \n");	
	HAL_FSUSB_Send_Data((uint8_t *)&g_bulk_ack,4,in_ep_index);    

}  

