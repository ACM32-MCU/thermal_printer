/***********************************************************************
 * Filename    : app.c
 * Description : app source file
 * Author(s)   : xwl  
 * version     : V1.0
 * Modify date : 2020-04-07  
 ***********************************************************************/
#include  "app.h"
#include  "usb_app.h"  
#include  "usb_desc.h" 

#if 1   
static DEVICE_REQUEST dev_req;  
static uint8_t config_value=0;
static uint8_t AltSetting=0;
static uint16_t usb_device_status=0;  // 远程唤醒和自供电状态

volatile uint32_t flag_usb_int=0;
volatile uint32_t flag_usb_ep1_int=0;
volatile uint32_t flag_usb_ep2_int=0;
volatile uint32_t flag_usb_ep3_int=0;
volatile uint32_t flag_usb_ep4_int=0;
uint8_t MaxLun=0; 

uint8_t out_ep_index=0;
uint8_t in_ep_index=0;  

//BYTE flag_set_report = 0;
//BYTE flag_get_report = 0;

void USB_IRQHandler(void)
{
	volatile uint32_t	temp=0;

	temp = USBINT->INT_STAT_RAW;  

	if(temp&USB_SUSPEND)	  //suspend
	{		
		flag_usb_int |=USB_SUSPEND;
		USBINT->INT_CLR = USB_SUSPEND;  
	}
	if(temp&USB_RESUME)	  //resume
	{
		flag_usb_int |=USB_RESUME;
		USBINT->INT_CLR = USB_RESUME;
	}
	if(temp&USB_BUS_RESET)	  	//bus reset	  
	{		
		flag_usb_int |=USB_BUS_RESET;
		USBINT->INT_CLR = USB_BUS_RESET;
	}	

	if(temp &USB_EP0_SETUP_PACKET)	//EP0
	{
		flag_usb_int |=USB_EP0_SETUP_PACKET; 
		USBINT->INT_CLR = USB_EP0_SETUP_PACKET;  
	}
	
	if(!flag_usb_ep1_int)
	{
		if(temp &USB_EP1_OUT_PACKET)	//EP1
		{   
			while(!(USBINT->INT_STAT_RAW & MASK_EPX_ACK(USB_EP1))){}; // wait for ACK sent to host    
            USBINT->INT_CLR = MASK_EPX_ACK(USB_EP1); 
			if(USBCTRL->EPxCSR[1] & ( 1<< 19) )//Toggle error 
			{
				USBCTRL->EPxCSR[1] ^= (1<<17); //out toggle want
				USBCTRL->EPxCSR[1] |= (1<<18); //update want toggle; 
				USBCTRL->EPxCSR[1] |= 1<<11;   //set rx ready, wait for a new packet   
                USBINT->INT_CLR = USB_EP1_OUT_PACKET | USB_EP1_IN; 
                return; //not process this frame. device has sent response before     				
			}		
				
			flag_usb_ep1_int = USB_EP1_OUT_PACKET; 
			// clear in token flag, for Interrupt tranfer type, in token flag may set after sending response    
			USBINT->INT_CLR = USB_EP1_OUT_PACKET | USB_EP1_IN;  
		}  
	} 
	if(!flag_usb_ep2_int)
	{
		if(temp &USB_EP2_OUT_PACKET)	//EP2
		{  		
			while(!(USBINT->INT_STAT_RAW & MASK_EPX_ACK(USB_EP2))){}; // wait for ACK sent to host    
            USBINT->INT_CLR = MASK_EPX_ACK(USB_EP2); 
			if( USBCTRL->EPxCSR[2] & ( 1<< 19) )//Toggle error 
			{
				USBCTRL->EPxCSR[2] ^= (1<<17); //out toggle want
				USBCTRL->EPxCSR[2]|= (1<<18); //update want toggle; 
				USBCTRL->EPxCSR[2] |= 1<<11;   //set rx ready, wait for a new packet 
                USBINT->INT_CLR = USB_EP2_OUT_PACKET | USB_EP2_IN; 
                return;    				
			}		
							
			flag_usb_ep2_int = USB_EP2_OUT_PACKET; 
			// clear in token flag, for Interrupt tranfer type, in token flag may set after sending response    
		    USBINT->INT_CLR = USB_EP2_OUT_PACKET | USB_EP2_IN;  
		}  
	} 
	if(!flag_usb_ep3_int)
	{
		if(temp &USB_EP3_OUT_PACKET)	//EP3
		{   
			while(!(USBINT->INT_STAT_RAW & MASK_EPX_ACK(USB_EP3))){}; // wait for ACK sent to host    
            USBINT->INT_CLR = MASK_EPX_ACK(USB_EP3); 
			if( USBCTRL->EPxCSR[3] & ( 1<< 19) )//Toggle error 
			{
				USBCTRL->EPxCSR[3] ^= (1<<17); //out toggle want
				USBCTRL->EPxCSR[3] |= (1<<18); //update want toggle; 
				USBCTRL->EPxCSR[3] |= 1<<11;   //set rx ready, wait for a new packet 
                USBINT->INT_CLR = USB_EP3_OUT_PACKET | USB_EP3_IN; 
                return;    				
			}		
				
			flag_usb_ep3_int = USB_EP3_OUT_PACKET; 
			// clear in token flag, for Interrupt tranfer type, in token flag may set after sending response    
		    USBINT->INT_CLR = USB_EP3_OUT_PACKET | USB_EP3_IN;
		}  
	}      		
	if(!flag_usb_ep4_int)
	{
		if(temp &USB_EP4_OUT_PACKET)	//EP4
		{   
			while(!(USBINT->INT_STAT_RAW & MASK_EPX_ACK(USB_EP4))){}; // wait for ACK sent to host    
            USBINT->INT_CLR = MASK_EPX_ACK(USB_EP4); 
			if( USBCTRL->EPxCSR[4] & ( 1<< 19) )//Toggle error 
			{
				USBCTRL->EPxCSR[4] ^= (1<<17); //out toggle want
				USBCTRL->EPxCSR[4] |= (1<<18); //update want toggle; 
				USBCTRL->EPxCSR[4] |= 1<<11;   //set rx ready, wait for a new packet 
                USBINT->INT_CLR = USB_EP4_OUT_PACKET | USB_EP4_IN; 
                return;    				
			}		
				
			flag_usb_ep4_int = USB_EP4_OUT_PACKET; 
			// clear in token flag, for Interrupt tranfer type, in token flag may set after sending response     
		    USBINT->INT_CLR = USB_EP4_OUT_PACKET | USB_EP4_IN;   
		}  
	}  

	USB_Monitor(); 
}  


