/***********************************************************************
 * Filename    : app.h
 * Description : app header file
 * Author(s)   : xwl  
 * version     : V1.0
 * Modify date : 2019-09-24
 ***********************************************************************/
#ifndef __USB_APP_H__
#define __USB_APP_H__    

#include "ACM32Fxx_HAL.h"  

/*--------------from usb1.1 protocal------------------*/
#define USB_REMOTE_WAKEUP         0xA0              

#define USB_REQUEST_MASK			              0x0F
#define USB_REQUEST_TYPE_MASK                     0x60
#define USB_STANDARD_REQUEST                      0x00	//标准请求
#define USB_CLASS_REQUEST                         0x20	//类请求
#define USB_VENDOR_REQUEST                        0x40	//厂商请求

#define USB_RECIPIENT_MASK                        0x1F
#define USB_RECIPIENT_DEVICE                      0x00
#define USB_RECIPIENT_INTERFACE                   0x01
#define USB_RECIPIENT_ENDPOINT                    0x02 

/* USB Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL                0
#define USB_FEATURE_REMOTE_WAKEUP                 1
#define USB_FEATURE_TEST_MODE                     2

/* USB GET_STATUS Bit Values */
#define USB_GETSTATUS_SELF_POWERED                0x01
#define USB_GETSTATUS_REMOTE_WAKEUP               0x02
#define USB_GETSTATUS_ENDPOINT_STALL              0x01


//usb Descriptor type	
#define USB_DEVICE_DESCRIPTOR_TYPE		                0x01
#define	USB_CONFIGURATION_DESCRIPTOR_TYPE	            0x02
#define	USB_STRING_DESCRIPTOR_TYPE		                0x03
#define	USB_INTERFACE_DESCRIPTOR_TYPE	            	0x04
#define	USB_ENDPOINT_DESCRIPTOR_TYPE	            	0x05
#define	USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE	        0x06
#define	USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR_TYPE	0x07
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE	            0x08
#define USB_HID_DESCRIPTOR_TYPE                   0x21       //HID描述符
#define USB_REPORT_DESCRIPTOR_TYPE                0x22       //REPORT描述符
#define USB_PHYSICAL_DESCRIPTOR_TYPE              0x23       //PHYSICAL描述符

// defined USB device classes
#define USB_DEVICE_CLASS_RESERVED           0x00
#define USB_DEVICE_CLASS_AUDIO              0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    0x03
#define USB_DEVICE_CLASS_MONITOR            0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 0x05
#define USB_DEVICE_CLASS_POWER              0x06
#define USB_DEVICE_CLASS_PRINTER            0x07
#define USB_DEVICE_CLASS_STORAGE            0x08
#define USB_DEVICE_CLASS_HUB                0x09
#define USB_CLASS_CODE_TEST_CLASS_DEVICE    0xDC
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    0xFF


#define USB_SUBCLASS_CODE_TEST_CLASS         0xA0
#define USB_PROTOCOL_CODE_TEST_CLASS         0xB0

#define INDEX_LANGUAGE_ID       0x00 // 默认Index等于0的字符串描述符描述的是设备所支持的所有语言ID，对应的就是Table 9-15
#define INDEX_MANUFACTURER      0x01
#define INDEX_PRODUCT           0x02
#define INDEX_SERIALNUMBER		0x03

// Standard Request Codes
#define USB_REQUEST_GET_STATUS                    0x00
#define USB_REQUEST_CLEAR_FEATURE                 0x01
#define USB_REQUEST_SET_FEATURE                   0x03
#define USB_REQUEST_SET_ADDRESS                   0x05
#define USB_REQUEST_GET_DESCRIPTOR                0x06
#define USB_REQUEST_SET_DESCRIPTOR                0x07
#define USB_REQUEST_GET_CONFIGURATION             0x08
#define USB_REQUEST_SET_CONFIGURATION             0x09
#define USB_REQUEST_GET_INTERFACE                 0x0A
#define USB_REQUEST_SET_INTERFACE                 0x0B
#define USB_REQUEST_SYNC_FRAME                    0x0C

// Class Request Codes
#define     GET_MAX_LUN                    0xFE
#define     Bulk_Only_Mass_Storage_Reset   0xFF


// Scsi Command Operation Code List.
#define     OPCODE_TEST_UNIT_READY       0x00
#define     OPCODE_REQUEST_SENSE         0x03
#define     OPCODE_MODE_SELECT6          0x15   
#define     OPCODE_MODE_SENSE6           0x1A   
#define     OPCODE_INQUIRY               0x12
#define     OPCODE_START_STOP_UNIT       0x1B
#define     OPCODE_PREVENT_REMOVAL       0x1E
#define     OPCODE_Read_Format_Capacity  0x23
#define     OPCODE_READ_CAPACITY         0x25
#define     OPCODE_READ10                0x28
#define     OPCODE_WRITE10               0x2A
#define     OPCODE_VERIFY10              0x2F
#define     OPCODE_MODE_SENSE10          0x5A  


extern uint8_t out_ep_index;
extern uint8_t in_ep_index;  


void USB_Monitor(void);
void usb_transfer_monitor(void);           

#endif

