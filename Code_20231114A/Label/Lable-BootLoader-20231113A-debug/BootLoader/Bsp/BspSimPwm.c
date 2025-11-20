#include "includes.h"


/*
**
*/
void BspSimPwmRunEvent(bspSimPwmSt* simPwmS)
{
	u32 __cycle;
	
	if(DELAY_UNREACH == SystemDelay(&simPwmS->osDelayS, simPwmS->runDuty))
	{
		return;
	}

	if(0xFF == simPwmS->times)
	{
		simPwmS->outHigh();
		simPwmS->sta = SIM_PWM_STA_OK;
		return;
	}

//	if(0X00 == simPwmS->times)  // wzh
//	{
//		simPwmS->outLow();
//		simPwmS->sta = SIM_PWM_STA_OK;
//		return;
//	}

//	if(0x00 == simPwmS->times) // wzh
//	{
//		simPwmS->outLow();
//		simPwmS->sta = SIM_PWM_STA_OK;
//		//TRACE("simPwmS is low!\r\n");
//		return;
//	}
	
	//wzh
	if (simPwmS->totleTime >= simPwmS->osDelayS.delays )
	{
	    simPwmS->totleTime =  simPwmS->totleTime - simPwmS->osDelayS.delays ;
	}
	else
	{
		  simPwmS->totleTime = 0;
	}
		
	if ((0x00 == simPwmS->times) || (0 == simPwmS->totleTime))
	{
		simPwmS->outLow();
		simPwmS->sta = SIM_PWM_STA_OK;
		return;
	}
	else
	{
		ClrPowerEnterTime();
	}
	
	__cycle = simPwmS->onMs + simPwmS->offMs;
	
	//simPwmS->totleTime--; //wzh
		
	if(simPwmS->totleTime % __cycle > simPwmS->offMs)
	{
		simPwmS->outHigh();
	}
	else
	{
		simPwmS->outLow();
	}

	if(0xFE == simPwmS->times && simPwmS->totleTime == 0xFD * __cycle)
	{
		simPwmS->totleTime = 0xFE * __cycle;
	}

}	

void BspSimPwmCall(bspSimPwmSt* simPwmS, u32 times, u32 onMs, u32 offMs)
{
	if(SIM_PWM_STA_OK != simPwmS->sta)
	{
		return;
	}
	simPwmS->sta       = SIM_PWM_STA_BUZY;
	simPwmS->times     = times;
	simPwmS->onMs      = onMs ;// / simPwmS->runDuty;  //wzh
	simPwmS->offMs     = offMs ; // / simPwmS->runDuty; //wzh
	simPwmS->totleTime = simPwmS->times * (simPwmS->onMs + simPwmS->offMs);
	simPwmS->osDelayS.ticks = 0; //wzh
}

void BspSimPwmParaReset(bspSimPwmSt* simPwmS)
{
	simPwmS->times     = 0;
	simPwmS->onMs      = 0;
	simPwmS->offMs     = 0;
	simPwmS->totleTime = 0;
	simPwmS->sta       = SIM_PWM_STA_OK;
	//memset(&simPwmS->osDelayS, 0, sizeof(simPwmS->osDelayS));
	simPwmS->outLow();
}