/*----------------- 标准请求--------------------*/
void usb_get_status(void)
{  
	uint8_t recipient=0;
	uint16_t status=0;
	uint8_t bEpIndex=0;
	uint8_t bEpDir=0;
	uint16_t ep_status=0;

	printfS("get_status!\n");

	recipient = dev_req.bmRequestType & USB_RECIPIENT_MASK;
	if(recipient==USB_RECIPIENT_DEVICE)
	{
		HAL_FSUSB_Send_Data((uint8_t *)(&usb_device_status),2,USB_EP0);	   
	}
	else if(recipient==USB_RECIPIENT_INTERFACE)
   	{
		HAL_FSUSB_Send_Data((uint8_t *)(&status),2,USB_EP0);   
   	}
   	else if(recipient==USB_RECIPIENT_ENDPOINT)
   	{
		bEpIndex = dev_req.wIndex & 0x0f;
		bEpDir = dev_req.wIndex & 0x80;
		ep_status=HAL_USB_Get_Stall_Status(bEpIndex, bEpDir);   

		HAL_FSUSB_Send_Data((uint8_t *)(&ep_status),2,USB_EP0);	   	
   	}
   	else
      	HAL_FSUSB_EP0_Send_Stall();	
}


void usb_clear_feature(void)
{ 
	uint8_t recipient=0;
	uint8_t bEpIndex=0;
	uint8_t bEpDir=0;

	printfS("clear_feature!\n");   

	recipient = dev_req.bmRequestType & USB_RECIPIENT_MASK;
	if(recipient==USB_RECIPIENT_DEVICE)
	{
		if(dev_req.wValue==USB_FEATURE_REMOTE_WAKEUP)
		{
			usb_device_status &= (~USB_GETSTATUS_REMOTE_WAKEUP);
		}
		HAL_FSUSB_EP0_Send_Empty_Packet();  
	}
   	else if(recipient==USB_RECIPIENT_ENDPOINT)
   	{
		bEpIndex = dev_req.wIndex & 0x0f;
		bEpDir = dev_req.wIndex & 0x80;
    	usb_clear_stall(bEpIndex, bEpDir);
		HAL_FSUSB_EP0_Send_Empty_Packet();    

//		flag_clear_stall=1;
   	}
   	else
		HAL_FSUSB_EP0_Send_Stall();
}


