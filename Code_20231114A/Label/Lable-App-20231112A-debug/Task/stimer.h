#ifndef __STIMER_H_
#define __STIMER_H_

#include <stdint.h>

/*
**
*/
typedef enum{
	STIMER_PERIOD = 0x00,
	STIMER_SLEEP, 
}sTimerIdEnum;

/*
**
*/
void SfTimerStart(sTimerIdEnum timerId, u32 time, u16 num);
void SfTimerEvent(void);
void TimerTaskScheduler(void *args);

#endif

