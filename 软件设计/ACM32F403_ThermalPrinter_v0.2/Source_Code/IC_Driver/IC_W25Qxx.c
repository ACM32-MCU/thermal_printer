/*
  ******************************************************************************
  * @file    IC_W25Qxx.c
  * @version V1.0.0
  * @date    2020
  * @brief   W25Qxx IC driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the spi norflash driver for W25Qxx.
  *           @ Initialization and de-initialization functions
  *           @ IO operation functions
  *           @ Peripheral Control functions
  ******************************************************************************
*/

#include "IC_W25Qxx.h"

SPI_HandleTypeDef SPI_Handle;

/*********************************************************************************
* function    :  IC_W25Qxx_WriteEnable
* Description :  Write Enable
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_WriteEnable(void)
{
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = WRITE_ENABLE;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
}

/*********************************************************************************
* function   :  IC_W25Qxx_WriteDisable
* Description :  Write Disable
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_WriteDisable(void)
{
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = WRITE_DISABLE;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
}

/*********************************************************************************
* function   :  IC_W25Qxx_WriteDisable
* Description :  Write Disable
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_WriteRegister(uint8_t fu8_Register_S7_S0, uint8_t fu8_Register_S15_S08)
{
    uint8_t lu8_DataBuffer[3];

    lu8_DataBuffer[0] = WRITE_STATUS_REGISTER;
    lu8_DataBuffer[1] = fu8_Register_S7_S0;
    lu8_DataBuffer[2] = fu8_Register_S15_S08;

    /* Write Enable */
    IC_W25Qxx_WriteEnable();

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 3);
    /* CS Realse */
    __SPI_CS_Release();

    /* Wait Write register End */
    IC_W25Qxx_WaitBusy();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_ID
* Description :  Read Manufacture ID and Device ID
* Input       :  
* Output      :  Manufacture ID and Device ID
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
uint16_t IC_W25Qxx_Read_ID(void)
{
    uint8_t lu8_DataBuffer[4];
    
    lu8_DataBuffer[0] = READ_DEVICE_ID;
    lu8_DataBuffer[1] = 0;
    lu8_DataBuffer[2] = 0;
    lu8_DataBuffer[3] = 0;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 4);
    /* Recieve Manufacture ID and Device ID */
    __SPI_Read_Data(lu8_DataBuffer, 2);
    /* CS Realse */
    __SPI_CS_Release();
    
    return ((uint16_t)lu8_DataBuffer[0] << 8 | (uint16_t)lu8_DataBuffer[1]);
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_RegisterS07_S00
* Description :  Read Status Register S07 ~ S00
* Input       :  
* Output      :  Status Register S07 ~ S00
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
uint8_t IC_W25Qxx_Read_RegisterS07_S00(void)
{
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = READ_STATUS_REGISTER_S07_S00;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* Recieve Status Register S07 ~ S00 */
    __SPI_Read_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
    
    return lu8_DataBuffer[0];
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_RegisterS15_S08
* Description :  Read Status Register S15 ~ S08
* Input       :  
* Output      :  Status Register S15 ~ S08
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
uint8_t IC_W25Qxx_Read_RegisterS15_S08(void)
{
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = READ_STATUS_REGISTER_S15_S08;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* Recieve Status Register S15 ~ S08 */
    __SPI_Read_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
    
    return lu8_DataBuffer[0];
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_Data
* Description :  Read Data
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Read_Data(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[4];
    
    lu8_DataBuffer[0] = READ_DATA;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 4);
    /* Recieve Data */
    __SPI_Read_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_Dual_IO
* Description :  Dual Io Fast Read
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Read_Dual_IO(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[4];

    lu8_DataBuffer[0] = DUAL_IO_FAST_READ;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);

    lu8_DataBuffer[0] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);
    lu8_DataBuffer[3] = (0xC8);

    /* Set SPI Work In 2 Wire Mode */
    __SPI_SET_2X_MODE();

    /* Send Address */
    __SPI_Write_Data(lu8_DataBuffer, 4);
    /* Recieve Data */
    __SPI_Read_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();

    /* Set SPI Work In 1 Wire Mode */
    __SPI_SET_1X_MODE();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_Dual_Output
* Description :  Dual Output Fast Read
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Read_Dual_Output(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[4];

    lu8_DataBuffer[0] = DUAL_OUTPUT_FAST_READ;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);
    lu8_DataBuffer[3] = (0x00);
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 5);

    /* Set SPI Work In 2 Wire Mode */
    __SPI_SET_2X_MODE();

    /* Recieve Data */
    __SPI_Read_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();

    /* Set SPI Work In 1 Wire Mode */
    __SPI_SET_1X_MODE();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_Quad_IO
* Description :  Quad Io Fast Read
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Read_Quad_IO(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[6];

    lu8_DataBuffer[0] = QUAD_IO_FAST_READ;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);

    lu8_DataBuffer[0] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);
    lu8_DataBuffer[3] = (0x00);

    lu8_DataBuffer[4] = (0x00);
    lu8_DataBuffer[5] = (0x00);

    /* Set SPI Work In 4 Wire Mode */
    __SPI_SET_4X_MODE();

    /* Send Address */
    __SPI_Write_Data(lu8_DataBuffer, 6);
    /* Recieve Data */
    __SPI_Read_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();

    /* Set SPI Work In 1 Wire Mode */
    __SPI_SET_1X_MODE();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Read_Quad_Output
