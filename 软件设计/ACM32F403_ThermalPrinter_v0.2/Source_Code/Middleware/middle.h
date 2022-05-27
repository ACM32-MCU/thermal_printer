#ifndef __MIDDLEWARE__H_H
#define __MIDDLEWARE__H_H

#include "stdint.h"
#include "ACM32Fxx_HAL.h"
#include "power_manage.h"

/*******************************************************************************
*                               �궨��                                            *
*******************************************************************************/
/*��ƽ����*/
#define LEVEL_HIGH               1
#define LEVEL_LOW                0

/*GPIO�˿ڶ���*/
#define PORT_POWER_CHECK         GPIOEF
#define PORT_POWER_SWITCH        GPIOEF
#define PORT_THP_VDD             GPIOAB
#define PORT_THP_VH              GPIOAB

/*���IO���䵽ͬһ��PORT�ϣ�����ͬʱ����*/
#define  PORT_MOTOR              GPIOAB
//#define PORT_MOTOR_AP            GPIOAB
//#define PORT_MOTOR_AN            GPIOAB
//#define PORT_MOTOR_BP            GPIOAB
//#define PORT_MOTOR_BN            GPIOAB

#define PORT_TPH_STB123          GPIOAB
#define PORT_TPH_STB456          GPIOAB

#define PORT_TPH_LATCH           GPIOAB

#define PORT_LED_GREEN           GPIOCD
#define PORT_LED_BLUE            GPIOCD
#define PORT_LED_RED             GPIOCD

#define PORT_PAPER_CHECK         GPIOAB

/*GPIO���Ŷ���*/
#define PIN_POWER_CHECK          (1U << 16)        //PF0
#define PIN_POWER_SWITCH         (1U << (16 + 1))  //PF1
#define PIN_THP_VDD              (1U << (6 + 16))  //PB6
#define PIN_THP_VH               (1U << (9 + 16))  //PB9

#define PIN_MOTOR_AP             (1U << 2)         //PA2
#define PIN_MOTOR_AN             (1U << 3)         //PA3
#define PIN_MOTOR_BP             (1U << 0)         //PA0
#define PIN_MOTOR_BN             (1U << 1)         //PA1

#define PIN_TPH_STB123           (1U << (15 + 16)) //PB15
#define PIN_TPH_STB456           (1U << (12 + 16)) //PB12


#define PIN_TPH_LATCH            (1U << 7)         //PA7

#define PIN_LED_GREEN            (1U << 13)        //PC13
#define PIN_LED_BLUE             (1U << 14)        //PC14
#define PIN_LED_RED              (1U << 15)        //PC15

#define PIN_PAPER_CHECK          (1U << (2 + 16))  //PB2

/*Timer����*/
#define TIM_POWER                TIM3
#define TIM_HEAT                 TIM6
#define TIM_MOTOR                TIM7

/*Timerʱ��ԴƵ��(CK_PSC)*/
#define TIM_CLOCK_FREQ           (10000)

/*��������ʱ��(ms)*/
#define ANTI_SHAKE_DURATION            20
/*�������1��ʱ��*/
#define ONE_STEP_DURATION              2
/*һ��dot�ļ���ʱ��*/
#define DOT_GROUP_HEAT_DURATION        2


/*******************************************************************************
*                               ȫ�ֱ���                                           *
*******************************************************************************/
extern volatile uint32_t gu32_SystemCount;
extern uint32_t gu32_AdcBuffer[21];

/*******************************************************************************
*                               �����ӿ�                                           *
*******************************************************************************/

/*������ʱ��*/
#define StartTimer(tim)                    SET_BIT(tim->CR1, BIT0)
/*�رն�ʱ��*/
void StopTimer(TIM_TypeDef *tim);

/*���IO�жϱ�־*/
#define ClearGPIOITFlag(port, pin)         SET_BIT(port->IC, pin)
/*ʹ��IO�ж�*/
#define EnableGPIOIT(port, pin)            SET_BIT(port->IEN, pin)
/*�ر�IO�ж�*/
#define DisableGPIOIT(port, pin)           CLEAR_BIT(port->IEN, pin)
/*��ȡIO��ƽ*/
#define GetIOLevel(port, pin)              ((port->IDATA & (pin)) ? 1 : 0)
/*����IO��ƽ*/
#define SetIOLevelHigh(port, pin)          SET_BIT(port->ODATA, pin)
#define SetIOLevelLow(port, pin)           CLEAR_BIT(port->ODATA, pin)

/*��ȡsystick��ǰ����ֵ*/
#define GetSystickCounter()                gu32_SystemCount;

/*��ʱ����*/
#define delay_ms(ms)                       System_Delay_MS(ms)
#define delay(val)                         System_Delay(val)

/*����Maskֵ������PortX��ӦIOֵ*/
void SetIOGroupLevel(GPIO_TypeDef *port, uint32_t mask, uint32_t val);

/*��������ӡͷ��������*/
void TPHSPISend(uint8_t *data, uint32_t len);

/*�ֿ�Flash��д*/
void FontSPISend(uint8_t *data, uint32_t len);
void FontSPIRecv(uint8_t *data, uint32_t len);


#endif
