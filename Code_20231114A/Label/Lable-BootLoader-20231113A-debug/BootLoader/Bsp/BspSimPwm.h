#ifndef		__BSP_SIM_PWM_H_
#define		__BSP_SIM_PWM_H_

#include "usrOs.h"

/*
**
*/
#define BSP_SIM_PWM_HIGH_FOR_EVER      0xFFFFFFFF


typedef enum{
	SIM_PWM_STA_OK = 0,
	SIM_PWM_STA_BUZY
}simPwmStaEnum;

/*
**
*/
typedef struct{
	simPwmStaEnum sta;
	u32 times;
	u32 onMs;
	u32 offMs;
	u32 runDuty;
	u32 totleTime; 
	osDelaySt osDelayS;
	void (*outHigh)(void);
	void (*outLow)(void);
}bspSimPwmSt;

/*
**
*/
void BspSimPwmRunEvent(bspSimPwmSt* simPwmS);
void BspSimPwmCall(bspSimPwmSt* simPwmS, u32 times, u32 onMs, u32 offMs);
void BspSimPwmParaReset(bspSimPwmSt* simPwmS);



#endif


