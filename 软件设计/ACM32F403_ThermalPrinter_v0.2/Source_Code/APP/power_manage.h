#ifndef __POWER_MANAGE__H_H
#define __POWER_MANAGE__H_H

#include "stdint.h"

/*******************************************************************************
*                               �궨��                                            *
*******************************************************************************/
/*��Դ��ťλ�ö���*/
#define POWER_BTN_UP         1
#define POWER_BTN_DOWN       0

/*��Դ״̬����*/
#define POWER_ON             1
#define POWER_OFF            0

/*******************************************************************************
*                               ȫ�ֱ���                                           *
*******************************************************************************/

extern volatile uint8_t gu8_PowerState;
//extern volatile uint8_t gu8_PowerBtnPos;


/*******************************************************************************
*                               �����ӿ�                                           *
*******************************************************************************/
void PowerPortISR(void);
void PowerTimerISR(void);
void PowerManageInit(void);
void SystemPowerSwitch(uint8_t state);
void TPHHeatPowerSwitch(uint8_t state);
void TPHLogicPowerSwitch(uint8_t state);

#endif

