#include "includes.h"

/*
**
*/
UART_HandleTypeDef bspPcUartHandler;
uartSt bspPcUartS;

/*
**
*/
static void __PcPortInit(void);

/*
**
*/
void BspPcUartSendPacket(u08 *buf, u16 len)
{
	//u16 i;

	//for(i = 0; i < len; i++)
	//{
	//	BSP_PC_UART->TDR = buf[i];
	//	while((BSP_PC_UART->ISR&0X40) == 0)
	//	{
	//		;//等待发送结束
	//	}
	//}

	HAL_UART_Transmit(&bspPcUartHandler, buf, len, 1000); 
	
	//while(__HAL_UART_GET_FLAG(&bspPcUartHandler,UART_FLAG_TC)!=SET);		//等待发送结束
}

/*
**
*/
void BspPcInit(void)
{
	__PcPortInit();

	bspPcUartHandler.Instance 	      = BSP_PC_UART;
	bspPcUartHandler.Init.BaudRate	  = BSP_PC_BAUD_RATE;
	bspPcUartHandler.Init.WordLength  = UART_WORDLENGTH_8B;
	bspPcUartHandler.Init.StopBits	  = UART_STOPBITS_1;
	bspPcUartHandler.Init.Parity	  = UART_PARITY_NONE;
	bspPcUartHandler.Init.HwFlowCtl   = UART_HWCONTROL_NONE;
	bspPcUartHandler.Init.Mode		  = UART_MODE_TX_RX;
	HAL_UART_Init(&bspPcUartHandler);
	
	__HAL_UART_DISABLE_IT(&bspPcUartHandler, UART_IT_TC); //disable send interrupt;
	
	__HAL_UART_ENABLE_IT(&bspPcUartHandler, UART_IT_RXNE );//enable receive interrupt;//| UART_IT_IDLE);//配置了接收中断和总线空闲中断
	
	//HAL_UART_Receive_IT(&bspPcUartHandler, &bspPcUartS.rxDat, 1);
	
	HAL_NVIC_EnableIRQ(BSP_PC_UART_IRQ);		   //enable uart interrupt;
	HAL_NVIC_SetPriority(BSP_PC_UART_IRQ, 0, 0); // Preemption Priority is 0, Response priority is 0;
	
	__HAL_UART_CLEAR_FLAG(&bspPcUartHandler, UART_FLAG_TC);
	
	bspPcUartS.busyflag = false;
}

/*
**
*/
static void __PcPortInit(void)
{

}




