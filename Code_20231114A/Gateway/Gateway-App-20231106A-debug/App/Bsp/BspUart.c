#include "includes.h"



/********************************************************************************
**Function:UART接收事件
**Mark    :None
********************************************************************************/
void UartRecEvent(uartSt *uart, uartRecCallBackFun func)
{
	uart->rxTime++;
	
	if(uart->rxTime < 20)
	{
		return;
	}

	if(0 == uart->rxTop)
	{
		return;
	}
	func();
	uart->rxTop = 0;
}


