/*
**MCU:STM32L151CBT6A
**Clock:32MHz
**Flash:128KB
**RAM:32KB
*/

#include "includes.h"

/*
**
*/
u08 rstRes;
//const char DisplayVersion[] = "Ver:7.1.4";
//const char Version[] = "EleLableV7.1.4A";
const char DisplayVersion[] = "Ver:6.0.1";
const char Version[] = "EleLableV6.0.1.A";

/*
**
*/
void GetResetReasion(void)
{
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)
	{
		rstRes |= BIT(0);
		//TRACE("RCC_FLAG_OBLRST复位!\r\n");
	}
	
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
	{
		rstRes |= BIT(1);
		//TRACE("RCC_FLAG_PINRST复位!\r\n");
	}

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
	{
		rstRes |= BIT(2);
		//TRACE("RCC_FLAG_PORRST复位!\r\n");
	}

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
	{
		rstRes |= BIT(3);
		//TRACE("RCC_FLAG_SFTRST复位!\r\n");
	}

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
	{
		rstRes |= BIT(4);
		//TRACE("RCC_FLAG_IWDGRST复位!\r\n");
	}

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
	{
		rstRes |= BIT(5);
		//TRACE("RCC_FLAG_WWDGRST复位!\r\n");
	}

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)
	{
		rstRes |= BIT(6);
		//TRACE("RCC_FLAG_LPWRRST复位!\r\n");
	}

	__HAL_RCC_CLEAR_RESET_FLAGS();
	
}


/*
**
*/
int main(void)
{
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();
	BspInit();
	// TaskStart(TASK_START_CAD);
	SX1276SetRx(0);
	TRACE("init \r\n");

	while(1)
	{			
		CommLoraRecDatHandler(); 
		CommLoraReSendEvent();
		BspLedEvent();
		BspKeyEvent();
		
		BspDataRecordEvent();
		BspBuzRunEvent();
		BspLcdRefreshEvent();
		BspLedEvent();// wzh
		BspLedTest();		
//		HandlerPowerEvent();	
		SfTimerEvent();	
		TaskScheduler();
		BspLedEvent();// wzh	 
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
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Configure the main internal regulator output voltage 
	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8; //wzhRCC_SYSCLK_DIV1;//
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}






void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	printf("\r\n Error Handler %s line %d\r\n",file,line);
	printf("\r\n \r\n");
	printf("\r\n  \r\n");
/*************************************ERROR PROCESS************************************************/
//	__set_FAULTMASK(1);
  //NVIC_SystemReset();
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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif 


