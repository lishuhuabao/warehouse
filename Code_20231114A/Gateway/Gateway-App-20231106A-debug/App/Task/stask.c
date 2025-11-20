#include "includes.h"

/*
**
*/
#define  spTASK_SIZE(array) (sizeof(array)/sizeof(*array))
#define  TASK_INVALID  0xFF

uint16_t oldtime;
uint16_t newtime;
#define TIMEMAX 3600  //分秒的周期



enum State                    //运行状态
{
	silent_state = 0,   //静默状态0
	rec_orgnet_state,   //收到组网状态1
	repeat_orgnet_state,//再组网状态2
	send_orgnet_state,  //发送组网状态3
	wait_around_state,  //空等状态4
	reply_ack_state,    //回复ACK状态5
	up_data_state,      //上传数据状态6
	wait_ack_state,     //等待ACK状态7
	output_state,       //GPRS输出数据状态8
	display_state,      //打印状态9
}state = silent_state;

/**********今天的数据是否上传**********/
enum
{
	no_up = 0,
	has_up,
}	today_flag = no_up;     

/**********上传计时周期是否到达**********/
enum
{
	time_no_arr = 0,
	time_arr,
}	flag_up_time = time_no_arr; 

enum
{
	  UNALIVE      = 0x00, 
	  ALIVE        = 0x01,
    ALWAYS_ALIVE = 0xFF,
};




/************************************************
函数名称 ： _task_soft_timer_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_soft_timer_cb(void *args)
{
	  spTimerTaskScheduler(NULL);
}
/************************************************
函数名称 ： _task_scan_radio_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_scan_radio_cb(void *args)
{
		SX1276Task();
}


#define Send_Length 255
/************************************************
函数名称 ： _task_scan_radio_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_send_data_cb(void *args)
{
	SX1276Send(loraSendPacketS.buf, loraSendPacketS.len);
}
/************************************************
函数名称 ： tasklist
功    能 ： 任务列表
参    数 ： 
返 回 值 ： 
作    者 ： sun
*************************************************/
struct task_t tasklist[] =
{
	 { TASK_SOFT_TIMER,		ALWAYS_ALIVE,	 NULL,	_task_soft_timer_cb 	  }, //时间管理任务，请勿删除
	 { TASK_SCAN_RADIO,		ALWAYS_ALIVE,	 NULL,	_task_scan_radio_cb   	}, //无线Sx1278任务管理
	 { TASK_SEND_DATA,		UNALIVE,	     NULL,	_task_send_data_cb    	}, 
};


/************************************************
函数名称 ： _getTaskNum
功    能 ： 获取指定任务在任务列表中的位置
参    数 ： 任务号
返 回 值 ： 任务Id号
作    者 ： sun
*************************************************/
static uint8_t _getTaskNum (uint8_t task_id)
{
   uint8_t i;
   for (i = 0; i < spTASK_SIZE(tasklist); ++i)
   {
     if(tasklist[i].id == task_id )
		 {
		   return i;
		 }
   }
	 return TASK_INVALID;
}

/************************************************
函数名称 ： spTaskStart
功    能 ： 开始指定任务
参    数 ： 任务号
返 回 值 ： 无
作    者 ： sun
*************************************************/
void spTaskStart(uint8_t task_id)
{
  uint8_t t_id;
	t_id = _getTaskNum(task_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	tasklist[t_id].flags = ALIVE;
}
/************************************************
函数名称 ： spTaskStop
功    能 ： 停止指定任务
参    数 ： 任务号
返 回 值 ： 无
作    者 ： sun
*************************************************/
void spTaskStop(uint8_t task_id)
{
  uint8_t t_id;
	t_id = _getTaskNum(task_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	tasklist[t_id].flags = UNALIVE;
}

/************************************************
函数名称 ： spTaskScheduler
功    能 ： 轮训任务列表中的任务,任务分配器
参    数 ： void
返 回 值 ： void
作    者 ： sun
*************************************************/

void spTaskScheduler(void)
{
	 
	uint8_t i;
	for (i = 0; i < spTASK_SIZE(tasklist); ++i)
	{
		if (tasklist[i].flags != UNALIVE ) 
		{
			if(tasklist[i].flags != ALWAYS_ALIVE ) 
			{
				tasklist[i].flags = UNALIVE;
				tasklist[i].handle(tasklist[i].args);
			}
			else
			{
				tasklist[i].handle(tasklist[i].args);
			}
		}
	}
}


