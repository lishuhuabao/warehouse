#ifndef _STASK_H_
#define _STASK_H_

#include <stdint.h>
#include "myConfig.h"

/*
**
*/
#define  TASK_INVALID  0xFF

typedef enum{
	  TASK_SOFT_TIMER = 0,  //ticker任务函数
	  TASK_SCAN_RADIO,
	  TASK_START_CAD,
	  TASK_SEND_DATA,
}taskIdEnum;

/*
**
*/
typedef enum{
	UNALIVE      = 0x00, 
	ALIVE_ONCE   = 0x01,
	ALWAYS_ALIVE = 0xFF,
}taskLiveStaEnum;


/*
**
*/
typedef struct{
    taskIdEnum id;     //任务标志编号，如第一步中添TASK_SOFT_TIMER
    taskLiveStaEnum flags;  //是否运行标志， ALWAYS_ALIVE:始终循环，UNALIVE:不运行可通过函数启动
    void *args;     //任务运行参数
    void (*handle)(void *args);//任务运行回调函数
}taskSt;

/*
**
*/
void TaskStart(u08 taskId);
void TaskScheduler(void);

#endif

