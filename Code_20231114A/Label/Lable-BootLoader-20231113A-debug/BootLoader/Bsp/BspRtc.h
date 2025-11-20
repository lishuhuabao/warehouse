#ifndef __BSP_RTC_H_
#define __BSP_RTC_H_

/*
**
*/
extern RTC_HandleTypeDef bspRtcHandler;

/*
**
*/
void BspRtcResetWakeUpTime(u32 ms);
void BspRtcInit(void);
void EnterLowerPwrWithWup(u16 ms);
void LowPower_WakeUp(void);


#endif

