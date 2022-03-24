#ifndef __POWER_MANAGE__H_H
#define __POWER_MANAGE__H_H

#include "stdint.h"

/*******************************************************************************
*                               宏定义                                            *
*******************************************************************************/
/*电源按钮位置定义*/
#define POWER_BTN_UP         1
#define POWER_BTN_DOWN       0

/*电源状态定义*/
#define POWER_ON             1
#define POWER_OFF            0

/*******************************************************************************
*                               全局变量                                           *
*******************************************************************************/

extern volatile uint8_t gu8_PowerState;
extern volatile uint8_t gu8_PrintDemo;
//extern volatile uint8_t gu8_PowerBtnPos;


/*******************************************************************************
*                               函数接口                                           *
*******************************************************************************/
void PowerPortISR(void);
void PowerTimerISR(void);
void PowerManageInit(void);
void SystemPowerSwitch(uint8_t state);
void TPHHeatPowerSwitch(uint8_t state);
void TPHLogicPowerSwitch(uint8_t state);

#endif

