#include "printer.h"
#include "middle.h"
#include "debug.h"
#include "power_manage.h"
#include "word_stock.h"

volatile uint32_t gu32_RxCpltStatus = false,gu32_RxCpltStatus2 = false,gu32_RxCpltStatus3 = false;

extern const unsigned char  char_bits[][72];
uint8_t gu8_RxBuffer[4096];

extern T_APDU gApdu;

/* Global Variables */

/*加热阶段(分组)*/
volatile uint8_t gu8_HeatStage = TPH_STB_IDLE;
/*当前步进电机步数(相位)*/
volatile uint8_t gu8_PrtMotorStep = 0;
/*缺纸标志*/
volatile bool gb_OutOfPaper = false;
/*一行打印完毕标志*/
volatile bool gb_OneDotLineOver = true;
/*一次打印的第一行标志*/
volatile bool gb_FirstDotLine = true;
/*打印停止*/
volatile bool gb_PrtMotorStop = true;

/*打印电机正转(逻辑电平控制DRV8833)*/
uint8_t PM_Forward[8] = {PIN_MOTOR_AP | PIN_MOTOR_BN,                //step1
	                     PIN_MOTOR_AP | PIN_MOTOR_AN | PIN_MOTOR_BN, //step2
	                     PIN_MOTOR_AN | PIN_MOTOR_BN,                //step3
	                     PIN_MOTOR_AN | PIN_MOTOR_BP | PIN_MOTOR_BN, //step4
	                     PIN_MOTOR_AN | PIN_MOTOR_BP,                //step5
	                     PIN_MOTOR_AP | PIN_MOTOR_AN | PIN_MOTOR_BP, //step6
	                     PIN_MOTOR_AP | PIN_MOTOR_BP,                //step7
	                     PIN_MOTOR_AP | PIN_MOTOR_BP | PIN_MOTOR_BN  //step8
	                    };

/*打印电机反转(逻辑电平控制DRV8833)*/
uint8_t PM_Reverse[8] = {PIN_MOTOR_AP | PIN_MOTOR_BP | PIN_MOTOR_BN, //step1
	                     PIN_MOTOR_AP | PIN_MOTOR_BP,                //step2
	                     PIN_MOTOR_AP | PIN_MOTOR_AN | PIN_MOTOR_BP, //step3
	                     PIN_MOTOR_AN | PIN_MOTOR_BP,                //step4
	                     PIN_MOTOR_AN | PIN_MOTOR_BP | PIN_MOTOR_BN, //step5
	                     PIN_MOTOR_AN | PIN_MOTOR_BN,                //step6
	                     PIN_MOTOR_AP | PIN_MOTOR_AN | PIN_MOTOR_BN, //step7
	                     PIN_MOTOR_AP | PIN_MOTOR_BN                 //step8
	                    };

static void SetSTBSignal(uint8_t stage, uint8_t signal)
{
	switch(stage)
	{
		case TPH_STB_IDLE:
			SetIOLevelLow(PORT_TPH_STB123, PIN_TPH_STB123);
			SetIOLevelLow(PORT_TPH_STB456, PIN_TPH_STB456);
			break;
		case TPH_STB123:
			if(signal == LEVEL_LOW)
				SetIOLevelLow(PORT_TPH_STB123, PIN_TPH_STB123);
			else
				SetIOLevelHigh(PORT_TPH_STB123, PIN_TPH_STB123);
			break;
		case TPH_STB456:
			if(signal == LEVEL_LOW)
			SetIOLevelLow(PORT_TPH_STB456, PIN_TPH_STB456);
			else
				SetIOLevelHigh(PORT_TPH_STB456, PIN_TPH_STB456);
			break;
		default:
			break;
	}
}

void HeatTimerISR(void)
{
	if(gu8_HeatStage == TPH_STB123)
	{
		/*关闭第一组加热，并启动第二组加热*/
		SetSTBSignal(TPH_STB123, LEVEL_LOW);
		SetSTBSignal(TPH_STB456, LEVEL_HIGH);
		gu8_HeatStage = TPH_STB456;
	}
	else if(gu8_HeatStage == TPH_STB456)
	{
		/*第二组加热完毕，停止加热定时器*/
		SetSTBSignal(TPH_STB456, LEVEL_LOW);
		gu8_HeatStage = TPH_STB_IDLE;
		StopTimer(TIM_HEAT);
	}
}

static void ChangePrtMotorOnePhase(uint8_t dir)
{
	uint8_t val;

	//++gu8_PrtMotorStep;
	//gu8_PrtMotorStep &= 0x07;
	if(gu8_PrtMotorStep > 8) gu8_PrtMotorStep = 1;

	val = (dir == PM_DIR_FORWARD) ? PM_Forward[gu8_PrtMotorStep - 1]
	                              : PM_Reverse[gu8_PrtMotorStep - 1];

	SetIOGroupLevel(PORT_MOTOR, PIN_MOTOR_MASK, val);

	//gu8_PrtMotorStep++;
}

void MotorTimerISR(void)
{
	gu8_PrtMotorStep++;
	
	if(gu8_PrtMotorStep == Step5 || gu8_PrtMotorStep == Step9) // one dot line
	{
		gb_OneDotLineOver = true;
	}

	if(gb_PrtMotorStop && gb_OneDotLineOver)
	{
		StopTimer(TIM_MOTOR);
		SetIOGroupLevel(PORT_MOTOR, PIN_MOTOR_MASK, 0);

		return;
	}

	ChangePrtMotorOnePhase(PM_DIR_FORWARD);
}

