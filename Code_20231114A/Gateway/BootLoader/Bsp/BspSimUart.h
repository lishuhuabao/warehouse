#ifndef __BSP_SIM_UART_H_
#define __BSP_SIM_UART_H_

#include "BspPort.h"

/*
**
*/
void BspSimUartInit(void); //模拟UARTG初始化
void BspSimUartSendByte(char dat); //模拟UART发送一个字节


#endif

