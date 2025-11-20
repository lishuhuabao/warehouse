#include    <stdio.h>
#include    <stdint.h>
#include "includes.h"

/*
**
*/
UART_HandleTypeDef bspDebugUartHandler;


/********************************************************************************
**
********************************************************************************/
void BspDebugInit(void)
{
	bspDebugUartHandler.Instance 	    = BSP_DEBUG_UART;
	bspDebugUartHandler.Init.BaudRate   = BSP_DEBUG_BAUD_RATE;
	bspDebugUartHandler.Init.WordLength = UART_WORDLENGTH_8B;
	bspDebugUartHandler.Init.StopBits   = UART_STOPBITS_1;
	bspDebugUartHandler.Init.Parity	    = UART_PARITY_NONE;
	bspDebugUartHandler.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	bspDebugUartHandler.Init.Mode	    = UART_MODE_TX;
	HAL_UART_Init(&bspDebugUartHandler);
	
	__HAL_UART_DISABLE_IT(&bspDebugUartHandler, UART_IT_TC); //disable send interrupt;
	__HAL_UART_CLEAR_FLAG(&bspDebugUartHandler, UART_FLAG_TC);
}


/********************************************************************************
**Function: for printf function send 
**Mark    : none 
********************************************************************************/
int fputc(int ch, FILE *f)
{
	BSP_DEBUG_UART->DR = (u08)(ch);
	while((BSP_DEBUG_UART->SR&0X40) == 0)
	{
		;//µÈ´ý·¢ËÍ½áÊø
	}
	
	return ch;
}

/*
**
*/
void BspDebugSleep(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};
	
	HAL_UART_MspDeInit(&bspDebugUartHandler);

	BSP_DEBUG_UART_TX_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin   = BSP_DEBUG_UART_TX_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_DEBUG_UART_TX_PORT, &__gpioInitSt);
}

/*
**
*/
void BspDebugAwake(void)
{
	HAL_UART_MspInit(&bspDebugUartHandler);
}









