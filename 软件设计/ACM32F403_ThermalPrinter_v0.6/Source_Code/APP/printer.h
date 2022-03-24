#ifndef  __PRINTER__HH_H
#define  __PRINTER__HH_H

#include "ACM32Fxx_HAL.h"

/***********************************MACRO**************************************/

//Dots per line and data length
#define TPH_DOTS_PER_LINE     384
#define TPH_DI_LEN            (TPH_DOTS_PER_LINE >> 3)

//Heating/Latching level and stage(group)
#define TPH_STB_IDLE        0
#define TPH_STB123          1
#define TPH_STB456          2

//Motor
#define PM_DIR_FORWARD    0
#define PM_DIR_REVERSE    1

#define PIN_MOTOR_MASK      (~(PIN_MOTOR_AP | PIN_MOTOR_AN \
	                         | PIN_MOTOR_BP | PIN_MOTOR_BN))

enum 
{
	Step0 = 0,
	Step1 = 1,
	Step2,
	Step3,
	Step4,
	Step5,
	Step6,
	Step7,
	Step8,
	Step9
};


/*****************************Function prototype*******************************/
void HardwareInitialize(void);
void HeatTimerISR(void);
void MotorTimerISR(void);
void PaperDetectISR(void);
void PowerOffISR(void);
bool PaperDetect(void);
void printing_test_flow_16(uint8_t *fu8_Data, uint32_t fu32_Size, uint32_t null);
void printing_test_flow_24(uint8_t *fu8_Data, uint32_t fu32_Size, uint32_t null);
void printing_test_flow_QC(uint8_t *fu8_Data, uint32_t fu32_Size);
void Vomit_Paper(void);


#endif