void usb_set_feature(void)
{
	uint8_t recipient=0;
	uint8_t bEpIndex=0;
	uint8_t bEpDir=0;

	printfS("set_feature!\n");
   
	recipient = dev_req.bmRequestType & USB_RECIPIENT_MASK;
	if(recipient==USB_RECIPIENT_DEVICE)
	{
		if(dev_req.wValue==USB_FEATURE_REMOTE_WAKEUP)
		{  
       		usb_device_status |= USB_GETSTATUS_REMOTE_WAKEUP;
		}
		HAL_FSUSB_EP0_Send_Empty_Packet();
	}
	else if(recipient==USB_RECIPIENT_ENDPOINT)
	{
		bEpIndex = dev_req.wIndex & 0x0f;
		bEpDir = dev_req.wIndex & 0x80;
    	usb_send_stall(bEpIndex, bEpDir);
		HAL_FSUSB_EP0_Send_Empty_Packet();  
   	}
   	else
      	HAL_FSUSB_EP0_Send_Stall();
}


void usb_set_address(void)
{
	printfS("set_address,addr=0x%x\n",dev_req.wValue);
	printfS("ADDR=0x%x \n", USBCTRL->USB_ADDR);   
}


void usb_get_configuration(void)
{
	printfS("get_configuration!\n");

	HAL_FSUSB_Send_Data(&config_value,1,USB_EP0);            
}

void usb_set_configuration(void)
{
	printfS("set_configuration!\n");
	
	if( (dev_req.wValue==0)||(dev_req.wValue==1) )
   	{
	  	config_value=dev_req.wValue;
		HAL_FSUSB_EP0_Send_Empty_Packet();    
   	}
   	else
	  	HAL_FSUSB_EP0_Send_Stall();	
}

