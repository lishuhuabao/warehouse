#include "includes.h"

/*
**
*/
//volatile uint32_t TickCounter;

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{

	while (1)
	{
		Error_Handler();
	}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
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
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
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
	OsTimeBaseEvent();
	HAL_IncTick();
}

/*
**
*/
void BSP_KEY_UP_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_KEY_UP_PIN);
}


/*
**
*/
void BSP_KEY_DOWN_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_KEY_DOWN_PIN);
}


/*
**
// wzh
void BSP_LCD_WF_BUSY_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_LCD_WF29_BUSY_PIN);
}
*/

/*
**
*/
void BSP_LORA_DIO0_EXIT_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_LORA_DIO0_PIN);		//调用中断处理公用函数
}


void BSP_LORA_DIO3_EXIT_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BSP_LORA_DIO3_PIN);		//调用中断处理公用函数
}

/*
**
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		//case BSP_LCD_WF29_BUSY_PIN:
		//	bspLcdWfS.step++;
		//	break;
		
		// wzh
		case BSP_LORA_DIO3_PIN:
		  SX1276EventIrqFlag[EVENT_DIO3_IRQ] = 1;
	        break;
			
		case BSP_KEY_UP_PIN:
			break;
		
		case BSP_KEY_DOWN_PIN:
			break;

		case BSP_LORA_DIO0_PIN:
    printf("lora dio0 irq\r\n");
			SX1276EventIrqFlag[EVENT_DIO0_IRQ] = 1;
	        break;
	        
		default:
			break;
	}
	
}


/*
**
*/
void RTC_WKUP_IRQHandler(void)
{
  printf("[%s][%d]", __func__, __LINE__);
	HAL_RTCEx_WakeUpTimerIRQHandler(&bspRtcHandler);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *bspRtcHandler)
{
  printf("[%s][%d]", __func__, __LINE__);
	LowPower_WakeUp();
	TaskStart(TASK_START_CAD);
}


