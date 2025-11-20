#include "includes.h"

/*
**
*/
#define TICK_RATE_HZ   1000 //系统运行频率需要配置
#define   __TIMER_ALWAYS_ALIVE      0xFFFF
#define   __TIMER_ALWAYS_UNLIVE     0x0000

#define __CREAT_TIMER(timerId, state, runPeriod,          times,arg, handler) \
                     {timerId, state, runPeriod,runPeriod,  times,arg, handler}

/*
**
*/
typedef struct {
	u08  id;             //定时任务ID
	taskLiveStaEnum  flags;          //运行标志
	u32 runPeriod;        //有效时间
	u32 runPeriodBackup;      //有效时间，用来重装用
	u16 times;
	void *args;
	void (*timerCb)(void *args);
}timerSt;                     

/*
**
*/
static void __TimerSleepCb(void *args);
static sTimerIdEnum __GetTimerTaskNum(u08 timerId);
static void __TimerTickerHandle(void);


static timerSt  sfTimerList[] = {
   __CREAT_TIMER( STIMER_SLEEP,	    UNALIVE,	10,    __TIMER_ALWAYS_UNLIVE, NULL,	__TimerSleepCb),
};

/*
**
*/
static void __TimerSleepCb(void *args)
{
//	SX1276SetSleep( );//没有检测到有效数据Radio休眠
    EnterLowerPwrWithWup(500);
}

/*
**
*/
static sTimerIdEnum __GetTimerTaskNum (u08 timerId)
{
	u08 i;

	for (i = 0; i < ARRAY_SIZE(sfTimerList); ++i)
	{
		if(sfTimerList[i].id == timerId )
		{
			return (sTimerIdEnum)i;
		}
	}
	return (sTimerIdEnum)TASK_INVALID;
}

/*
**
*/
void SfTimerStart(sTimerIdEnum timerId,u32 time,u16 num)
{
	sTimerIdEnum __id;
	
	__id = __GetTimerTaskNum(timerId);
	if(__id == TASK_INVALID )
	{
		return;
	}

	sfTimerList[__id].runPeriod = time;
	sfTimerList[__id].runPeriodBackup = time;
	sfTimerList[__id].times = num;
}

/*
**
*/
static void __TimerTickerHandle(void)
{ 
	 u08 i;
	 
	 for (i = 0; i < ARRAY_SIZE(sfTimerList); ++i)
	 {
		if (sfTimerList[i].times != __TIMER_ALWAYS_UNLIVE)
		{
			if(sfTimerList[i].runPeriod > 1)
			{
				sfTimerList[i].runPeriod--;
			}
			else //sfTimerList[i].runPeriod  == 0
			{
				if(sfTimerList[i].flags == UNALIVE)
				{
					sfTimerList[i].flags = ALIVE_ONCE;
				}
			}
		}
	}
}

/*
**
*/
void TimerTaskScheduler(void *args)
{ 
	u08 i;
	for (i = 0; i < ARRAY_SIZE(sfTimerList); ++i)
	{
		if (sfTimerList[i].flags == ALIVE_ONCE)
		{
			if (sfTimerList[i].times == __TIMER_ALWAYS_UNLIVE)
			{
				sfTimerList[i].flags = UNALIVE;
				sfTimerList[i].runPeriod = 0;
				continue;
			}
			else if (sfTimerList[i].times == __TIMER_ALWAYS_ALIVE)
			{
				sfTimerList[i].runPeriod = sfTimerList[i].runPeriodBackup;
				sfTimerList[i].flags = UNALIVE;
				sfTimerList[i].timerCb(sfTimerList[i].args);
			    
			}
			else
			{
				sfTimerList[i].times --;
				sfTimerList[i].flags = UNALIVE;
				sfTimerList[i].timerCb(sfTimerList[i].args);
				sfTimerList[i].runPeriod = sfTimerList[i].runPeriodBackup;
			}	  
		}		
	}
}

/*
**
*/
void SfTimerEvent(void)
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}	
	__TimerTickerHandle();
	Sx1276TimerLoop();
}



