#include "middle.h"
#include "debug.h"
#include "led.h"
#include "word_stock.h"
#include "printer.h"
#include "ble.h"

SPI_HandleTypeDef SPI2_Handle; //TPH data
SPI_HandleTypeDef SPI3_Handle; //spi flash
TIM_HandleTypeDef TIMx_Handler;
TIM_HandleTypeDef TIM2_Handler;   
ADC_HandleTypeDef ADC_Handle;
EXTI_HandleTypeDef EXTI_Line13_Handle;

__IO uint32_t u32_GroupFlag = 0;
__IO uint32_t u32_AwdFlag = 0;

volatile uint32_t gu32_VrefP = 0; //ADC reference voltage
uint32_t gu32_AdcBuffer[21];


/*******************************************************************************
*                                   GPIO Configuration                         *
*******************************************************************************/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_Handle;

	/***************************************************************************
	 * LED, GPC[15:14]<=>LED[R:G]
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_15 | GPIO_PIN_14;
	GPIO_Handle.Mode	  	= GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull	  	= GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOC, &GPIO_Handle);
	
	/* RTC access enable */
	System_Enable_Disable_RTC_Domain_Access(FUNC_ENABLE);
	
//	__HAL_RTC_PC13_SEL(0);	// GPIO function
	__HAL_RTC_PC14_SEL(0);
	__HAL_RTC_PC15_SEL(0);
//	__HAL_RTC_PC13_DIGIT();
	__HAL_RTC_PC14_DIGIT();
	__HAL_RTC_PC15_DIGIT();
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET); //led green off
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET); //led red off

	/***************************************************************************
	 * DRV8833->motor, GPA[3:2:1:0]<=>DRV8833[BIN1:BIN2:AIN2:AIN1]
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Handle.Mode	  	= GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull	  	= GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);

	/***************************************************************************
	 * Heating, GPB[15]<=>[STB123456]
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_15;
	GPIO_Handle.Mode	  	= GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull	  	= GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);

	/***************************************************************************
	 * Latch, GPA[7]<=>nLATCH                                                   
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_7;
	GPIO_Handle.Mode	  	= GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull	  	= GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);

	/***************************************************************************
	 * Paper detection, GPB[2]<=>PE                                             
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_2;
	GPIO_Handle.Mode	  	= GPIO_MODE_IT_HIGH_LEVEL;
	GPIO_Handle.Pull	  	= GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	
	/***************************************************************************
	 *电源按键检测IO(PF0)，用于检测电源按键的按下与释放。
	 *  case1:开机时，电源键按下超过2s，系统上电；(忽略2s以下所有按键事件)
	 *  case2:关机时，电源键按下超过2s，系统掉电；(忽略2s以下单击按键事件)
	 *  case3:双击按键，打印二维码
	 **************************************************************************/
	GPIO_Handle.Pin       = GPIO_PIN_0;
	GPIO_Handle.Mode      = GPIO_MODE_IT_RISING_FALLING;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOF, &GPIO_Handle);

	/* Clear Pending Interrupt */
	NVIC_ClearPendingIRQ(GPIOEF_IRQ);

	/* Enable External Interrupt */
	NVIC_EnableIRQ(GPIOEF_IRQ);

	/***************************************************************************
	 *电源开关IO(PF1)，根据电源按键检测IO状态，控制电源开关。
	 **************************************************************************/
	GPIO_Handle.Pin       = GPIO_PIN_1;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull      = GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOF, &GPIO_Handle);

	/***************************************************************************
	 *热敏打印头电源开关， GPB[9:6]<=>[VH:VDD]。
	 **************************************************************************/
	GPIO_Handle.Pin       = GPIO_PIN_9 | GPIO_PIN_6;
	GPIO_Handle.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Handle.Pull      = GPIO_NOPULL;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	
	/***************************************************************************
	 * 充电指示, GPB[12]，BAT_CHRG#
	 **************************************************************************/
	GPIO_Handle.Pin 	  	= GPIO_PIN_12;
	GPIO_Handle.Mode	  	= GPIO_MODE_INPUT;
	GPIO_Handle.Pull	  	= GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);

	/***************************************************************************
	 * 唤醒指示, GPC[13]，wake up sleep mode
	 **************************************************************************/
