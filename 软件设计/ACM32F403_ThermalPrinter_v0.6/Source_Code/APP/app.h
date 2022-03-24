/***********************************************************************
 * Filename    : app.h
 * Description : app header file
 * Author(s)   : xwl  
 * version     : V1.0
 * Modify date : 2019-09-24
 ***********************************************************************/
#ifndef __APP_H__
#define __APP_H__

#include "usb_app.h"  

typedef struct _bulk_io_request
{
	UINT32 dBulkIoSignature;  //具体的COMMAND
	UINT8  BulkIoPhase;     //下一帧数据的传输方向
	UINT8  Length[3];         //传输的数据BYTE数
} BULK_IO_REQUEST;


#define BulkIoPhase_In			0x80
#define BulkIoPhase_Out 		0x00
#define BulkIoSignature			0x43425355
#define BULK_IO_ACK				0x53425355

void USB_CMD_proc(void);  
        

#endif

