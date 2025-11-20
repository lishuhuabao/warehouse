#include "includes.h"

void HAL_MspInit(void)
{
	__HAL_RCC_COMP_CLK_ENABLE();
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}


/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(BSP_DEBUG_UART == huart->Instance)
	{
		BSP_DEBUG_UART_CLK_ENABLE();

		BSP_DEBUG_UART_TX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin 	  = BSP_DEBUG_UART_TX_PIN;
		GPIO_InitStruct.Mode	  = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull	  = GPIO_NOPULL;
		GPIO_InitStruct.Speed	  = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_DEBUG_UART_TX_PIN_AF;
		HAL_GPIO_Init(BSP_DEBUG_UART_TX_PORT, &GPIO_InitStruct);
	}
}


/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(BSP_DEBUG_UART == huart->Instance)
	{
		BSP_DEBUG_UART_CLK_DISABLE();
		HAL_GPIO_DeInit(BSP_DEBUG_UART_TX_PORT, BSP_DEBUG_UART_TX_PIN);
	}
}

/*
**
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(BSP_ZK_GT_SPI == hspi->Instance)
	{
		/* Peripheral clock enable */
		BSP_ZK_GT_SPI_CLK_EN();

		BSP_ZK_GT_SCK_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_ZK_GT_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_ZK_GT_MISO_AF;
		HAL_GPIO_Init(BSP_ZK_GT_SCK_PORT, &GPIO_InitStruct);

		BSP_ZK_GT_MOSI_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_ZK_GT_MOSI_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_ZK_GT_MISO_AF;
		HAL_GPIO_Init(BSP_ZK_GT_MOSI_PORT, &GPIO_InitStruct);

		BSP_ZK_GT_MISO_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_ZK_GT_MISO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_ZK_GT_MISO_AF;
		HAL_GPIO_Init(BSP_ZK_GT_MISO_PORT, &GPIO_InitStruct);
	}
	else if(BSP_LORA_SPI == hspi->Instance)
	{
		/* Peripheral clock enable */
		BSP_LORA_SPI_CLK_ENABLE();

		BSP_LORA_SPI_SCK_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_MISO_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_SCK_PORT, &GPIO_InitStruct);

		BSP_LORA_SPI_MOSI_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_MOSI_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_MISO_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_MOSI_PORT, &GPIO_InitStruct);

		BSP_LORA_SPI_MISO_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_MISO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_MISO_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_MISO_PORT, &GPIO_InitStruct);
	}
}

/*
**
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(BSP_ZK_GT_SPI == hspi->Instance)
	{
		/* Peripheral clock disable */
		BSP_ZK_GT_SPI_CLK_DIS();
		
		HAL_GPIO_DeInit(BSP_ZK_GT_SCK_PORT, BSP_ZK_GT_SCK_PIN);
		HAL_GPIO_DeInit(BSP_ZK_GT_MOSI_PORT, BSP_ZK_GT_MOSI_PIN);
		HAL_GPIO_DeInit(BSP_ZK_GT_MISO_PORT, BSP_ZK_GT_MISO_PIN);
	}
	else if(BSP_LORA_SPI == hspi->Instance)
	{
		/* Peripheral clock disable */
		BSP_LORA_SPI_CLK_DISABLE();
		
		HAL_GPIO_DeInit(BSP_LORA_SPI_SCK_PORT, BSP_LORA_SPI_SCK_PIN);
		HAL_GPIO_DeInit(BSP_LORA_SPI_MOSI_PORT, BSP_LORA_SPI_MOSI_PIN);
		HAL_GPIO_DeInit(BSP_LORA_SPI_MISO_PORT, BSP_LORA_SPI_MISO_PIN);
	}
}

/*
**
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(BSP_BUZ_TIMER == htim_pwm->Instance)
	{
		BSP_BUZ_TIMER_CLK_ENABLE();
		BSP_BUZ_GPIO_CLK_ENABLE();
		
		GPIO_InitStruct.Pin       = BSP_BUZ_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = BSP_BUZ_PIN_AF;
		HAL_GPIO_Init(BSP_BUZ_PORT, &GPIO_InitStruct);
	}
}

/*
**
*/
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
	if(BSP_BUZ_TIMER == htim_pwm->Instance)
	{
		BSP_BUZ_TIMER_CLK_DISABLE();
		HAL_GPIO_DeInit(BSP_BUZ_PORT, BSP_BUZ_PIN);
	}
}

/*
**
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	if(RTC == hrtc->Instance)
	{
		__HAL_RCC_RTC_ENABLE();
		/* RTC interrupt Init */
		HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	}
}

/*
**
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
	if(RTC == hrtc->Instance)
	{
		/* Peripheral clock disable */
		__HAL_RCC_RTC_DISABLE();

		/* RTC interrupt DeInit */
		HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
	}
}

/*
**
*/
//void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
//{
//	if(BSP_BAT_ADC == hadc->Instance)
//	{
//		BSP_BAT_ADC_CLK_EN; 
//	}
//}

/*
**
*/
//void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
//{
//	if(BSP_BAT_ADC == hadc->Instance)
//	{
//		BSP_BAT_ADC_CLK_DIS;
	
//		HAL_GPIO_DeInit(BSP_BAT_VOLT_PORT, BSP_BAT_VOLT_PIN);
//	}

//}


