#include "middle.h"
#include "printer.h"

/*电源键按下/释放中断*/
void GPIOEF_IRQHandler(void)
{
	/*清标志*/
	ClearGPIOITFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);

	PowerPortISR();
}

/*缺纸中断*/
void GPIOAB_IRQHandler(void)
{
	/*清标志*/
	ClearGPIOITFlag(PORT_PAPER_CHECK, PIN_PAPER_CHECK);

	PaperDetectISR();
}


/*电源按键定时器中断*/
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & TIMER_SR_UIF)
	{
		TIM3->SR = 0;
		
		PowerTimerISR();

		return;
	}
}

/*加热定时器中断*/
void TIM6_IRQHandler(void)
{
	if (TIM6->SR & TIMER_SR_UIF)
	{
		TIM6->SR = 0;   //write 0 to clear hardware flag 

		HeatTimerISR();

		return;
	}
}


/*步进电机定时器中断*/
void TIM7_IRQHandler(void)
{
	if (TIM7->SR & TIMER_SR_UIF)
	{
		TIM7->SR = 0;
		
		MotorTimerISR();

		return;
	}
}