void usb_get_interface(void)
{
	uint8_t inter_value=0;
	uint8_t recipient=0;

	printfS("get_interface!\n");

	recipient = dev_req.bmRequestType & USB_RECIPIENT_MASK;
	if(recipient==USB_RECIPIENT_INTERFACE)
	{
		inter_value=AltSetting+dev_req.wIndex;	 
		HAL_FSUSB_Send_Data(&inter_value,1,USB_EP0);     
	}
	else	
		HAL_FSUSB_EP0_Send_Stall();

}

    
void usb_set_interface(void)
{
	uint8_t  alt,inter_num;
	uint8_t recipient=0;

	printfS("set_interface!\n");

	recipient = dev_req.bmRequestType & USB_RECIPIENT_MASK;
	if(recipient==USB_RECIPIENT_INTERFACE)
	{
		inter_num=usb_descriptor_nconfig.inter1_descr.bInterfaceNumber; //bInterfaceNumber
		alt=usb_descriptor_nconfig.inter1_descr.bAlternateSetting; //bAlternateSetting	
	
		if( (dev_req.wIndex==inter_num)&&(dev_req.wValue==alt))
		{
		 	AltSetting=alt;	
			HAL_FSUSB_EP0_Send_Empty_Packet();   
		}
		else
			HAL_FSUSB_EP0_Send_Stall();

	}
	else	
		HAL_FSUSB_EP0_Send_Stall();
    
}

  
void usb_get_description(void)
{
	uint16_t wValue;
	uint16_t wLength;
	uint8_t wValue_bIndex;

	printfS("get_description->\n");

	wValue = (dev_req.wValue>>8);
	wValue_bIndex = dev_req.wValue & 0xff;
	wLength = dev_req.wLength; 
   
	if(wValue == USB_DEVICE_DESCRIPTOR_TYPE)
   	{
	    printfS("DEV_DESCRIPTOR!\n");
      
	  	if(wLength >device_descr.bLength)
	  	{
			wLength = device_descr.bLength;
	  	}
		HAL_FSUSB_Send_Data((uint8_t *)(&device_descr),wLength,USB_EP0);	  
   	}
   	else if(wValue == USB_CONFIGURATION_DESCRIPTOR_TYPE)
   	{
	    printfS("CONFIG_DESCRIPTOR!\n");
      
      	if( wLength > sizeof(usb_descriptor_nconfig) )
	  	{
		 	wLength = sizeof(usb_descriptor_nconfig);
	  	}
		HAL_FSUSB_Send_Data( (uint8_t *)(&usb_descriptor_nconfig),wLength, USB_EP0 );		
   	}

	else if (wValue == USB_STRING_DESCRIPTOR_TYPE)
	{    
		if(wValue_bIndex == INDEX_LANGUAGE_ID)       //LANGID string 
		{
			printfS("STRING_DESCRIPTOR__LANGID!\n");

		   	if(wLength>StrDesc_LanguageID[0]) 
		   	{
		    	wLength = StrDesc_LanguageID[0];
		   	}
			HAL_FSUSB_Send_Data((uint8_t *)StrDesc_LanguageID,wLength,USB_EP0);	  
		}
		else if(wValue_bIndex == INDEX_MANUFACTURER) //vender string
		{
			printfS("STRING_DESCRIPTOR__vender!\n");
		   
		   	if(wLength>StrDesc_Manufacturer[0]) 
		   	{
		      	wLength = StrDesc_Manufacturer[0];
		   	}
			HAL_FSUSB_Send_Data((uint8_t *)StrDesc_Manufacturer,wLength,USB_EP0);	  	
		}
		else if(wValue_bIndex == INDEX_PRODUCT)      //product string
		{
	        printfS("STRING_DESCRIPTOR__product!\n");
		   
 		   	if(wLength>StrDesc_Product[0]) 
		   	{
		      	wLength = StrDesc_Product[0];
		   	}
			HAL_FSUSB_Send_Data((uint8_t *)StrDesc_Product,wLength,USB_EP0);
		}
		else if(wValue_bIndex == INDEX_SERIALNUMBER) //SerialNumber	string
		{
         	printfS("STRING_DESCRIPTOR__SerialNumber!\n");
		   
 		   	if(wLength>StrDesc_SerialNumber[0]) 
		   	{
		      	wLength = StrDesc_SerialNumber[0];
		   	}
			HAL_FSUSB_Send_Data((uint8_t *)StrDesc_SerialNumber,wLength,USB_EP0);
		}
		else
		{		
			HAL_FSUSB_EP0_Send_Stall();
		}
	}
	else
	{
		HAL_FSUSB_EP0_Send_Stall();   
	}
}

 
void reserved(void)
{
	printfS("reserved!\n");
    
	HAL_FSUSB_EP0_Send_Stall(); 
}

