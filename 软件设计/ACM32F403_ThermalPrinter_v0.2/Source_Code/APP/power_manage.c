#include "power_manage.h"
#include "middle.h"
#include "debug.h"


/*��Դ��λ��*/
volatile uint8_t gu8_PowerBtnPos = POWER_BTN_DOWN;

/*��Դ��/�µ��ʶ������Դ������2s���ϣ�ϵͳ��/�ػ�*/
volatile uint8_t gu8_PowerState = POWER_OFF;

/*��Դ������������⵽�������¿�ʼ����(����systick)�������ͷ�ֹͣ����������*/
volatile uint32_t  gu32_PowerCheckCount = 0;

/*��¼���������������ж�˫���¼�*/
volatile uint8_t gu8_BottonClickCnt = 0;


/*��Դ������/�ͷ��жϷ������*/
void PowerPortISR(void)
{
	/*���ж�*/
	//ClearInterruptFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);
	DisableGPIOIT(PORT_POWER_CHECK, PIN_POWER_CHECK);

	DbgPrintf("PowerPortISR\r\n");

	/*����*/
	StartTimer(TIM_POWER);
}

__STATIC_INLINE void PressPowerButton(void)
{
	uint32_t systick;

	DbgPrintf("PressPowerButton\r\n");

	systick = GetSystickCounter();
	
	/*�ж��ϴε����¼�(�ͷŰ���ʱ��)�ͱ��ε����¼��ļ��*/
	/*����500ms��Ϊ�Ƕ����¼�*/
	if(gu8_BottonClickCnt > 0)
	{
		uint32_t peroid;

		peroid = (systick <= gu32_PowerCheckCount)
				? (0xFFFFFFFFU - gu32_PowerCheckCount + systick)
				: (systick - gu32_PowerCheckCount);

		if(peroid > 500)
			gu8_BottonClickCnt = 0;
	}

	gu32_PowerCheckCount = systick;
}

__STATIC_INLINE void ReleasePowerButton(void)
{
	uint32_t peroid;
	uint32_t systick;

	DbgPrintf("ReleasePowerButton\r\n");

	systick = GetSystickCounter();

	peroid = (systick <= gu32_PowerCheckCount)
				? (0xFFFFFFFFU - gu32_PowerCheckCount + systick)
				: (systick - gu32_PowerCheckCount);

	/*������/�ػ�*/
	if(peroid >= 2800)
	{
		DbgPrintf("1\r\n");
		gu32_PowerCheckCount = 0;
		gu8_PowerState = (gu8_PowerState == POWER_ON) ? POWER_OFF : POWER_ON;
	}
	else if(peroid > 50 && peroid < 500) /*����*/
	{
		DbgPrintf("2\r\n");
		/*���µ�һ��*/
		if(++gu8_BottonClickCnt == 1)
		{
			/*��¼��һ�ΰ���ʱ��ϵͳ�����������ж��´ΰ���ʱ���Ƿ���Ϊ˫��*/
			gu32_PowerCheckCount = systick;
			return;
		}
		
		DbgPrintf("3\r\n");
		/*˫������*/
		gu8_BottonClickCnt = 0;
		gu32_PowerCheckCount = 0;
		//TODO:
		
	}
}

/*��ȡ��Դ����λ��*/
static uint8_t GetPowerBtnPos(void)
{	
	if(GetIOLevel(PORT_POWER_CHECK, PIN_POWER_CHECK) == LEVEL_HIGH)
		return POWER_BTN_UP;
	else
		return POWER_BTN_DOWN;
}

/*��Դ�����������жϷ�����*/
void PowerTimerISR(void)
{
	uint8_t pos;

	/*�򿪵�Դ��ťIO�ж�*/
	ClearGPIOITFlag(PORT_POWER_CHECK, PIN_POWER_CHECK);
	EnableGPIOIT(PORT_POWER_CHECK, PIN_POWER_CHECK);

	pos = GetPowerBtnPos();

	/*��Ч����*/
	if(pos == gu8_PowerBtnPos)
		return;

	gu8_PowerBtnPos = pos;

	/*�������£���ʼ��ʱ(����systick)*/
	if(gu8_PowerBtnPos == POWER_BTN_DOWN)
		PressPowerButton();
	else /*�����ͷ�*/
		ReleasePowerButton();
}

/*ϵͳ��Դ����*/
void SystemPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_POWER_SWITCH, PIN_POWER_SWITCH);
	else
		SetIOLevelLow(PORT_POWER_SWITCH, PIN_POWER_SWITCH);
}

/*����ͷ���ȵ�Դ����*/
void TPHHeatPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_THP_VH, PIN_THP_VH);
	else
		SetIOLevelLow(PORT_THP_VH, PIN_THP_VH);
}

/*����ͷ�߼���Դ����*/
void TPHLogicPowerSwitch(uint8_t state)
{
	if(state == POWER_ON)
		SetIOLevelHigh(PORT_THP_VDD, PIN_THP_VDD);
	else
		SetIOLevelLow(PORT_THP_VDD, PIN_THP_VDD);
}

/*ϵͳ��Դ��ʼ����������Ҫ������3s���ϣ�ϵͳ�ϵ�*/
void PowerManageInit(void)
{
	/*�ȴ�3s���򿪵�Դ*/
	gu32_PowerCheckCount = GetSystickCounter();
	delay_ms(3000);
	SystemPowerSwitch(POWER_ON);
	DbgPrintf("Power on !\r\n");
	/*�ȴ������ͷ�*/
	while(gu8_PowerBtnPos == POWER_BTN_DOWN);
}

