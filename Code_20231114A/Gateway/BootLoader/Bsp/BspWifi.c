#include    <stdio.h>
#include    <stdint.h>
#include "includes.h"

/*
**
*/
UART_HandleTypeDef bspWifiUartHandler;
uartSt bspWifiUartS;


/*
**
*/
void BspWifiUartSendPacket(u08 *buf, u16 len)
{
	//u16 i;

	//for(i = 0; i < len; i++)
	//{
	//	BSP_WIFI_UART->TDR = buf[i];
	//	while((BSP_WIFI_UART->ISR&0X40) == 0)
	//	{
	//		;//等待发送结束
	//	}
	//}

	HAL_UART_Transmit(&bspWifiUartHandler, buf, len, 1000); 
	
	//while(__HAL_UART_GET_FLAG(&bspWifiUartHandler,UART_FLAG_TC)!=SET);		//等待发送结束
}

/*
**
*/
void BspWifiInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;
	// UART
	bspWifiUartHandler.Instance 	      = BSP_WIFI_UART;
	bspWifiUartHandler.Init.BaudRate	  = BSP_WIFI_BAUD_RATE;
	bspWifiUartHandler.Init.WordLength  = UART_WORDLENGTH_8B;
	bspWifiUartHandler.Init.StopBits	  = UART_STOPBITS_1;
	bspWifiUartHandler.Init.Parity	    = UART_PARITY_NONE;
	bspWifiUartHandler.Init.HwFlowCtl   = UART_HWCONTROL_NONE;
	bspWifiUartHandler.Init.Mode		    = UART_MODE_TX_RX;
	HAL_UART_Init(&bspWifiUartHandler);
	
	__HAL_UART_DISABLE_IT(&bspWifiUartHandler, UART_IT_TC); //disable send interrupt;
	
	__HAL_UART_ENABLE_IT(&bspWifiUartHandler, UART_IT_RXNE);//enable receive interrupt;//| UART_IT_IDLE);//配置了接收中断和总线空闲中断
		
	HAL_NVIC_EnableIRQ(BSP_WIFI_UART_IRQ);		   //enable uart interrupt;
	HAL_NVIC_SetPriority(BSP_WIFI_UART_IRQ, 0, 0); // Preemption Priority is 0, Response priority is 0;
	
	__HAL_UART_CLEAR_FLAG(&bspWifiUartHandler, UART_FLAG_TC);
	
	// RST
	
	BSP_WIFI_RST_GPIO_CLK_ENABLE();	

	__gpioInitSt.Pin = BSP_WIFI_RST_PIN;
	__gpioInitSt.Mode = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull = GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BSP_WIFI_RST_PORT, &__gpioInitSt);									// 配置RST引脚
	
	//BSP_WIFI_RST_(1);
	bspWifiUartS.busyflag = false;
	wifiCfgS.flag = false;
	
}
