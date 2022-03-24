#include "middle.h"
#include "printer.h"
#include "debug.h"
#include "led.h"

/*��Դ������/�ͷ��ж�*/
void GPIOEF_IRQHandler(void)
{
	/*���־*/
	ClearGPIOITFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);

	PowerPortISR();
}

/*ȱֽ�ж�*/
void GPIOAB_IRQHandler(void)
{
	/*���־*/
	ClearGPIOITFlag(PORT_PAPER_CHECK, PIN_PAPER_CHECK);
//	DbgPrintf("out of paper int !!! \r\n");
	PaperDetectISR();
}

/*��Դ������ʱ���ж�*/
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & TIMER_SR_UIF)
	{
		TIM3->SR = 0;
		
		PowerTimerISR();

		return;
	}
}

/*���ȶ�ʱ���ж�*/
void TIM6_IRQHandler(void)
{
	if (TIM6->SR & TIMER_SR_UIF)
	{
		TIM6->SR = 0;   //write 0 to clear hardware flag 

		HeatTimerISR();

		return;
	}
}

/*���������ʱ���ж�*/
void TIM7_IRQHandler(void)
{
	if (TIM7->SR & TIMER_SR_UIF)
	{
		TIM7->SR = 0;
		
		MotorTimerISR();

		return;
	}
}

/*��Դ�µ綨ʱ���ж�*/
void TIM2_IRQHandler(void)
{	
	if (TIM2->SR & TIMER_SR_UIF)
	{
		TIM2->SR = 0;

		PowerOffISR();
		return;
	}
}

/*��Դ��ⶨʱ���ж�*/
void TIM4_IRQHandler(void)
{	
	if (TIM4->SR & TIMER_SR_UIF)
	{
		TIM4->SR = 0;
//		DbgPrintf("power detect int !!! \r\n");
		ADC_Start_PowerDetect();
		return;
	}
}
