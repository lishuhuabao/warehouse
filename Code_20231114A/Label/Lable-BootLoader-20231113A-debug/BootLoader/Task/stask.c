#include "includes.h"

/*
**
*/
static void __TaskSendDataCb(void *args);
static void __TaskSoftTimerCb(void *args);
static void __TaskScanRadioCb(void *args);
static void __TaskStartCadCb(void *args);
static u08 __GetTaskNum (u08 taskId);


static taskSt taskList[] = {
	 {TASK_SOFT_TIMER,		ALWAYS_ALIVE,	 NULL,	__TaskSoftTimerCb}, //时间管理任务，请勿删除
	 {TASK_SCAN_RADIO,		ALWAYS_ALIVE,	 NULL,	__TaskScanRadioCb}, //无线Sx1278任务管理
	 {TASK_START_CAD,		UNALIVE,	     NULL,	__TaskStartCadCb}, //MCU 唤醒后启动CAD检测任务
	 {TASK_SEND_DATA,		UNALIVE,	     NULL,	__TaskSendDataCb}, 
};


/*
**
*/
static void __TaskSendDataCb(void *args)
{
	SX1276Send(loraSendPacketS.buf, loraSendPacketS.len);
}

/*
**
*/
static void __TaskSoftTimerCb(void *args)
{
	TimerTaskScheduler(NULL);
}

/*
**
*/
static void __TaskScanRadioCb(void *args)
{
	SX1276Task();
}

/*
**
*/
static void __TaskStartCadCb(void *args)
{
	return;
	SX1276StartCad();
}

/*
**
*/
static u08 __GetTaskNum (u08 taskId)
{
	u08 i;

	for (i = 0; i < ARRAY_SIZE(taskList); ++i)
	{
		if(taskList[i].id == taskId)
		{
			return i;
		}
	}
	
	return TASK_INVALID;
}

/*
**
*/
void TaskStart(u08 taskId)
{
	u08 __id;
	
	__id = __GetTaskNum(taskId);
	
	if(TASK_INVALID == __id)
	{
		return;
	}
	taskList[__id].flags = ALIVE_ONCE;
}

/*
**
*/
void TaskScheduler(void)
{
	u08 i;
	
	for (i = 0; i < ARRAY_SIZE(taskList); ++i)
	{
		if (UNALIVE != taskList[i].flags) 
		{
			if(ALWAYS_ALIVE != taskList[i].flags) 
			{
				taskList[i].flags = UNALIVE;
				taskList[i].handle(taskList[i].args);
			}
			else
			{
				taskList[i].handle(taskList[i].args);
			}
		}
	}
}


