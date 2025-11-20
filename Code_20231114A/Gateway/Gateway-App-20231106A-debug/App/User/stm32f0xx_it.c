#include "includes.h"

/*
**
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
	  NVIC_SystemReset();
	  TRACE("Reset By HardFault_Handler!\r\n");
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	OsTimeBaseEvent();
}

/*
**
*/
void BSP_PC_UART_IRQ_HANDLER(void)
{
	//static u08 tstdata;
	if (__HAL_UART_GET_FLAG(&bspPcUartHandler, UART_FLAG_RXNE) != RESET ) //if(BSP_PC_UART->ISR&(1<<5))		
	{	
		//tstdata = BSP_PC_UART->RDR;
		//HAL_UART_Transmit(&bspWifiUartHandler, &tstdata, 1, 1000);
		bspPcUartS.rxBuf[bspPcUartS.rxTop++] = BSP_PC_UART->RDR; 
		if(bspPcUartS.rxTop >= sizeof(bspPcUartS.rxBuf))
		{
			bspPcUartS.rxTop = 0;
		}
		bspPcUartS.rxTime = 0; 
		BspLedFlashSet(BSP_LED_TYPE_PC_RXD, 2, 50, 50);
	}
	
	//if (__HAL_UART_GET_FLAG(&bspPcUartHandler, UART_FLAG_IDLE) == SET )
	//{
	//
	//}
	
}

/*
**
*/
void BSP_WIFI_UART_IRQ_HANDLER(void)
{
	//static u08 tstdata;
	if (__HAL_UART_GET_FLAG(&bspWifiUartHandler, UART_FLAG_RXNE) != RESET ) //if(BSP_WIFI_UART->ISR&(1<<5))
	{	 
		//tstdata = BSP_WIFI_UART->RDR;
		//HAL_UART_Transmit(&bspPcUartHandler, &tstdata, 1, 1000);
		bspWifiUartS.rxBuf[bspWifiUartS.rxTop++] = BSP_WIFI_UART->RDR; 
		if(bspWifiUartS.rxTop >= sizeof(bspWifiUartS.rxBuf))
		{
			bspWifiUartS.rxTop = 0;
		}
		bspWifiUartS.rxTime = 0; 
		BspLedFlashSet(BSP_LED_TYPE_WIFI_RXD, 2, 50, 50);
	}
	
	//if (__HAL_UART_GET_FLAG(&bspWifiUartHandler, UART_FLAG_IDLE) == SET )
	//{
	//  TcpClosedFlag = strstr((const char*)bspWifiUartS.rxBuf, "CLOSED\r\n" ) ? 1 : 0;
	//}
	
}

/*
**
*/
void BSP_LORA_DIO0_EXIT_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_LORA_DIO0_PIN);		//
}


/*
**
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	    case BSP_LORA_DIO0_PIN:
			SX1276EventIrqFlag[EVENT_DIO0_IRQ] = 1;
	        break;
	        
		default:
			break;

	}
}


