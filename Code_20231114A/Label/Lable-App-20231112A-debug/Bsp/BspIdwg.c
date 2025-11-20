#include "includes.h"

#if 0

/* IWDG and TIM handlers declaration */
static IWDG_HandleTypeDef   IwdgHandle;
TIM_HandleTypeDef           Input_Handle;
RCC_ClkInitTypeDef          RCC_ClkInitStruct;
RCC_OscInitTypeDef          RCC_OscInitStruct;

uint16_t tmpCC4[2] = {0, 0};
__IO uint32_t uwLsiFreq = 0;
__IO uint32_t uwCaptureNumber = 0;
static uint32_t GetLSIFrequency(void);

/*
**
*/
void BsIdwgpInit(void)
{

	/*##-1- Check if the system has resumed from IWDG reset ####################*/
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
	{
		TRACE("Reset By IDWG!\r\n");
	}

	  __HAL_RCC_CLEAR_RESET_FLAGS();

	  uwLsiFreq = GetLSIFrequency();
  
	  /*##-3- Configure & Start the IWDG peripheral #########################################*/
	  /* Set counter reload value to obtain 1 sec. IWDG TimeOut.
	     IWDG counter clock Frequency = uwLsiFreq
	     Set Prescaler to 32 (IWDG_PRESCALER_32)
	     Timeout Period = (Reload Counter Value * 32) / uwLsiFreq
	     So Set Reload Counter Value = (1 * uwLsiFreq) / 32 */
	     
	  IwdgHandle.Instance = IWDG;
	  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_128;
	  IwdgHandle.Init.Reload = (uwLsiFreq / 32);

	  if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	  {
	    Error_Handler();
	  }

  /* Infinite loop */
  while (1)
  {

    /* Refresh IWDG: reload counter */
    if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
    {
      /* Refresh Error */
      Error_Handler();
    }
  }
}

/**
  * @brief  Configures TIM10 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
static uint32_t GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;


  /* Configure the TIM peripheral *********************************************/ 
  /* Set TIMx instance */  
  Input_Handle.Instance = TIM10;
  
  /* TIM10 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM10 CH1.
     The Rising edge is used as active edge.
     The TIM10 CCR1 is used to compute the frequency value. 
  ------------------------------------------------------------ */
  Input_Handle.Init.Prescaler         = 0;
  Input_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  Input_Handle.Init.Period            = 0xFFFF;
  Input_Handle.Init.ClockDivision     = 0;
  if(HAL_TIM_IC_Init(&Input_Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Connect internally the TIM10_CH1 Input Capture to the LSI clock output */
  HAL_TIMEx_RemapConfig(&Input_Handle, TIM_TIM10_LSI);
  
  /* Configure the Input Capture of channel 1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&Input_Handle, &TIMInput_Config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&Input_Handle, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the TIM10 get 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable TIM10 CC1 Interrupt Request */
  HAL_TIM_IC_Stop_IT(&Input_Handle, TIM_CHANNEL_1);
  
  /* Deinitialize the TIM10 peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&Input_Handle);

  return uwLsiFreq;
}



/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  uint32_t lsiperiod = 0;

  /* Get the Input Capture value */
  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&Input_Handle, TIM_CHANNEL_1);

  if (uwCaptureNumber >= 2)
  {
    /* Compute the period length */
    lsiperiod = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);

    /* Frequency computation */ 
    uwLsiFreq = (uint32_t) SystemCoreClock / lsiperiod;
    uwLsiFreq *= 8;
  }
}

#endif

