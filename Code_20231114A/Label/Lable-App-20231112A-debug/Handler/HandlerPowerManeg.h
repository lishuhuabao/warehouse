#ifndef __HANDLER_POWER_MANEG_H_
#define __HANDLER_POWER_MANEG_H_

/*
**
*/
typedef enum{
	SYS_PWR_STA_RUN = 0,
	SYS_PWR_STA_STOP,
}sysPwrStaEnum;

//#define SLEEP_WAIT_TIME       10
#define SLEEP_WAIT_TIME       dataSaveS.waitSleepTime


/*
**
*/
typedef struct{
	u16 time;
	sysPwrStaEnum sta;
}sysPwrSt;

extern sysPwrSt sysPwrS;

/*
**
*/
void EnterLowerPower(void);
void ExitLowerPower(void);
void HandlerPowerEvent(void);
void ClrPowerEnterTime(void);

#endif