//	GPIO_Handle.Pin       = GPIO_PIN_13;
//  GPIO_Handle.Mode      = GPIO_MODE_IT_FALLING;
//  GPIO_Handle.Pull      = GPIO_PULLUP;
//  GPIO_Handle.Alternate = GPIO_FUNCTION_0;
//  HAL_GPIO_Init(GPIOC, &GPIO_Handle);
  /* RTC access enable */
//  System_Enable_Disable_RTC_Domain_Access(FUNC_ENABLE);          
//  __HAL_RTC_PC13_SEL(0);  // GPIO function   
//  __HAL_RTC_PC13_PULL_UP_ENABLE();
//  __HAL_RTC_PC13_DIGIT(); 
  /* Clear Pending Interrupt */
//  NVIC_ClearPendingIRQ(GPIOCD_IRQn);       
  /* Enable External Interrupt */
//  NVIC_EnableIRQ(GPIOCD_IRQn);	
	
	/* Config EXTI */
/*	System_Module_Enable(EN_EXTI);	
  EXTI_Line13_Handle.u32_Line    = EXTI_LINE_13;
  EXTI_Line13_Handle.u32_Mode    = EXTI_MODE_INTERRUPT;
	EXTI_Line13_Handle.u32_Trigger = EXTI_TRIGGER_FALLING;
	EXTI_Line13_Handle.u32_GPIOSel = EXTI_GPIOC;
  HAL_EXTI_SetConfigLine(&EXTI_Line13_Handle);
  HAL_EXTI_ClearAllPending();

  // Clear flags、Standby Enable 
  PMU->CR1 |= RPMU_CR_STB_EN | RPMU_CR_CWUF | RPMU_CR_CSBF;
  // Wakeup IO Filter Enable 
  PMU->CR1 |= 0x00020000 << 8;
  // Wakeup IO Enable 
  PMU->CR1 |= 0x00020000;
	// PC13 
  PMU->IOCR &= ~0x40;  // must configure PC13 as digital function   
	PMU->CR1 |= RPMU_CR_CWUF; // clear wakeup flag  
*/
}

/*******************************************************************************
*                                   SPI Initialization                         *
*******************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Handle; 
    
    /* SPI1 */
    if (hspi->Instance == SPI1)
    {
    }
    /* SPI2 */
    else if (hspi->Instance == SPI2) 
    {
        /* Enable Clock */
        System_Module_Enable(EN_SPI2);
        System_Module_Enable(EN_GPIOAB);

        /* SPI3 CLK  PortB Pin8 */
        /* SPI3 MOSI PortB Pin7 */
        GPIO_Handle.Pin            = GPIO_PIN_7 | GPIO_PIN_8;
        GPIO_Handle.Mode           = GPIO_MODE_AF_PP;
        GPIO_Handle.Pull           = GPIO_PULLUP;
        GPIO_Handle.Alternate      = GPIO_FUNCTION_4;
        HAL_GPIO_Init(GPIOB, &GPIO_Handle);

        /* Clear Pending Interrupt */
        NVIC_ClearPendingIRQ(SPI2_IRQn);
        
        /* Enable External Interrupt */
        NVIC_EnableIRQ(SPI2_IRQn);
    }
    /* SPI3 */
    else if (hspi->Instance == SPI3)
    {
        /* Enable Clock */
        System_Module_Enable(EN_SPI3);
        System_Module_Enable(EN_GPIOAB);
        
        /* SPI3 CS   PortA Pin15 */
        /* SPI3 CLK  PortB Pin3 */
        /* SPI3 MOSI PortB Pin5 */
        /* SPI3 MISO PortB Pin4 */
        GPIO_Handle.Pin            = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_Handle.Mode           = GPIO_MODE_AF_PP;
        GPIO_Handle.Pull           = GPIO_PULLUP;
        GPIO_Handle.Alternate      = GPIO_FUNCTION_4;
        HAL_GPIO_Init(GPIOB, &GPIO_Handle);
				
				/* CS USE Software Control */
				/*   SPI CS PortA Pin15    */
				GPIO_Handle.Pin            = GPIO_PIN_15;
				GPIO_Handle.Mode           = GPIO_MODE_OUTPUT_PP;
				GPIO_Handle.Pull           = GPIO_PULLUP;
				GPIO_Handle.Alternate      = GPIO_FUNCTION_0;
				HAL_GPIO_Init(GPIOA, &GPIO_Handle);
				/* First Set CS HIGH */
				__SPI_CS_Release(); 
				
				System_Delay(50000);
				
        /* Clear Pending Interrupt */
        NVIC_ClearPendingIRQ(SPI3_IRQn);
        
        /* Enable External Interrupt */
        NVIC_EnableIRQ(SPI3_IRQn);
    }
}

