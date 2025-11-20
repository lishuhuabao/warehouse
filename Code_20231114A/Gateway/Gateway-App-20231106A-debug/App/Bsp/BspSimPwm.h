#ifndef		__BSP_SIM_PWM_H_
#define		__BSP_SIM_PWM_H_

#include "usrOs.h"

/*
**
*/
typedef enum{
	SIM_PWM_STA_OK = 0,
	SIM_PWM_STA_BUZY
}simPwmStaEnum;

/*
**
*/
typedef struct{
	simPwmStaEnum sta;
	u08 times;
	u16 onMs;
	u16 offMs;
	u16 runDuty;
	u16 totleTime; 
	osDelaySt osDelayS;
	void (*outHigh)(void);
	void (*outLow)(void);
}bspSimPwmSt;

/*
**
*/
void BspSimPwmRunEvent(bspSimPwmSt* simPwmS);
void BspSimPwmCall(bspSimPwmSt* simPwmS, u08 times, u16 onMs, u16 offMs);
void BspSimPwmParaReset(bspSimPwmSt* simPwmS);



#endif


