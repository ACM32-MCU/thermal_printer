#ifndef __MIDDLEWARE__H_H
#define __MIDDLEWARE__H_H

#include "stdint.h"
#include "ACM32Fxx_HAL.h"
#include "power_manage.h"

/*******************************************************************************
*                               宏定义                                            *
*******************************************************************************/
/*电平定义*/
#define LEVEL_HIGH               1
#define LEVEL_LOW                0

/*GPIO端口定义*/
#define PORT_POWER_CHECK         GPIOEF
#define PORT_POWER_SWITCH        GPIOEF
#define PORT_THP_VDD             GPIOAB
#define PORT_THP_VH              GPIOAB

/*电机IO分配到同一个PORT上，便于同时换相*/
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

/*GPIO引脚定义*/
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

//#define PIN_LED_GREEN            (1U << 13)        //PC13
#define PIN_LED_RED            		(1U << 14)        //PC14
#define PIN_LED_GREEN             (1U << 15)        //PC15

#define PIN_PAPER_CHECK          (1U << (2 + 16))  //PB2

/*Timer定义*/
#define TIM_POWER                TIM3
#define TIM_POWEROFF             TIM2
#define TIM_POWERDETECT          TIM4
#define TIM_HEAT                 TIM6
#define TIM_MOTOR                TIM7

/*Timer时钟源频率(CK_PSC)*/
#define TIM_CLOCK_FREQ           (10000)

/*按键防抖时间(ms)*/
#define ANTI_SHAKE_DURATION            20
/*步进电机1步时间*/
#define ONE_STEP_DURATION              2
/*一组dot的加热时间*/
#define DOT_GROUP_HEAT_DURATION        2
/*按键下电时间*/
#define POWER_OFF_DURATION        		2000
/*电源检测时间*/
#define POWER_DETECT_DURATION        	1000
/*******************************************************************************
*                               全局变量                                           *
*******************************************************************************/
extern volatile uint32_t gu32_SystemCount;
extern uint32_t gu32_AdcBuffer[21];

/*******************************************************************************
*                               函数接口                                           *
*******************************************************************************/

/*启动定时器*/
#define StartTimer(tim)                    SET_BIT(tim->CR1, BIT0)
/*关闭定时器*/
void StopTimer(TIM_TypeDef *tim);

/*清除IO中断标志*/
#define ClearGPIOITFlag(port, pin)         SET_BIT(port->IC, pin)
/*使能IO中断*/
#define EnableGPIOIT(port, pin)            SET_BIT(port->IEN, pin)
/*关闭IO中断*/
#define DisableGPIOIT(port, pin)           CLEAR_BIT(port->IEN, pin)
/*获取IO电平*/
#define GetIOLevel(port, pin)              ((port->IDATA & (pin)) ? 1 : 0)
/*设置IO电平*/
#define SetIOLevelHigh(port, pin)          SET_BIT(port->ODATA, pin)
#define SetIOLevelLow(port, pin)           CLEAR_BIT(port->ODATA, pin)

/*获取systick当前计数值*/
#define GetSystickCounter()                gu32_SystemCount;

/*延时函数*/
#define delay_ms(ms)                       System_Delay_MS(ms)
#define delay(val)                         System_Delay(val)

/*根据Mask值，设置PortX相应IO值*/
void SetIOGroupLevel(GPIO_TypeDef *port, uint32_t mask, uint32_t val);

/*向热敏打印头发送数据*/
void TPHSPISend(uint8_t *data, uint32_t len);

/*字库Flash读写*/
void FontSPISend(uint8_t *data, uint32_t len);
void FontSPIRecv(uint8_t *data, uint32_t len);

/*ADC测量，电池和热敏*/
void ADC_Start_PowerDetect(void);

#endif