* Description :  Quad Output Fast Read
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Read_Quad_Output(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[6];

    lu8_DataBuffer[0] = QUAD_OUTPUT_FAST_READ;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);
    lu8_DataBuffer[4] = (0x00);
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 5);

    /* Set SPI Work In 4 Wire Mode */
    __SPI_SET_4X_MODE();

    /* Recieve Data */
    __SPI_Read_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();

    /* Set SPI Work In 1 Wire Mode */
    __SPI_SET_1X_MODE();
}

/*********************************************************************************
* function   :  IC_W25Qxx_PageProgram
* Description :  Page Program
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_PageProgram(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[4];
    
    lu8_DataBuffer[0] = PAGE_PROGARM;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);

    /* Write Enable */
    IC_W25Qxx_WriteEnable();
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 4);
    /* Send Data */
    __SPI_Write_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();
    
    /* Wait Erase End */
    IC_W25Qxx_WaitBusy();
}

/*********************************************************************************
* function   :  IC_W25Qxx_PageProgram_Quad
* Description :  Quad Page Program 
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_PageProgram_Quad(uint8_t *pu8_Buffer, uint32_t fu32_DataAddress, uint32_t fu32_Length)
{
    uint8_t lu8_DataBuffer[4];
    
    lu8_DataBuffer[0] = QUAD_PAGE_PROGRAM;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);

    /* Write Enable */
    IC_W25Qxx_WriteEnable();
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 4);

    /* Set SPI Work In 4 Wire Mode */
    __SPI_SET_4X_MODE();

    /* Send Data */
    __SPI_Write_Data(pu8_Buffer, fu32_Length);
    /* CS Realse */
    __SPI_CS_Release();

    /* Set SPI Work In 1 Wire Mode */
    __SPI_SET_1X_MODE();

    /* Wait Erase End */
    IC_W25Qxx_WaitBusy();
}

/*********************************************************************************
* function   :  IC_W25Qxx_EraseSector
* Description :  Erease The specific Sector
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_EraseSector(uint32_t fu32_DataAddress)
{
    uint8_t lu8_DataBuffer[4];
    
    lu8_DataBuffer[0] = SECTOR_ERASE;
    lu8_DataBuffer[1] = (uint8_t)(fu32_DataAddress >> 16 & 0xFF);
    lu8_DataBuffer[2] = (uint8_t)(fu32_DataAddress >> 8  & 0xFF);
    lu8_DataBuffer[3] = (uint8_t)(fu32_DataAddress >> 0  & 0xFF);

    /* Write Enable */
    IC_W25Qxx_WriteEnable();
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 4);
    /* CS Realse */
    __SPI_CS_Release();
    
    /* Wait Erase End */
    IC_W25Qxx_WaitBusy();
}

/*********************************************************************************
* function   :  IC_W25Qxx_EraseChip
* Description :  Erease The Whole Chip
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_EraseChip(void)
{
    uint8_t lu8_DataBuffer[1];

    lu8_DataBuffer[0] = CHIP_ERASE;

    /* Write Enable */
    IC_W25Qxx_WriteEnable();

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();

    /* Wait Erase End */
    IC_W25Qxx_WaitBusy();
}

/*********************************************************************************
* function   :  IC_W25Qxx_QuadConfig
* Description :  Quad Function Config
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_QuadConfig(bool fb_Config)
{
    if (fb_Config == true) 
    {
        /* Set W25Qxx Quad Enable */
        IC_W25Qxx_WriteRegister(REGISTER_NULL, REGISTER_S15_S08_QE);
    }
    else 
    {
        /* Set W25Qxx Quad Disable */
        IC_W25Qxx_WriteRegister(REGISTER_NULL, REGISTER_NULL);
    }
}

/*********************************************************************************
* function   :  IC_W25Qxx_WaitBusy
* Description :  Wait IC Not Busy
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_WaitBusy(void)
{   
    /* Wait IC Not Busy */
    while(IC_W25Qxx_Read_RegisterS07_S00() & REGISTER_S07_S00_WIP);
}

/*********************************************************************************
* function   :  IC_W25Qxx_PowerDown
* Description :  
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_PowerDown(void)
{ 
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = DEEP_POWER_DOWN;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Wakeup
* Description :  
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Wakeup(void)
{  
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = RELEASE_FORM_DEEP_POWER_DOWN;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
}

/*********************************************************************************
* function   :  IC_W25Qxx_Reset
* Description :  W25Qxx Reset
* Input       :  
* Output      :  
* Author      :  Chris_Kyle                  Data : 2020
**********************************************************************************/
void IC_W25Qxx_Reset(void)
{
    uint8_t lu8_DataBuffer[1];
    
    lu8_DataBuffer[0] = ENABLE_RESET;

    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();


    lu8_DataBuffer[0] = RESET;
    
    /* CS Select */
    __SPI_CS_Select();
    /* Send command */
    __SPI_Write_Data(lu8_DataBuffer, 1);
    /* CS Realse */
    __SPI_CS_Release();
}