/*************************************************************************
 * USB标准设备请求入口地址指针表	
 * 请求代码值由4个bit位组成，共16种可能，所以有16个元素
 *************************************************************************/
void (*StandardDeviceRequest[])(void) =
{
	usb_get_status,	        //0x00
	usb_clear_feature,	    //0x01
	reserved,
	usb_set_feature,	    //0x03
	reserved,
	usb_set_address,        //0x05
	usb_get_description,	//0x06
	reserved,
	usb_get_configuration,	//0x08
	usb_set_configuration,	//0x09
	usb_get_interface,		//0x0a
	usb_set_interface,		//0x0b
	reserved,
	reserved,
	reserved,
	reserved
};


void usb_control_transfer(void)
{		
	uint8_t type,req;  

	dev_req.bmRequestType =	USBCTRL->SETIP_0_3_DATA &0xff;	
	dev_req.bRequest = (USBCTRL->SETIP_0_3_DATA>>8)&0xff;
	dev_req.wValue   = (USBCTRL->SETIP_0_3_DATA>>16)&0xffff;
	dev_req.wIndex   = USBCTRL->SETIP_4_7_DATA&0xffff;
	dev_req.wLength  = (USBCTRL->SETIP_4_7_DATA>>16)&0xffff;   

	type = dev_req.bmRequestType & USB_REQUEST_TYPE_MASK;
	req  = dev_req.bRequest&USB_REQUEST_MASK;	
	if (type == USB_STANDARD_REQUEST)		//标准请求
	{
		printfS("standard request-->");
	
		(*StandardDeviceRequest[req])();   
	}
//	else if(type == USB_CLASS_REQUEST)	    //类请求
//	{
//		printfS("class request-->");
	
//		if(dev_req.bRequest&0xF0)  			//UMS CLASS
//		{
//		   	if(dev_req.bRequest == GET_MAX_LUN)  //Get Max LUN
//		   	{
//		      	usb_get_MaxLun();
//		   	}
//		   	else if( dev_req.bRequest == Bulk_Only_Mass_Storage_Reset)
//		   	{								    //Bulk-Only Mass Storage Reset
//		   	  	usb_mass_storage_reset();
//		   	} 
//		   	else
//				HAL_FSUSB_EP0_Send_Stall();  	
//		}
//	}
	else
    {
		HAL_FSUSB_EP0_Send_Stall();  
    }
				
}

void USB_Bus_Reset(void)
{
	USBINT->INT_EN |= (1<<1);	//enable suspend interrupt
	USBCTRL->WORKING_MODE &= ~(1<<6);	//Res:transmit

	if(!(USBCTRL->WORKING_MODE&0x08)) 	  
	{
		USBCTRL->WORKING_MODE  |=1<<2;
		USBCTRL->WORKING_MODE  &=~(1<<2);  
	}	

	USBCTRL->EPxCSR[1] = 0x02100;			 //clear in/out toggle,stall,stall status
	USBCTRL->EPxCSR[1]  |= (1<<18)|(1<<15); //enable change
	USBCTRL->EPxCSR[2]  = 0x02100;	
	USBCTRL->EPxCSR[2] |= (1<<18)|(1<<15);
	USBCTRL->EPxCSR[3]  = 0x02100;
	USBCTRL->EPxCSR[3] |= (1<<18)|(1<<15);
	USBCTRL->EPxCSR[4] = 0x02100;
	USBCTRL->EPxCSR[4] |= (1<<18)|(1<<15);   
}

void USB_Suspend(void) 
{	
	USBINT->INT_EN &= ~(1<<1);	//disable suspend interrupt
	
	USBCTRL->WORKING_MODE |= (1<<6)|(1<<4);	//Res:idle

	printfS("WORKMODE=0x%x \n", USBCTRL->WORKING_MODE);  
		   
	//low power
}