void SPI_Init(void)
{
	/***************************************************************************
	 * SPI2, 向热敏打印头发送数据
	 **************************************************************************/
	SPI2_Handle.Instance                 = SPI2;
	SPI2_Handle.Init.SPI_Mode            = SPI_MODE_MASTER;
	SPI2_Handle.Init.SPI_Work_Mode       = SPI_WORK_MODE_0;
	SPI2_Handle.Init.X_Mode              = SPI_1X_MODE;
	SPI2_Handle.Init.First_Bit           = SPI_FIRSTBIT_MSB;
	SPI2_Handle.Init.BaudRate_Prescaler  = SPI_BAUDRATE_PRESCALER_8;

	HAL_SPI_Init(&SPI2_Handle);

	/***************************************************************************
	 * SPI3, 字库Flash通信
	 **************************************************************************/
	SPI3_Handle.Instance                 = SPI3;
	SPI3_Handle.Init.SPI_Mode            = SPI_MODE_MASTER;
	SPI3_Handle.Init.SPI_Work_Mode       = SPI_WORK_MODE_0;
	SPI3_Handle.Init.X_Mode              = SPI_1X_MODE;
	SPI3_Handle.Init.First_Bit           = SPI_FIRSTBIT_MSB;
	SPI3_Handle.Init.BaudRate_Prescaler  = SPI_BAUDRATE_PRESCALER_32;

	HAL_SPI_Init(&SPI3_Handle);
}

/*******************************************************************************
*                                 Timer Initialization                         *
*******************************************************************************/
uint32_t HAL_TIMER_MSP_Init(TIM_HandleTypeDef * htim)
{
	System_Module_Reset(RST_TIM3);
	System_Module_Enable(EN_TIM3);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);

	System_Module_Reset(RST_TIM2);
	System_Module_Enable(EN_TIM2);
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);

	System_Module_Reset(RST_TIM4);
	System_Module_Enable(EN_TIM4);
	NVIC_ClearPendingIRQ(TIM4_IRQn);
	NVIC_EnableIRQ(TIM4_IRQn);
	
	System_Module_Reset(RST_TIM6);
	System_Module_Enable(EN_TIM6);
	NVIC_ClearPendingIRQ(TIM6_IRQn);
	NVIC_EnableIRQ(TIM6_IRQn);  

	System_Module_Reset(RST_TIM7);
	System_Module_Enable(EN_TIM7);
	NVIC_ClearPendingIRQ(TIM7_IRQn);
	NVIC_EnableIRQ(TIM7_IRQn);  

	return 0;
}

