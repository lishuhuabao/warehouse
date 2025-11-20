#ifndef _SX1276_TIMER_H_
#define _SX1276_TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef void (*cbTimerFunc)(void);

typedef struct timer_param_s{
    uint8_t     timerNum;    //定时编号
    uint32_t    period;      //定时周期
    bool        oneShot;     //true只执行一次
    bool        start;       //开始启动
    uint32_t    timerTick;   //定时计数
    cbTimerFunc timerCbFunc; //定时回调
    struct timer_param_s *next;
}timer_t;



#ifdef __cplusplus
}
#endif


/*
**
*/
void TimerRegister(uint8_t num, uint32_t period, bool oneShot, cbTimerFunc cbFunc);
void TimerChangePeriod(uint8_t num, uint32_t period);
void TimerStart(uint8_t num);
void TimerStop(uint8_t num);
void TimerReset(uint8_t num);
void TimerLoop(void);

#endif //TIMER_H_