/* Out of paper interruption */
void PaperDetectISR(void)
{
	/*因为缺纸后必须关机换纸，关闭中断即可*/
	DisableGPIOIT(PORT_PAPER_CHECK, PIN_PAPER_CHECK);

	gb_OutOfPaper = true;
	
	/* */
	//TODO: check
}

void SetLATSignal(uint8_t signal)
{
	if(signal == LEVEL_LOW)
		SetIOLevelLow(PORT_TPH_LATCH, PIN_TPH_LATCH);
	else
		SetIOLevelHigh(PORT_TPH_LATCH, PIN_TPH_LATCH);
}

bool PaperDetect(void)
{
	return gb_OutOfPaper;
}

void SendOneDotLineData(uint8_t *data)
{
	/* Send one dot line data. */
	TPHSPISend(data, TPH_DI_LEN);

	/* After send one dot line, send LAT signal low pulse.*/
	SetLATSignal(LEVEL_LOW);
	delay(1);
	SetLATSignal(LEVEL_HIGH);
}

void StartPrinting(void)
{
	gb_PrtMotorStop = false;
	gu8_PrtMotorStep = 1;

	SetSTBSignal(TPH_STB_IDLE, 0);
	SetLATSignal(LEVEL_HIGH);
	TPHLogicPowerSwitch(POWER_ON);
	TPHHeatPowerSwitch(POWER_ON);
}

void StopPrinting(void)
{
	gu8_PrtMotorStep = 0;
	gb_PrtMotorStop = true;

	StopTimer(TIM_HEAT);
	TPHHeatPowerSwitch(POWER_OFF);
	SetSTBSignal(TPH_STB_IDLE, 0);
	SetLATSignal(LEVEL_HIGH);
	TPHLogicPowerSwitch(POWER_OFF);
}

/*打印数据。注意：必须是数据长度必须是整行。*/
void Printing(uint8_t *data, uint32_t length)
{
	uint8_t *ptr = data;
	uint32_t offset = 0;
	bool lastLine = false;

	if(length % TPH_DI_LEN != 0)
		return;

	gb_OneDotLineOver = true;
	gb_FirstDotLine = true;

	StartPrinting();

	while(1)
	{
		/*正在加热，等待*/
		if(gu8_HeatStage != TPH_STB_IDLE)
		{
			continue;
		}

		/*数据送入热敏头的缓存器*/
		if(length > offset)
		{
			// Send and latch data 
			SendOneDotLineData(ptr);
			offset += TPH_DI_LEN;
			ptr += TPH_DI_LEN;
		}
		else
		{
			/* last line  */
			gb_PrtMotorStop = true;
		}

		/*如果是第一行数据，开始驱动步进电机走第一步，并启动电机定时器*/
		if(gb_FirstDotLine)
		{
			gb_FirstDotLine = false;
			ChangePrtMotorOnePhase(PM_DIR_FORWARD);
			StartTimer(TIM_MOTOR);
		}

		/*等待步进电机转过上一个点行*/
		while(!gb_OneDotLineOver);

		/*标志新的点行开始*/
		gb_OneDotLineOver = false;

		/*电机转过最后一行，打印结束*/
		if(gb_PrtMotorStop)
		{
			StopPrinting();
			return;
		}

		/*缺纸或者有关闭信号*/
		if(gb_OutOfPaper || gu8_PowerState == POWER_OFF)
		{
			StopPrinting();
			/*  */
			//StartOutOfPaperAlert();

			return;
		}

		/*加热第一个分组，并启动加热计时器。*/
		gu8_HeatStage = TPH_STB123;
		SetSTBSignal(gu8_HeatStage, LEVEL_HIGH);
		StartTimer(TIM_HEAT);
	}
}

void printing_test_flow(uint8_t *fu8_Data, uint32_t fu32_Size)
{
	uint8_t word_len;
	uint32_t i, j, k, l=0;
	uint32_t offset = 0;
	uint8_t  char_bits_16[32];
	word_len = fu32_Size;

	//将收到的数据回发，测试时使用
	//HAL_UART_Transmit(&hDbgUart, gu8_RxBuffer, hDbgUart.lu32_RxCount, 0);
	DbgPrintf("receive Count = %d\r\n", fu32_Size);
	DbgPrintfBuf(fu8_Data, fu32_Size, "receive data:");

	while(word_len)
	{
		if(word_len > 48)
		{
			k = 48;
			word_len -= 48;
		}
		else
		{
			k = word_len;
			word_len	= 0;
		}
		//16*16字库，每个字16行，每行2byte（16bit）
		//打印机一行384bit，一行48个byte，多余的换行
		for(i = 0; i < 32; i += 2)														//打印列数据
		{
			for(j = 0; j < k; )																	//打印行数据
			{
				if(fu8_Data[j+l*48]>0x81)													//中文
				{
					if(j == 47)																			//最后1byte是中文
					{
						gu8_RxBuffer[offset] = 0;
						offset++;
						j++;
					}
					else
					{
						Get_GBK_16(&fu8_Data[j+l*48], char_bits_16);
						gu8_RxBuffer[offset] 	 = char_bits_16[i];
						gu8_RxBuffer[offset+1] = char_bits_16[i+1];
						offset += 2;
						j += 2;
					}
				}
				else																							//英文和数字
				{
					Get_GBK_char_16(&fu8_Data[j+l*48], char_bits_16);
					gu8_RxBuffer[offset] = char_bits_16[i/2];	
					offset++;
					j++;
				}
			}
			for(j = k; j < 48; j++)															//整行补零
			{
				gu8_RxBuffer[offset] = 0;
				offset++;
			}
		}
		Printing(gu8_RxBuffer, offset);
		offset = 0;
		l++;																									//打印新的一行
	}
}