void Timer_Init(void)
{
	uint32_t timer_clock;

	timer_clock = System_Get_APBClock();

	/* if hclk/pclk != 1, then timer clk = pclk * 2 */
	if (System_Get_SystemClock() != System_Get_APBClock())
	{
		timer_clock =  System_Get_APBClock() << 1;
	}

	HAL_TIMER_MSP_Init(&TIMx_Handler);

	/***************************************************************************
	 * Timer3，电源按键定时器
	 **************************************************************************/
	TIMx_Handler.Instance 							= TIM3;
	TIMx_Handler.Init.ARRPreLoadEn 			= TIM_ARR_PRELOAD_ENABLE;
	TIMx_Handler.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	TIMx_Handler.Init.CounterMode 			= TIM_COUNTERMODE_UP;
	TIMx_Handler.Init.RepetitionCounter = 0;
	TIMx_Handler.Init.Prescaler 				= (timer_clock/TIM_CLOCK_FREQ) - 1;
	TIMx_Handler.Init.Period 						= (TIM_CLOCK_FREQ/1000)*ANTI_SHAKE_DURATION - 1;

	HAL_TIMER_Base_Init(&TIMx_Handler);
	/*只有计数器溢出才会产生中断，计数器只运行一次*/
	TIMx_Handler.Instance->CR1 |= (BIT2 | BIT3);
	HAL_TIM_ENABLE_IT(&TIMx_Handler, TIMER_INT_EN_UPD);

	/***************************************************************************
	 * Timer2，电源下电定时器
	 **************************************************************************/
	TIMx_Handler.Instance 							= TIM2;
	TIMx_Handler.Init.ARRPreLoadEn 			= TIM_ARR_PRELOAD_ENABLE;
	TIMx_Handler.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	TIMx_Handler.Init.CounterMode 			= TIM_COUNTERMODE_UP;
	TIMx_Handler.Init.RepetitionCounter = 0;
	TIMx_Handler.Init.Prescaler 				= (timer_clock/TIM_CLOCK_FREQ) - 1;
	TIMx_Handler.Init.Period 						= (TIM_CLOCK_FREQ/1000)*POWER_OFF_DURATION - 1;
	
	HAL_TIMER_Base_Init(&TIMx_Handler);
	TIMx_Handler.Instance->CR1 = BIT2;
	HAL_TIM_ENABLE_IT(&TIMx_Handler, TIMER_INT_EN_UPD);

	/***************************************************************************
	 * Timer6，加热定时器
	 **************************************************************************/
	TIMx_Handler.Instance 							= TIM6;
	TIMx_Handler.Init.ARRPreLoadEn 			= TIM_ARR_PRELOAD_ENABLE;
	TIMx_Handler.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	TIMx_Handler.Init.CounterMode 			= TIM_COUNTERMODE_UP;
	TIMx_Handler.Init.RepetitionCounter = 0;
	TIMx_Handler.Init.Prescaler 				= (timer_clock/TIM_CLOCK_FREQ) - 1;
	TIMx_Handler.Init.Period 						= (TIM_CLOCK_FREQ/1000)*DOT_GROUP_HEAT_DURATION - 1;

	HAL_TIMER_Base_Init(&TIMx_Handler);
	/*只有计数器溢出才会产生中断*/
	TIMx_Handler.Instance->CR1 = BIT2;
	HAL_TIM_ENABLE_IT(&TIMx_Handler, TIMER_INT_EN_UPD);

	/***************************************************************************
	 * Timer7，步进电机定时器
	 **************************************************************************/
	TIMx_Handler.Instance 							= TIM7;
	TIMx_Handler.Init.ARRPreLoadEn 			= TIM_ARR_PRELOAD_ENABLE;
	TIMx_Handler.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	TIMx_Handler.Init.CounterMode 			= TIM_COUNTERMODE_UP;
	TIMx_Handler.Init.RepetitionCounter = 0;
	TIMx_Handler.Init.Prescaler 				= (timer_clock/TIM_CLOCK_FREQ) - 1;
	TIMx_Handler.Init.Period 						= (TIM_CLOCK_FREQ/1000)*ONE_STEP_DURATION - 1;

	HAL_TIMER_Base_Init(&TIMx_Handler);
	/*只有计数器溢出才会产生中断*/
	TIMx_Handler.Instance->CR1 = BIT2;
	HAL_TIM_ENABLE_IT(&TIMx_Handler, TIMER_INT_EN_UPD);

	/***************************************************************************
	 * Timer4，电量检测定时器
	 **************************************************************************/
	TIMx_Handler.Instance 							= TIM4;
	TIMx_Handler.Init.ARRPreLoadEn 			= TIM_ARR_PRELOAD_ENABLE;
	TIMx_Handler.Init.ClockDivision 		= TIM_CLOCKDIVISION_DIV1;
	TIMx_Handler.Init.CounterMode 			= TIM_COUNTERMODE_UP;
	TIMx_Handler.Init.RepetitionCounter = 0;
	TIMx_Handler.Init.Prescaler 				= (timer_clock/TIM_CLOCK_FREQ) - 1;
	TIMx_Handler.Init.Period 						= (TIM_CLOCK_FREQ/1000)*POWER_DETECT_DURATION - 1;

	HAL_TIMER_Base_Init(&TIMx_Handler);
	/*只有计数器溢出才会产生中断*/
	TIMx_Handler.Instance->CR1 = BIT2;
	HAL_TIM_ENABLE_IT(&TIMx_Handler, TIMER_INT_EN_UPD);
}

