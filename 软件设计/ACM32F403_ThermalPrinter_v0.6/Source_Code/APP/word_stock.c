#include "word_stock.h"
#include "debug.h"
T_APDU gApdu;

GPIO_InitTypeDef GPIO_Handle; 
extern uint8_t gu8_RxBuffer[];
extern UART_HandleTypeDef hDbgUart;
extern const unsigned char  ascii_16[95][16];


/*********************************************************************************
* Function    : ret_xx
* Description : Return status configuration
* Input       : Return len and status
* Outpu       : None
**********************************************************************************/
void ret_xx(UINT16 ret_len,UINT16 ret_code)
{
	gApdu.RetLen=ret_len;
	gApdu.LEN2=ret_len+0x02;
	gApdu.SW1 = (UINT8) (ret_code >> 8);
	gApdu.SW2 = (UINT8) ret_code;
}

/*********************************************************************************
* Function    : Apdu_Send_Xor/Apdu_Receive_Xor
* Description : 
* Input       : None
* Outpu       : None
**********************************************************************************/
void Apdu_Send_Xor(void)
{
	uint16_t i;
	gApdu.XOR=0;
	for (i = 0;i < gApdu.RetLen; i++)
	{
		gApdu.XOR ^= gApdu.ApduData[i];	
	}
	gApdu.XOR ^= gApdu.SW1;
	gApdu.XOR ^= gApdu.SW2;
}

uint8_t Apdu_Receive_Xor(uint8_t *data)
{
	uint16_t i,len=0;
	uint8_t rec_xor=0;
	
	len = (gApdu.LEN1<<8) + gApdu.LEN2;
	
	for (i = 0;i < len; i++)
	{
		rec_xor ^= data[i];	
	}
	if(rec_xor == data[len])
	{
		return 1;
	}
	else
	{
		ret_xx(0,RET_XOR_ERR);
		return 0;
	}
}

/*********************************************************************************
* Function    : SPI_Read_ID_Test
* Description : SPI Read ID
* Input       : None
* Outpu       : None
**********************************************************************************/
void SPI_Read_ID_Test(void)
{
	uint16_t lu16_ID;
	
	/* Read Read Manufacture ID and Device ID */
  lu16_ID = IC_W25Qxx_Read_ID();
	gApdu.ApduData[0] = lu16_ID>>8;
	gApdu.ApduData[1] = lu16_ID;
	ret_xx(2,RET_OK);
}


/*********************************************************************************
* Function    : SPI_Read_Test
* Description : SPI Read
* Input       : None
* Outpu       : None
**********************************************************************************/
void SPI_Read_Test(void)
{
	uint32_t lu32_address;
	uint8_t lu8_len;
	uint32_t lu32_TestMode = 0;
	
	lu32_address = (gApdu.ApduData[0]<<24) + (gApdu.ApduData[1]<<16) + (gApdu.ApduData[2]<<8) + gApdu.ApduData[3];
  lu8_len = gApdu.ApduData[4];
	HAL_SPI_Wire_Config(&SPI3_Handle, SPI_1X_MODE);
  /* Please Select Test Mode */
  lu32_TestMode = TEST_1X_MODE;
	/* Read Data */
	IC_W25Qxx_Read_Data(gApdu.ApduData, lu32_address, lu8_len);
	ret_xx(lu8_len,RET_OK);
}
/*********************************************************************************
* Function    : SPI_Write_Test
* Description : SPI Write
* Input       : None
* Outpu       : None
**********************************************************************************/
void SPI_Write_Test(void)
{
	uint32_t lu32_address,lu32_len;
	uint32_t lu32_TestMode = 0;
	
	lu32_address = (gApdu.ApduData[0]<<24) + (gApdu.ApduData[1]<<16) + (gApdu.ApduData[2]<<8) + gApdu.ApduData[3];
	lu32_len = gApdu.LC - 4;
	HAL_SPI_Wire_Config(&SPI3_Handle, SPI_1X_MODE);
  /* Please Select Test Mode */
  lu32_TestMode = SPI_1X_MODE;
	/* Page Program */
  IC_W25Qxx_PageProgram((UINT8 *)&gApdu.ApduData[4], lu32_address, lu32_len);
	ret_xx(0,RET_OK);
}

/*********************************************************************************
* Function    : SPI_ChipErase_Test
* Description : SPI ChipErase
* Input       : None
* Outpu       : None
**********************************************************************************/
void SPI_ChipErase_Test(void)
{
  /* Erase Chip */
  IC_W25Qxx_EraseChip();
	ret_xx(0,RET_OK);
}

