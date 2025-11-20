#ifndef		__BSP_UART_H_
#define		__BSP_UART_H_

#include "myConfig.h"

/*
**
*/
#define __UART_PACKET_MAX  256

/*
**
*/
typedef struct{
	u08 rxBuf[__UART_PACKET_MAX]; //接收缓存区
	u16 rxTop; //接收缓存区到达位置
	u16 rxBottom;//接收缓存区处理位置
	u08 rxDat; //接收数据
	u16 rxTime;//接收事件间隔
	u08 busyflag; //空闲标志
}uartSt;

typedef	void (*uartRecCallBackFun)(void);

/*
**
*/
void UartRecEvent(uartSt *uart, uartRecCallBackFun func); //UART接收事件

#endif