/*******************************************************************************
*                                  ADC Initialization                          *
*******************************************************************************/
uint32_t ADC_GetVrefP(void)
{
	ADC_ChannelConfTypeDef ADC_ChannelConf;
	uint32_t TrimValue_3v, AdcValue_VrefP, VrefP,temp;

	ADC_Handle.Init.ClockDiv 							= ADC_CLOCK_DIV8;
	ADC_Handle.Init.ConConvMode 					= ADC_CONCONVMODE_DISABLE;
	ADC_Handle.Init.JChannelMode 					= ADC_JCHANNELMODE_DISABLE;
	ADC_Handle.Init.DiffMode 							= ADC_DIFFMODE_DISABLE;
	ADC_Handle.Init.DMAMode 							= ADC_DMAMODE_DISABLE;
	ADC_Handle.Init.OverMode 							= ADC_OVERMODE_DISABLE;
	ADC_Handle.Init.OverSampMode 					= ADC_OVERSAMPMODE_DISABLE;
	ADC_Handle.Init.AnalogWDGEn 					= ADC_ANALOGWDGEN_DISABLE;
	ADC_Handle.Init.ExTrigMode.ExTrigSel 	= ADC_SOFTWARE_START;
	ADC_Handle.Init.ChannelEn 						= ADC_CHANNEL_VBGR_EN;

	ADC_Handle.Instance = ADC;

	HAL_ADC_Init(&ADC_Handle);

	/* The total adc regular channels number */
	ADC_Handle.ChannelNum = 1;

	/* Add adc channels */
	ADC_ChannelConf.Channel 	= ADC_CHANNEL_VBGR;
	ADC_ChannelConf.RjMode 		= 0;
	ADC_ChannelConf.Sq 				= ADC_SEQUENCE_SQ1;
	ADC_ChannelConf.Smp 			= ADC_SMP_CLOCK_320;
	HAL_ADC_ConfigChannel(&ADC_Handle,&ADC_ChannelConf);

	HAL_ADC_Polling(&ADC_Handle, &AdcValue_VrefP, ADC_Handle.ChannelNum, 0);

	DbgPrintf("The adc convert result : Channel 18 = 0x%08x \r\n", AdcValue_VrefP);

	TrimValue_3v = *(volatile uint32_t*)(0x00080240); //Read the 1.2v trim value in 3.0v vrefp.

	DbgPrintf("The adc 1.2v trim value is : 0x%08x \r\n", TrimValue_3v);

	if(((~TrimValue_3v&0xFFFF0000)>>16) == (TrimValue_3v&0x0000FFFF))
	{
		temp = TrimValue_3v & 0xFFF;

		VrefP = (uint32_t)(temp * 3000 / (AdcValue_VrefP & 0xFFF));

		return VrefP;
	}
	else
		return 0;
}

void ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChannelConf;
	ADC_AnalogWDGConfTypeDef ADC_AnalogWDGConf;
	
	gu32_VrefP = ADC_GetVrefP();

	ADC_Handle.Init.ClockDiv     					= ADC_CLOCK_DIV8;
	ADC_Handle.Init.ConConvMode  					= ADC_CONCONVMODE_DISABLE;
	ADC_Handle.Init.JChannelMode 					= ADC_JCHANNELMODE_DISABLE;
	ADC_Handle.Init.DiffMode     					= ADC_DIFFMODE_DISABLE;
	ADC_Handle.Init.DMAMode      					= ADC_DMAMODE_DISABLE;
	ADC_Handle.Init.OverMode     					= ADC_OVERMODE_DISABLE;
	ADC_Handle.Init.OverSampMode 					= ADC_OVERSAMPMODE_DISABLE;
	ADC_Handle.Init.AnalogWDGEn  					= ADC_ANALOGWDGEN_DISABLE;
	ADC_Handle.Init.ExTrigMode.ExTrigSel 	= ADC_SOFTWARE_START;
	ADC_Handle.Init.ChannelEn    					= ADC_CHANNEL_EXT2_EN | ADC_CHANNEL_EXT3_EN;

	ADC_Handle.Instance   = ADC;
		
	ADC_Handle.AdcResults = &gu32_AdcBuffer[0];

	HAL_ADC_Init(&ADC_Handle);

	ADC_Handle.ChannelNum = 2;

	/* ADC Channel EXT2 => THER */
	ADC_ChannelConf.Channel = ADC_CHANNEL_EXT2;
	ADC_ChannelConf.RjMode  = 0;
	ADC_ChannelConf.Sq      = ADC_SEQUENCE_SQ1;
	ADC_ChannelConf.Smp     = ADC_SMP_CLOCK_320;
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_ChannelConf);

	/* ADC Channel EXT3 => BAT */
	ADC_ChannelConf.Channel = ADC_CHANNEL_EXT3;
	ADC_ChannelConf.RjMode  = 0;
	ADC_ChannelConf.Sq      = ADC_SEQUENCE_SQ2;
	ADC_ChannelConf.Smp     = ADC_SMP_CLOCK_320;
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_ChannelConf);

}

