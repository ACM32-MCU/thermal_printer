#ifndef _USBDESC_H_
#define _USBDESC_H_

#include "ACM32Fxx_HAL.h"  

/**********************************************************
*	definitions
**********************************************************/

/**********************************************************
*	include files
**********************************************************/

/**********************************************************
*	structure
**********************************************************/
typedef struct _USB_DEVICE_DESCRIPTOR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

/*
typedef struct USB_DEVICE_QUALIFIER_DESCRIPTOR {
    BYTE bLength;
    BYTE bDescriptorType;
    WORD bcdUSB;
    BYTE bDeviceClass;
    BYTE bDeviceSubClass;
    BYTE bDeviceProtocol;
    BYTE bMaxPacketSize0;
    BYTE bNumConfigurations;
    BYTE bReserved;
} USB_DEVICE_QUALIFIER_DESCRIPTOR, *PUSB_DEVICE_QUALIFIER_DESCRIPTOR;
// Done!   */

typedef __PACKED_STRUCT  _USB_CONFIGURATION_DESCRIPTOR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t MaxPower;
} USB_CONFIGURATION_DESCRIPTOR;	 


typedef __PACKED_STRUCT  _USB_INTERFACE_DESCRIPTOR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} USB_INTERFACE_DESCRIPTOR;

typedef __PACKED_STRUCT _USB_CCID_DESCRIPTOR {
	uint8_t bLength;			
	uint8_t bDescriptorType;		
	uint16_t bcdCCID;                 
	uint8_t bMaxSlotIndex;		
	uint8_t bVoltageSupport;		
	uint8_t dwProtocols0;
	uint8_t dwProtocols1;
	uint8_t dwProtocols2;
	uint8_t dwProtocols3;

	uint8_t dwDefaultClock0;
	uint8_t dwDefaultClock1;
	uint8_t dwDefaultClock2;
	uint8_t dwDefaultClock3;			
	uint8_t dwMaximumClock0;
	uint8_t dwMaximumClock1;
	uint8_t dwMaximumClock2;
	uint8_t dwMaximumClock3;			
	uint8_t bNumClockSupporte; 
	    
	uint8_t dwDataRate0;
	uint8_t dwDataRate1;
	uint8_t dwDataRate2;
	uint8_t dwDataRate3;			
	uint8_t dwMaxDataRate0;
	uint8_t dwMaxDataRate1;
	uint8_t dwMaxDataRate2;
	uint8_t dwMaxDataRate3;
	uint8_t bNumDataRatesSupp;
	      
	uint32_t dwMaxIFSD;	
	uint32_t dwSynchProtocols;
	uint32_t dwMechanical;		
	uint32_t dwFeatures;		
	uint32_t dwMaxCCIDMessageL;
	       
	uint8_t bClassGetResponse;       
	uint8_t bClassEnvelope;		
	uint16_t wLcdLayout;		
	uint8_t bPINSupport;		
	uint8_t bMaxCCIDBusySlots;  
} USB_CCID_DESCRIPTOR;

//USB  HID描述符结构体 
typedef __PACKED_STRUCT _USB_HID_DESCRIPTOR {
    uint8_t bLength;		          //HID描述符长度,= 09H
    uint8_t bHIDDescriptorType;	  //HID描述符类型,= 21H
    uint8_t bcdHID0;				  //HID Class Specification release number in BCD
	uint8_t bcdHID1;				  
	uint8_t bCountryCode;			  //bCountryCode: none
	uint8_t bNumDescriptors;		  //bNumDescriptors:
	uint8_t bREPORTDescriptorType;  //REPORT描述符类型,= 22H
	uint8_t wDescriptorLength0;	  //REPORT描述符长度
	uint8_t wDescriptorLength1;
} USB_HID_DESCRIPTOR;

typedef __PACKED_STRUCT  _USB_ENDPOINT_DESCRIPTOR {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} USB_ENDPOINT_DESCRIPTOR; 

typedef __PACKED_STRUCT _USB_DESCRIPTOR_NCONFIG
{
	USB_CONFIGURATION_DESCRIPTOR config_descr;

    USB_INTERFACE_DESCRIPTOR 	 inter1_descr; 
	USB_ENDPOINT_DESCRIPTOR		 ep1_descr_in;
	USB_ENDPOINT_DESCRIPTOR		 ep1_descr_out;

} USB_DESCRIPTOR_NCONFIG;

#define CONFIG_TOTAL_LENGTH	  sizeof(USB_DESCRIPTOR_NCONFIG)


extern USB_DESCRIPTOR_NCONFIG  usb_descriptor_nconfig;
//extern uint8_t usb_descriptor_config[32];

/**********************************************************
*	extern variables
**********************************************************/
extern const USB_DEVICE_DESCRIPTOR device_descr;
extern const uint8_t StrDesc_LanguageID[4];
extern const uint8_t StrDesc_Manufacturer[22];
extern const uint8_t StrDesc_Product[30];
extern const uint8_t StrDesc_SerialNumber[34];
//extern USB_STRING_DESCRIPTOR string_descr;
//extern USB_STRING_DESCRIPTOR product_string;
//extern const USB_QUALIFIER_DESCRIPTOR qualifier_descr;
extern uint8_t UsbStrDescSerialNumber[34];
extern const uint8_t usb_report_desc[42];
	




#endif

