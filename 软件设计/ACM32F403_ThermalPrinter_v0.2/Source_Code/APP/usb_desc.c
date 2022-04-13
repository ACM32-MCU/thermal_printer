/***********************************************************************
 * Filename    : main.c
 * Description : Modules studying
 * Author(s)   : eric   
 * version     : 1.0
 * Modify date : 2012-02-02
 ***********************************************************************/

#include  "usb_app.h"
#include  "usb_desc.h"


const USB_DEVICE_DESCRIPTOR device_descr=
 {
	0x12,
	USB_DEVICE_DESCRIPTOR_TYPE,	//bDescriptorType: DEVICE
	0x0200,	                    //bcdUSB: usb 2.0

	//USB_CLASS_CODE_TEST_CLASS_DEVICE,		//bDeviceClass: independent interfaces
	0,				           
	0,				            //bDeviceSubClass: 0
	0,				            //bDeviceProtocol: class specific protocols NOT used on device basis
	EP0_MAX_PACKET_SIZE,	    //bMaxPacketSize0: maximum packet size for endpoint zero
	0x1234,	                    //idVendor	//0x101d	  	
	0xABCD,	                    //idProduct

	0x0200,	                    //bcdDevice
	0,                    	//iManufacturer: index of string
	0,                       //iProduct: index of manufacturer string
	0,                          //iSerialNumber: index of product string
	0x01                        //bNumConfigurations: 1 configuration
};

//uint8_t usb_descriptor_config[32]=
//{
//	0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0xfa,
//  	//0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, USB_REMOTE_WAKEUP, 0xfa,
//  	0x09, 0x04, 0x00, 0x00, 0x02, USB_CLASS_CODE_TEST_CLASS_DEVICE, USB_SUBCLASS_CODE_TEST_CLASS, USB_PROTOCOL_CODE_TEST_CLASS, 0x00,
//  	0x07, 0x05, (EP_DIR_IN+USB_EP_IN), 0x02, EPX_MAX_PACKET_SIZE&0xff, EPX_MAX_PACKET_SIZE>>8, 0x00,
//  	0x07, 0x05, (EP_DIR_OUT+USB_EP_OUT), 0x02, EPX_MAX_PACKET_SIZE&0xff, EPX_MAX_PACKET_SIZE>>8, 0x00
//};	

USB_DESCRIPTOR_NCONFIG 	usb_descriptor_nconfig=
{
   //ÅäÖÃÃèÊö·û
  {
	0x09,	             //bLength
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	//bDescriptor 02
	CONFIG_TOTAL_LENGTH,   //wTotalLength 
	0x01,	          	   //bNumInterface       	   
	0x01,		           //bConfiguration Value
	0x00,			       //iConfiguration
	0x80,			       //bmAttributes,BUS-POWER
	0xfa                   //0x64	 //bMaxPower,200mA
  },

  //½Ó¿Ú1ÃèÊö·û(UMS1)
  {
	0x09,		         //bLength: 0x09 bytes
	0x04,		         //bDescriptorType: INTERFACE
	0x00,                //bInterfaceNumber: interface 0
	0x00,                //bAlternateSetting: alternate setting 0
	0x02,                //bNumEndpoints: 2 endpoint + zero ep

	USB_CLASS_CODE_TEST_CLASS_DEVICE,   //bInterfaceClass: vendor specific interface
	USB_SUBCLASS_CODE_TEST_CLASS,      //bInterfaceSubClass: 06 SCSI Command Set
	USB_PROTOCOL_CODE_TEST_CLASS,//bInterfaceProtocol:

	0x00                 //iInterface: index of string
  },
  //¶ËµãÃèÊö·û£¬EP1 BULK IN
  {
	0x07,			     //bLength: 0x07 bytes
	0x05,                //bDescriptorType: ENDPOINT
	(EP_DIR_IN+USB_EP_IN),                //bEndpointAddress: IN endpoint 1
	0x02,                //bmAttributes: BULK-ONLY(00:Control 01:isoch 10:bulk 11:intr
	EPX_MAX_PACKET_SIZE,    //wMaxPacketSize: 64 bytes
	0x00                 //bInterval: polling interval is 1 ms
  },
  //¶ËµãÃèÊö·û£¬EP1 BULK OUT
  {
	 0x07,               //bLength: 0x07 bytes
	 0x05,               //bDescriptorType: ENDPOINT
	 (EP_DIR_OUT+USB_EP_OUT),               //bEndpointAddress: out endpoint 1
	 0x02,               //bmAttributes: BULK-ONLY
	 EPX_MAX_PACKET_SIZE,   //wMaxPacketSize: 64 bytes
	 0x00		    	 //bInterval: polling interval is 1 ms
  }

};


const uint8_t StrDesc_LanguageID[4] = 
{
	4,				// Num bytes of this descriptor
	3,				// String descriptor
	0x09,			// Language ID LSB
	0x04			// Language ID	
};

const uint8_t StrDesc_Manufacturer[22]=
{
	22,				// Num bytes of this descriptor
	3,				// String descriptor
	'A',	0,
	'i',	0,
	's',	0,
	'i',	0,
	'n',	0,
	'o',	0,
	'C',	0,
	'h',	0,
	'i',	0,
	'p',	0
};

const uint8_t StrDesc_Product[30]=
{
	30,				// Num bytes of this descriptor
	3,				// String descriptor
	'U',	0,
	'S',	0,
	'B',	0,
	' ',	0,
	'F',	0,
	'l',	0,
	'a',	0,
	's',	0,
	'h',	0,
	' ',	0,
	'D',	0,
	'i',	0,
	's',	0,
	'k',	0
};

const uint8_t StrDesc_SerialNumber[34]=
{
	34,			// Num bytes of this descriptor
	3,			// String descriptor
	'1',	0,
	'2',	0,
	'3',	0,
	'4',	0,
	'5',	0,
	'6',	0,
	'7',	0,
	'8',	0,
	'9',	0,
	'0',	0,
	'A',	0,
	'B',	0,
	'C',	0,
	'D',	0,
	'E',	0,
	'F',	0
};