void ADC_Start_PowerDetect(void)
{
	uint32_t i, Voltage[2],flag_print;
	gu32_AdcBuffer[0] = 0;
	gu32_AdcBuffer[1] = 0;
	Voltage[0] = 0;
	Voltage[1] = 0;
	if(GetIOLevel(PORT_THP_VH, PIN_THP_VH))
		flag_print = 1;
	else
		flag_print = 0;
	
	if(flag_print == 0)
	{
		TPHHeatPowerSwitch(POWER_ON);	
	}
	HAL_ADC_Polling(&ADC_Handle, gu32_AdcBuffer, ADC_Handle.ChannelNum, 0);
	for (i = 0; i < ADC_Handle.ChannelNum; i++)
	{
//		DbgPrintf("The adc convert result : Channel %d = 0x%08x. ", gu32_AdcBuffer[i]>>16 & 0xFF,gu32_AdcBuffer[i]);
    Voltage[i] = (gu32_AdcBuffer[i]&0xFFF)*gu32_VrefP/4095;
//    DbgPrintf("The Voltage is: %d mV \r\n", Voltage[i]);
	}
	if(flag_print == 0)
	{
		TPHHeatPowerSwitch(POWER_OFF);
	}
	Voltage[1] = Voltage[1]*3/2;

	if(flag_print == 0)																		//非打印期间，检测VCC
	{
		DbgPrintf("VCC: %d mV \r\n", Voltage[1]);
		if(Voltage[1] < 0xc80)															//电池低电压，低于3.2V
		{
			DbgPrintf("Battery low voltage");
			LED_OFF(LED_GREEN);
			LED_ON(LED_RED);
		}
		else
		{
			LED_ON(LED_GREEN);
			LED_OFF(LED_RED);	
		}
	}
	else																									//打印期间，检测THER
	{
		DbgPrintf("THER: %d mV \r\n", Voltage[0]);
		if((Voltage[0] > 0xA28))
		{
			DbgPrintf("THER voltage too high");
			LED_OFF(LED_GREEN);
			LED_ON(LED_RED);
			TPHHeatPowerSwitch(POWER_OFF);
			TPHLogicPowerSwitch(POWER_OFF);
		}
		else
		{
			LED_ON(LED_GREEN);
			LED_OFF(LED_RED);
		}	
	}
}

void HardwareInitialize(void) 
{
	System_Init();
	System_SysTick_Init();
	System_Delay_MS(ANTI_SHAKE_DURATION);
	DebugInit();
	GPIO_Config();
	Timer_Init();
	BleInit();
	SPI_Init();
	PowerManageInit();
//	ADC_Init();
//	StartTimer(TIM_POWERDETECT);
}

void StopTimer(TIM_TypeDef *tim)
{
	tim->CR1 &= ~BIT0;
	tim->EGR = BIT0;     // no UIF generated because URS=1
}

void SetIOGroupLevel(GPIO_TypeDef *port, uint32_t mask, uint32_t val)
{
	port->ODATA = (port->ODATA & mask) | val;
}

void TPHSPISend(uint8_t *data, uint32_t len)
{
	HAL_SPI_Transmit(&SPI2_Handle, data, len, 5000);
}

void FontSPISend(uint8_t *data, uint32_t len)
{
	HAL_SPI_Transmit(&SPI3_Handle, data, len, 5000);
}

void FontSPIRecv(uint8_t *data, uint32_t len)
{
	HAL_SPI_Receive(&SPI3_Handle, data, len, 5000);
}

