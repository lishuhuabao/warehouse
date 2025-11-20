#ifndef _SX1276_TIMER_H_
#define _SX1276_TIMER_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
**
*/
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

/*
**
*/
void TimerRegister(uint8_t num, uint32_t period, bool oneShot, cbTimerFunc cbFunc);
void TimerChangePeriod(uint8_t num, uint32_t period);
void Sx1276TimerStart(uint8_t num);
void TimerStop(uint8_t num);
void TimerReset(uint8_t num);
void Sx1276TimerLoop(void);

#ifdef __cplusplus
}
#endif

#endif //TIMER_H_
