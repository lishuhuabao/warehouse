#include "includes.h"

/*
**
*/
ADC_HandleTypeDef adcHandler; 



/*
**
*/
void BspAdcInit(void)
#if 0
{
	adcHandler.Instance					 = ADC1;
	//adcHandler.Init.ClockPrescaler		 = ADC_CLOCK_ASYNC_DIV1;
	adcHandler.Init.ClockPrescaler		     = ADC_CLOCK_ASYNC_DIV1;
	adcHandler.Init.Resolution			 = ADC_RESOLUTION_12B;
	adcHandler.Init.DataAlign 			 = ADC_DATAALIGN_RIGHT;
	adcHandler.Init.ScanConvMode			 = ADC_SCAN_DIRECTION_FORWARD;
	adcHandler.Init.EOCSelection			 = ADC_EOC_SINGLE_CONV;
	adcHandler.Init.LowPowerAutoWait		 = DISABLE;
	adcHandler.Init.LowPowerAutoPowerOff	 = DISABLE;
	adcHandler.Init.ContinuousConvMode	 = DISABLE;
	adcHandler.Init.DiscontinuousConvMode = DISABLE;
	adcHandler.Init.ExternalTrigConv		 = ADC_SOFTWARE_START;
	adcHandler.Init.ExternalTrigConvEdge	 = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adcHandler.Init.DMAContinuousRequests = DISABLE;
	adcHandler.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	if (HAL_ADC_Init(&adcHandler) != HAL_OK)
	{
	  Error_Handler();
	}

	
	HAL_ADCEx_Calibration_Start(&adcHandler);					 //校准ADC
}
#else 
{
	ADC_ChannelConfTypeDef sConfig = {0};

	adcHandler.Instance = ADC1;
	adcHandler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	adcHandler.Init.Resolution = ADC_RESOLUTION_12B;
	adcHandler.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adcHandler.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adcHandler.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	adcHandler.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	adcHandler.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	adcHandler.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	adcHandler.Init.ContinuousConvMode = DISABLE;
	adcHandler.Init.NbrOfConversion = 1;
	adcHandler.Init.DiscontinuousConvMode = DISABLE;
	adcHandler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adcHandler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adcHandler.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&adcHandler) != HAL_OK)
	{
		Error_Handler();
	}


	sConfig.Channel = ADC_CHANNEL_18;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
	if (HAL_ADC_ConfigChannel(&adcHandler, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}
#endif

u16 BspGetAdc(u32 ch)   
{
    ADC_ChannelConfTypeDef __adcChanConf = {0};
	u16 __val;

    __adcChanConf.Channel=ch;                                   //通道
    __adcChanConf.Rank=ADC_REGULAR_RANK_1;                                       //第1个序列，序列1
    __adcChanConf.SamplingTime=ADC_SAMPLETIME_384CYCLES;      //采样时间               
    HAL_ADC_ConfigChannel(&adcHandler,&__adcChanConf);        //通道配置

	
    HAL_ADC_Start(&adcHandler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&adcHandler,100);                //轮询转换

	__val = (u16)HAL_ADC_GetValue(&adcHandler);
	HAL_ADC_Stop(&adcHandler);    
 	
	return (u16)__val;	        	//返回最近一次ADC1规则组的转换结果
}