void USB_Resume(void)  
{	
	USBINT->INT_EN |= (1<<1);	//enable suspend interrupt
	
	USBCTRL->WORKING_MODE &= ~(1<<6);	//Res:transmit

	printfS("WORKMODE=0x%x \n", USBCTRL->WORKING_MODE);   
	
}

void USB_Monitor(void)
{   
	if((flag_usb_int & USB_BUS_RESET))	//bus reset 
	{
		flag_usb_int &= (~USB_BUS_RESET);
		USB_Bus_Reset();

		printfS("bus reset \n");    	    	
	}

	if((flag_usb_int & USB_SUSPEND))	//suspend
	{
		flag_usb_int &= (~USB_SUSPEND);
		USB_Suspend();

		printfS("suspend \n");     	    	
	}

	if((flag_usb_int & USB_RESUME))	    //resume
	{
		flag_usb_int &= (~USB_RESUME);
		USB_Resume();  

	    printfS("resume \n");       	    	
	}
	
	if((flag_usb_int & USB_EP0_SETUP_PACKET))	//EP0 setup packet received 
	{
	    printfS("setup packet received !\n");

	    
		flag_usb_int &= (~USB_EP0_SETUP_PACKET);
		usb_control_transfer();

     	printfS("---------control transfer end---------------------\n");		
	}

}


void usb_transfer_monitor(void)
{
	if( flag_usb_ep1_int ==USB_EP1_OUT_PACKET)	//EP1 OUT packet received
	{
	  printfS("EP1 OUT packet received !\n");

	  USBINT->INT_EN &= ~USB_EP1_OUT_PACKET; //disable EP1 out interrupt
		out_ep_index=USB_EP1;
		in_ep_index=USB_EP1;		
		USB_CMD_proc();

		flag_usb_ep1_int=0;	 
		USBINT->INT_EN  |= USB_EP1_OUT_PACKET; //enable EP1 out interrupt

		printfS("---------EP1 transfer end---------------------\n\n");
	}


	if( flag_usb_ep2_int ==USB_EP2_OUT_PACKET)	//EP2 OUT packet received
	{
		printfS("EP2 OUT packet received !\n");

	  USBINT->INT_EN  &= ~USB_EP2_OUT_PACKET; //disable EP2 out interrupt
		out_ep_index=USB_EP2;
		in_ep_index=USB_EP2;
		USB_CMD_proc();

		flag_usb_ep2_int=0;	 
		USBINT->INT_EN  |= USB_EP2_OUT_PACKET; //enable EP2 out interrupt

		printfS("---------EP2 transfer end---------------------\n");
	}
	
	if( flag_usb_ep3_int ==USB_EP3_OUT_PACKET)	//EP3 OUT packet received
	{
		printfS("EP3 OUT packet received !\n");

		USBINT->INT_EN  &= ~USB_EP3_OUT_PACKET; //disable EP3 out interrupt
		out_ep_index=USB_EP3;
		in_ep_index=USB_EP3;
		USB_CMD_proc();

		flag_usb_ep3_int=0;	  
		USBINT->INT_EN  |= USB_EP3_OUT_PACKET; //enable EP3 out interrupt

    printfS("---------EP3 transfer end---------------------\n");
	}

	if( flag_usb_ep4_int ==USB_EP4_OUT_PACKET)	//EP4 OUT packet received
	{
	  printfS("EP4 OUT packet received !\n");

	  USBINT->INT_EN  &= ~USB_EP4_OUT_PACKET; //disable EP4 out interrupt
		out_ep_index=USB_EP4;
		in_ep_index=USB_EP4;		      
		USB_CMD_proc();

		flag_usb_ep4_int=0;	
		USBINT->INT_EN  |= USB_EP4_OUT_PACKET;//enable EP4 out interrupt  

    printfS("---------EP4 transfer end---------------------\n");
	}
}

#endif  
