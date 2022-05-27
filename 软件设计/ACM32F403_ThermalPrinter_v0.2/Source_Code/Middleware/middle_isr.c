#include "middle.h"
#include "printer.h"

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




