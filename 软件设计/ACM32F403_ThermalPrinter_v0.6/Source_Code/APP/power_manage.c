#include "power_manage.h"
#include "middle.h"
#include "debug.h"
#include "printer.h"
#include "led.h"

/*电源键位置*/
volatile uint8_t gu8_PowerBtnPos = POWER_BTN_DOWN;

/*电源上/下电标识，当电源键按下2s以上，系统开/关机*/
volatile uint8_t gu8_PowerState = POWER_OFF;

/*电源键计数器，检测到按键按下开始计数(基于systick)；按键释放停止计数并清零*/
volatile uint32_t  gu32_PowerCheckCount = 0;

/*记录单击次数，用于判断双击事件*/
volatile uint8_t gu8_BottonClickCnt = 0;

/*记录是否需要打印Demo*/
volatile uint8_t gu8_PrintDemo = 0;

/*电源键按下/释放中断服务程序*/
void PowerPortISR(void)
{
	/*关中断*/
	//ClearInterruptFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);
	DisableGPIOIT(PORT_POWER_CHECK, PIN_POWER_CHECK);

	DbgPrintf("PowerPortISR\r\n");

	/*防抖*/
	StartTimer(TIM_POWER);
}

/*获取电源开关位置*/
static uint8_t GetPowerBtnPos(void)
{
	if(GetIOLevel(PORT_POWER_CHECK, PIN_POWER_CHECK) == LEVEL_HIGH)
		return POWER_BTN_UP;
	else
		return POWER_BTN_DOWN;
}

__STATIC_INLINE void PressPowerButton(void)
{
	uint32_t systick;
	uint32_t peroid;
	
	DbgPrintf("PressPowerButton\r\n");

	/*按键下电计时 */
	StartTimer(TIM_POWEROFF);
	
	systick = GetSystickCounter();
	
	/*判断上次单击事件(释放按键时刻)和本次单击事件的间隔*/
	/*大于500ms认为是独立事件*/
	if(gu8_BottonClickCnt > 0)
	{
		peroid = (systick <= gu32_PowerCheckCount)
				? (0xFFFFFFFFU - gu32_PowerCheckCount + systick)
				: (systick - gu32_PowerCheckCount);	
		
		if(peroid > 500)
		{
			gu8_BottonClickCnt = 0;		
		}	
	}
	
	gu32_PowerCheckCount = systick;
}

__STATIC_INLINE void ReleasePowerButton(void)
{
	uint32_t peroid;
	uint32_t systick;

	/*按键抬起，关闭下电Timer*/
	StopTimer(TIM_POWEROFF);
	
	systick = GetSystickCounter();

	peroid = (systick <= gu32_PowerCheckCount)
				? (0xFFFFFFFFU - gu32_PowerCheckCount + systick)
				: (systick - gu32_PowerCheckCount);

	/*长按开/关机*/
	if(peroid >= 2800)
	{
		gu32_PowerCheckCount = 0;
		gu8_PowerState = (gu8_PowerState == POWER_ON) ? POWER_OFF : POWER_ON;
	}
	else if(peroid > 50 && peroid < 500) /*单击*/
	{
		/*按下第一次*/
		if(++gu8_BottonClickCnt == 1)
		{
			/*记录第一次按下时的系统计数，用于判断下次按下时，是否视为双击*/
			gu32_PowerCheckCount = systick;
			gu8_PrintDemo = 0;
			return;
		}
		gu8_BottonClickCnt = 0;
		gu32_PowerCheckCount = 0;
		/*双击按键*/
		//TODO:
		gu8_PrintDemo = 1;
	}
}


/*电源按键计数器中断服务函数*/
void PowerTimerISR(void)
{
	uint8_t pos;

	/*打开电源按钮IO中断*/
	ClearGPIOITFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);
	EnableGPIOIT(PORT_POWER_CHECK, PIN_POWER_CHECK);

	pos = GetPowerBtnPos();

	/*无效按键*/
	if(pos == gu8_PowerBtnPos)
		return;

	gu8_PowerBtnPos = pos;

	/*按键按下，开始计时(基于systick)*/
	if(gu8_PowerBtnPos == POWER_BTN_DOWN)
		PressPowerButton();
	else /*按键释放*/
		ReleasePowerButton();
}

/*系统电源开关*/
void SystemPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_POWER_SWITCH, PIN_POWER_SWITCH);
	else
		SetIOLevelLow(PORT_POWER_SWITCH, PIN_POWER_SWITCH);
}

/*热敏头加热电源开关*/
void TPHHeatPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_THP_VH, PIN_THP_VH);
	else
		SetIOLevelLow(PORT_THP_VH, PIN_THP_VH);
}

/*热敏头逻辑电源开关*/
void TPHLogicPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_THP_VDD, PIN_THP_VDD);
	else
		SetIOLevelLow(PORT_THP_VDD, PIN_THP_VDD);
}

/*系统电源初始化函数，主要处理长按3s以上，系统上电*/
void PowerManageInit(void)
{
	/*等待3s，打开电源*/
	gu32_PowerCheckCount = GetSystickCounter();
	delay_ms(3000);
	SystemPowerSwitch(POWER_ON);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); //VDD33 ON
	DbgPrintf("Power on !\r\n");
	
	if(PaperDetect())
	{
	    DbgPrintf("Out of paper !\r\n");
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_CLEAR); 				//VDD33 OFF
      SystemPowerSwitch(POWER_OFF);
	    //StartOutOfPaperAlert();
			LED_OFF(LED_GREEN);
			LED_ON(LED_RED);
	}
	else
	{
			LED_OFF(LED_RED);
			LED_ON(LED_GREEN);
	}
	/*等按键释放*/
	while(gu8_PowerBtnPos == POWER_BTN_DOWN);
}
