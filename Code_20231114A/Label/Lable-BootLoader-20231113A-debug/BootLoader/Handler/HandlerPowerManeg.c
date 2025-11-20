#include "includes.h"

/*
**
*/
sysPwrSt sysPwrS;

/*
**
*/
void EnterLowerPower(void)
{
	sysPwrS.sta = SYS_PWR_STA_STOP;
	//BspBuzSleep();
	//BspGt21l16S2wSleep();  //wzh
	//BspBatCheckSleep(); //wzh
	//BspEncrySleep(); //wzh
	#if DEBUG_TX_EN  // wzh
	//BspDebugSleep();
	#endif
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/*
**
*/
void ExitLowerPower(void)
{
	//SystemClock_Config();
	//sysPwrS.sta = SYS_PWR_STA_RUN;
	//BspBuzAwake();
	//BspGt21l16S2wAwake(); // wzh
	#if DEBUG_TX_EN  // wzh
	//BspDebugAwake();
	#endif
}

/*
**
*/
void HandlerPowerEvent(void)
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1000))
	{
		return;
	}

	if(sysPwrS.time < SLEEP_WAIT_TIME)
	{
		sysPwrS.time++;
	}
}

/*
**
*/
void ClrPowerEnterTime(void)
{
	sysPwrS.time = 0;
	//sysPwrS.sta = SYS_PWR_STA_RUN;
	//if(SYS_PWR_STA_STOP == sysPwrS.sta)
	{
		//ExitLowerPower();
	}
}

