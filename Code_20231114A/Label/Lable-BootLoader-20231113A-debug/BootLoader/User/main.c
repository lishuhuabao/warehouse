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
//#define APPLICATION_ADDRESS   (u32)0x08004000

#define __LED_RED_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_RED_PIN                    GPIO_PIN_10
#define __LED_RED_PORT                   GPIOB
#define __LED_RED_(x)                    HAL_GPIO_WritePin(__LED_RED_PORT, __LED_RED_PIN, (GPIO_PinState)(x)) 
#define __LED_RED_TOGGLE()               HAL_GPIO_TogglePin(__LED_RED_PORT, __LED_RED_PIN) 

#define __LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_GREEN_PIN                    GPIO_PIN_5
#define __LED_GREEN_PORT                   GPIOB
#define __LED_GREEN_(x)                    HAL_GPIO_WritePin(__LED_GREEN_PORT, __LED_GREEN_PIN, (GPIO_PinState)(x)) 
#define __LED_GREEN_TOGGLE()               HAL_GPIO_TogglePin(__LED_GREEN_PORT, __LED_GREEN_PIN) 

#define __LED_BLUE_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_BLUE_PIN                    GPIO_PIN_4
#define __LED_BLUE_PORT                   GPIOB
#define __LED_BLUE_(x)                    HAL_GPIO_WritePin(__LED_BLUE_PORT, __LED_BLUE_PIN, (GPIO_PinState)(x)) 
#define __LED_BLUE_TOGGLE()               HAL_GPIO_TogglePin(__LED_BLUE_PORT, __LED_BLUE_PIN) 

static void __GetUpdatePara(void);
static void __ExitUpdata(void);
	
//u08 softwareVersion[] = "V2.0.0";

typedef void (*iapJumpAppFunc)(void);
iapJumpAppFunc  iapJumpApp = NULL;


/*
**
*/


void __LedPortInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;

	__LED_RED_GPIO_CLK_ENABLE();

	//configure  pin as output without pull-up and push-down  resiter,
	//high port speed;
	__gpioInitSt.Pin   = __LED_RED_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull  = GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__LED_RED_PORT, &__gpioInitSt);
	__LED_RED_(0);
	
	__LED_GREEN_GPIO_CLK_ENABLE();
	__gpioInitSt.Pin   = __LED_GREEN_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull  = GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__LED_RED_PORT, &__gpioInitSt);
	__LED_GREEN_(0);
	
	__LED_BLUE_GPIO_CLK_ENABLE();
	__gpioInitSt.Pin   = __LED_BLUE_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull  = GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__LED_RED_PORT, &__gpioInitSt);
  __LED_BLUE_(0);
	
}


/*
**
*/
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();
  

  /* Clear all FLASH flags */  
 // __HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
 //                 //| FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
 //                   | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR |FLASH_FLAG_RDERR);
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
	                    | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR |FLASH_FLAG_RDERR);
	
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
  
	loraSendFlag = false;
	
	__LedPortInit();
	
	BspInit();	
	SX1276SetMaxPayloadLength(MODEM_LORA, 255); // wzh
	SX1276SetRx(2000);
	
	__GetUpdatePara();	
	__ExitUpdata();
	
  //HAL_Delay(3000);
//	TaskStart(TASK_START_CAD);
	//HAL_Delay(1000);

	while (1)
	{
		CommLoraRecDatHandler(); 
		//CommLoraReSendEvent();
	
		GetUpdatPacketDatTimeout();
		__ExitUpdata();	
			
//		HandlerPowerEvent();	
		SfTimerEvent();		
		TaskScheduler();
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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif 

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
		__LED_RED_(1);//¡¡∫Ïµ∆		
		commLoraCtrlS.step = COMM_LORA_STEP_UPDATA;
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
	static unsigned int app_address = 0;  //0x08004000
//	static osDelaySt __osDelayS;
	
	if( (APP_UPDATE_OTP == updateParaSaveS.updateFlag) || (loraSendFlag ==true))
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
	
	__LED_GREEN_(0);
	if(updateParaSaveS.curAppAddress == FLASH_APP_START_ADDR){
		__LED_RED_(1);
	}
	else if (updateParaSaveS.curAppAddress == FLASH_BACKUP_START_ADDR) {
	__LED_BLUE_(1);
	}
			HAL_Delay(500);
	app_address = *(volatile unsigned int *)(updateParaSaveS.curAppAddress + 4);
	iapJumpApp = (iapJumpAppFunc)app_address;
	__set_MSP(*(volatile unsigned int *)updateParaSaveS.curAppAddress);	
	iapJumpApp();
}
