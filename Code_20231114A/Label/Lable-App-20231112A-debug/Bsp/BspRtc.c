#include "includes.h"

/*
**
*/
RTC_HandleTypeDef bspRtcHandler;


/*
**
*/
void BspRtcInit(void)
{
	bspRtcHandler.Instance = RTC;
	bspRtcHandler.Init.HourFormat = RTC_HOURFORMAT_24;
	bspRtcHandler.Init.AsynchPrediv = 127;
	bspRtcHandler.Init.SynchPrediv = 255;
	bspRtcHandler.Init.OutPut = RTC_OUTPUT_DISABLE;
	bspRtcHandler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	bspRtcHandler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&bspRtcHandler) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	// 清除唤醒标志

}



/*
**
*/
void BspRtcResetWakeUpTime(u32 ms)
{
	return;
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	
	}

	HAL_RTCEx_DeactivateWakeUpTimer(&bspRtcHandler);

	if(HAL_RTCEx_SetWakeUpTimer_IT(&bspRtcHandler, ms * 2, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		Error_Handler();
	}
}


/*
**
*/
void EnterLowerPwrWithWup(u16 ms)
{
	return;
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	// 清除唤醒标志
//	HAL_RTCEx_SetWakeUpTimer_IT(&bspRtcHandler, (u32)(2.048 * (float)ms), RTC_WAKEUPCLOCK_RTCCLK_DIV16);//设置睡眠时间
	HAL_RTCEx_SetWakeUpTimer_IT(&bspRtcHandler, (u32)(0 * (float)ms), RTC_WAKEUPCLOCK_RTCCLK_DIV16);//设置睡眠时间
	/* Enter Stop Mode */
	if(sysPwrS.time >= SLEEP_WAIT_TIME)
	{
		EnterLowerPower();
    }
}

/*
**
*/
void LowPower_WakeUp(void)
{
	HAL_RTCEx_DeactivateWakeUpTimer(&bspRtcHandler);
    // 如果使用非MSI时钟，请恢复默认时钟
		//	SystemClock_Config(); //wzh

    // 清除唤醒标记 减少重复唤醒电流
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	
	}

	// 清除待机标志
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
	}
	//HAL_UART_MspInit(&bspDebugUartHandler);
	// ExitLowerPower();  //wzh
	
	if (sysPwrS.sta == SYS_PWR_STA_STOP)  //wzh
  {  
	  SystemClock_Config(); 
		sysPwrS.sta = SYS_PWR_STA_RUN;
		ExitLowerPower();
	}
	
}

