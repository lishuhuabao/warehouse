/*
**MCU:STM32F072CBT6
**RAM:16KB
**FLASH:128KB
**Clock:48MHz
*/

#include "includes.h"

/*
**
*/
const u08 hwVersion[] = "V1.0";
const u08 fwTestVer[] = "V1.1";
// const u08 fwRelVer[]  = "RelVer:20200106A";  //wzh
 const u08 fwRelVer[]  = "Net20200426A"; 
// IROM1: 08004000 0D00
// const u08 fwRelVer[]  = "Net20200426B"; 
 // IROM1: 08011000 0D00
 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/*
**
*/


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	BspInit();
	SX1276SetRx(0);

//	while(1)
//	{
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,1);
//	}
	//HAL_Delay(3000);
	//spTaskStart(TASK_SEND_DATA);//Æô¶¯SENDÈÎÎñ
	//HAL_Delay(1000);

	TRACE("system is start!\r\n");

	while(1)
	{
		esp12f_config();
		
		CommPcRecDatHandler();
		CommPcReSendEvent();
		
		CommServerRecDatHandler();
		CommServerReSendEvent();
		
		CommLoraRecDatHandler(); 
		CommLoraReSendEvent();
		
		BspLedEvent();
		BspDataRecordEvent();
		SfTimerEvent();
		spTaskScheduler();
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}





/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