/*********************************************************************************
* Function    : Command_Process_Flow
* Description : 字库下载命令
* Input       : None
* Outpu       : None
**********************************************************************************/
void Command_Process_Flow(void)
{
	if (gApdu.CLA==0x00)
	{
		switch(gApdu.INS)
		{
			case 0xE0:		SPI_Read_ID_Test();			   			break;
			case 0xE1:		SPI_Read_Test();			   				break;
			case 0xE2:		SPI_Write_Test();			   				break;
			case 0xE3:		SPI_ChipErase_Test();			   		break;
			default:			ret_xx(0,RET_INS_ERR);					break;
		}
	}
	else
	{
		ret_xx(0,RET_CLA_ERR);
	}
}

/*********************************************************************************
* Function    : uart_test_flow
* Description : 字库下载流程
* Input       : None
* Outpu       : None
**********************************************************************************/
void uart_test_flow(uint8_t *fu8_Data, uint32_t fu32_Size)
{
	uint32_t i, j;
	uint32_t offset = 0;

	memcpy((UINT8 *)&gApdu.NAD,fu8_Data,fu32_Size);
	
//	if(Apdu_Receive_Xor((UINT8 *)&gApdu.CLA))														//检查XOR值
//	{
		Command_Process_Flow();																						//命令处理
		if(gApdu.NAD == 0x55)																							//NAD变换
		{
				gApdu.NAD = 0xAA;
		}
//	}	
	Apdu_Send_Xor();																										//计算XOR值
	HAL_UART_Transmit(&hDbgUart, (UINT8 *)&gApdu.NAD, 4, 0); 						//发送4字节,NAD/PCB/LEN1/LEN2
	if(gApdu.RetLen)																										//发送数据
	{
			HAL_UART_Transmit(&hDbgUart, (UINT8 *)&gApdu.ApduData[0], gApdu.RetLen, 0);
	}	
	HAL_UART_Transmit(&hDbgUart, (UINT8 *)&gApdu.SW1, 3,0);							//发送状态字节,SW1/SW2/XOR
}


/*********************************************************************************
* Function    : Get_GBK_16
* Description : 查询字库，16*16
* Input       : GBK码和输出数据地址
* Outpu       : None
**********************************************************************************/
 void Get_GBK_16(uint8_t *code, uint8_t *dz_data)
 {
	uint8_t GBKH,GBKL;
 	uint32_t offset;
	uint32_t lu32_TestMode = 0;
	 
	GBKH=*code;
	GBKL=*(code+1);	   // GBKL=*(code+1);

 	if(GBKH>0XFE||GBKH<0X81)return;
 	GBKH-=0x81;
 	GBKL-=0x48;
	offset=((uint32_t)192*GBKH+GBKL)*32;
	 
	HAL_SPI_Wire_Config(&SPI3_Handle, SPI_1X_MODE);
  /* Please Select Test Mode */
  lu32_TestMode = TEST_1X_MODE;
	IC_W25Qxx_Read_Data(dz_data, offset+0x100,32);	 

}

/*********************************************************************************
* Function    : Get_GBK_24
* Description : 查询字库，24*24
* Input       : GBK码和输出数据地址
* Outpu       : 读取的GBK码长度
**********************************************************************************/
 UINT8 Get_GBK_24(uint8_t *code, uint8_t *dz_data)
 {
	uint8_t GBKH,GBKL;
 	uint32_t offset;
	uint32_t lu32_TestMode = 0;
	 
	GBKH=*code;
	GBKL=*(code+1);	   // GBKL=*(code+1);

 	if(GBKH>0X81)																							//中文
	{
		GBKH-=0xA0;
		GBKL-=0xA0;
		offset=((uint32_t)192*(GBKH+31.5)+GBKL)*72;
		 
		HAL_SPI_Wire_Config(&SPI3_Handle, SPI_1X_MODE);
		/* Please Select Test Mode */
		lu32_TestMode = TEST_1X_MODE;
		IC_W25Qxx_Read_Data(dz_data, offset,72);	
		return	2;
	}
	else																											//英文、数字
	{
		offset = (GBKH+0x19C0)*72;
		HAL_SPI_Wire_Config(&SPI3_Handle, SPI_1X_MODE);
		/* Please Select Test Mode */
		lu32_TestMode = TEST_1X_MODE;
		IC_W25Qxx_Read_Data(dz_data, offset,72);
		return	1;
	}
	return	0;
}
 /*********************************************************************************
* Function    : Get_GBK_char_16
* Description : 查询英文和数字，16*16
* Input       : GBK码和输出数据地址
* Outpu       : None
**********************************************************************************/
void Get_GBK_char_16(uint8_t *code, uint8_t *dz_data)
{
	uint8_t GBK,i;
	GBK=*code-' ';
	for (i = 0;i < 16; i++)
	{
		dz_data[i]=ascii_16[GBK][i];
	}
}

