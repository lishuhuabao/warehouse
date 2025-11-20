#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include <stdint.h>
//定时任务,目前只有sleep和test两个任务
enum{
	STIMER_Period    = 0x00,
	STIMER_SLEEP  		= 0x01,
	STIMER_REC = 0x02,
};

//定时任务函数
struct stimer_t
{
    uint8_t  id;             //定时任务ID
    uint8_t  flags;          //运行标志
	  uint32_t expires;        //有效时间
		uint32_t expires_t;      //有效时间，用来重装用
		uint16_t runs;
	  void *args;
    void (*timer_cb)(void *args);
};

void spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num);
void spTimerStop(uint8_t timer_id);
void spTimerTickerHandle(void);
void spTimerTaskScheduler(void *args);
void SfTimerEvent(void);

#endif

