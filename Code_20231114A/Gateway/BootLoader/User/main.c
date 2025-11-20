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
typedef void (*iapJumpAppFunc)(void);
iapJumpAppFunc  iapJumpApp = NULL;

const u08 fwRelVer[]  = "RelVer:20200318"; 


/*
**
*/
static void __GetUpdatePara(void);
static void __ExitUpdata(void);


/*
**
*/
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();
  

  /* Clear all FLASH flags */      
  //__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  //| FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
                  // | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR |FLASH_FLAG_RDERR);
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);
}


/*
**
*/
int main(void)
{
	HAL_Init();
	/* Unlock the Flash Program Erase controller */
	FLASH_If_Init();

	/* Configure the system clock */	
	SystemClock_Config();
	BspInit();
		
	__GetUpdatePara();
	__ExitUpdata();

	//TRACE("system is start!\r\n");

	while(1)
	{
		esp12f_config();
		
		CommPcRecDatHandler();
		//CommPcReSendEvent();
		
		CommServerRecDatHandler();
		
	  GetUpdatPacketDatTimeout();
		__ExitUpdata();	
		
		BspLedEvent();	
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
void _Error_Handler(char *file, int line)
{
 /* USER CODE BEGIN Error_Handler_Debug */
	//printf("\r\n Error Handler %s line %d\r\n",file,line);
	//printf("\r\n \r\n");
	//printf("\r\n  \r\n");
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/********************************************************************************
**Function:get iapS parameter;
**Input: iapS parameter
**Mark    : none
********************************************************************************/
static void __GetUpdatePara(void)
{	
	//u16 i;
	//u08 __fillBuf[PRO_DAT_PACK_LEN_128];
	
	BspIntFlashRead(FLASH_UPDATE_FLAG_START_ADDR, (u08 *)&updateParaSaveS, sizeof(updateParaSaveS));
	//updateFlag = 1;

	//TRACE("updateFlag is %x\r\n", updateParaSaveS.updateFlag);
	if ((updateParaSaveS.updateNum == 0 ) || (updateParaSaveS.updateNum == 0xFFFFFFFF))
  {
		 updateParaSaveS.updateNum = 0;
		 updateParaSaveS.curAppAddress = FLASH_APP_START_ADDR;
		 updateParaSaveS.nextAppAddress = FLASH_BACKUP_START_ADDR;
	} 

	if(APP_UPDATE_OTP == updateParaSaveS.updateFlag)
	{
		//__LED_RED_(0);
		BspLedFlashSet(BSP_LED_TYPE_PC_RXD, 0xFF, 500, 500);
		//commLoraCtrlS.step = COMM_LORA_STEP_UPDATA;
		//BspIntFlashErasePages(FLASH_BACKUP_START_ADDR, FLASH_BACKUP_END_ADDR);
		
		//for(i = 0; i < FLASH_BACKUP_SIZE / sizeof(__fillBuf); i++)
		//{
		//	BspIntFlashRead(FLASH_APP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
		//	BspIntFlashWrite(FLASH_BACKUP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
		//}
		//BspIntFlashErasePages(FLASH_APP_START_ADDR, FLASH_APP_END_ADDR);
		
		while(HAL_OK != BspIntFlashErasePages(updateParaSaveS.nextAppAddress, (u32)(updateParaSaveS.nextAppAddress + FLASH_APP_SIZE)))
    {
			HAL_Delay(1);
		}
		
		#if TRACE_BACKUP_EN
			TRACE("backup is over!\r\n");
		#endif
	}
}

/*
**
*/
static void __ExitUpdata(void)
{
	static unsigned int app_address = 0;
//	static osDelaySt __osDelayS;
	
	if(APP_UPDATE_OTP == updateParaSaveS.updateFlag)
	{
		//TRACE("no exit updata!\r\n");
		return;
	}
	
	//TRACE("Jump To App!\r\n");
	
	//BspIntFlashErasePages(FLASH_UPDATE_FLAG_START_ADDR, FLASH_UPDATE_FLAG_END_ADDR);	
	
	//if(DELAY_UNREACH == SystemDelay(&__osDelayS, 10000))
	//{
	//	return;
	//}
	
	memcpy((void*)0x20000000,(void*)updateParaSaveS.curAppAddress,0x100);

  SYSCFG->CFGR1|=0x03;
	
	app_address = *(volatile unsigned int *)(updateParaSaveS.curAppAddress + 4);
	iapJumpApp = (iapJumpAppFunc)app_address;
	__set_MSP(*(volatile unsigned int *)updateParaSaveS.curAppAddress);	
	iapJumpApp();
}
