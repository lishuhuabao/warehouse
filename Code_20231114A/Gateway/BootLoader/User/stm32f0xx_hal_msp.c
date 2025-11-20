/*
**
*/
#include "includes.h"


/*
**
*/
void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}

/*
**
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(BSP_LORA_SPI == hspi->Instance)
	{  
		BSP_LORA_SPI_CLK_ENABLE();

		BSP_LORA_SPI_SCK_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_SCK_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_SCK_PORT, &GPIO_InitStruct); 

		BSP_LORA_SPI_MOSI_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_MOSI_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_MOSI_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_MOSI_PORT, &GPIO_InitStruct); 

		BSP_LORA_SPI_MISO_GPIO_CLK_ENABLE();
		GPIO_InitStruct.Pin       = BSP_LORA_SPI_MISO_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_LORA_SPI_MISO_PIN_AF;
		HAL_GPIO_Init(BSP_LORA_SPI_MISO_PORT, &GPIO_InitStruct); 
	}
}

/*
**
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if(BSP_LORA_SPI == hspi->Instance)
	{
		BSP_LORA_SPI_CLK_DISABLE();

		HAL_GPIO_DeInit(BSP_LORA_SPI_SCK_PORT, BSP_LORA_SPI_SCK_PIN);
		HAL_GPIO_DeInit(BSP_LORA_SPI_MOSI_PORT, BSP_LORA_SPI_MOSI_PIN);
		HAL_GPIO_DeInit(BSP_LORA_SPI_MISO_PORT, BSP_LORA_SPI_MISO_PIN);
	}
}

/*
**
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(BSP_DEBUG_UART == huart->Instance)
	{
		/* Peripheral clock enable */
		BSP_DEBUG_UART_CLK_ENABLE();

		BSP_DEBUG_UART_TX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin       = BSP_DEBUG_UART_TX_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_NOPULL;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_DEBUG_UART_TX_PIN_AF;
		HAL_GPIO_Init(BSP_DEBUG_UART_TX_PORT, &GPIO_InitStruct);
	}
	else if(BSP_PC_UART == huart->Instance)
	{
		BSP_PC_UART_CLK_ENABLE();

		BSP_PC_UART_TX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin = BSP_PC_UART_TX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_PC_UART_TX_PIN_AF;
		HAL_GPIO_Init(BSP_PC_UART_TX_PORT, &GPIO_InitStruct);

		BSP_PC_UART_RX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin = BSP_PC_UART_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_PC_UART_RX_PIN_AF;
		HAL_GPIO_Init(BSP_PC_UART_RX_PORT, &GPIO_InitStruct);
	}
	else if(BSP_WIFI_UART == huart->Instance)
	{
		BSP_WIFI_UART_CLK_ENABLE();

		BSP_WIFI_UART_TX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin = BSP_WIFI_UART_TX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_WIFI_UART_TX_PIN_AF;
		HAL_GPIO_Init(BSP_WIFI_UART_TX_PORT, &GPIO_InitStruct);

		BSP_WIFI_UART_RX_GPIO_CLK_ENABLE();

		GPIO_InitStruct.Pin = BSP_WIFI_UART_RX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = BSP_WIFI_UART_RX_PIN_AF;
		HAL_GPIO_Init(BSP_WIFI_UART_RX_PORT, &GPIO_InitStruct);
	}
}

/*
**
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if(BSP_DEBUG_UART == huart->Instance)
	{
		/* Peripheral clock disable */
		BSP_DEBUG_UART_CLK_DISABLE();

		HAL_GPIO_DeInit(BSP_DEBUG_UART_TX_PORT, BSP_DEBUG_UART_TX_PIN);
	}
	else if(BSP_PC_UART == huart->Instance)
	{
		/* Peripheral clock disable */
		BSP_PC_UART_CLK_DISABLE();

		HAL_GPIO_DeInit(BSP_PC_UART_TX_PORT, BSP_PC_UART_TX_PIN);
		HAL_GPIO_DeInit(BSP_PC_UART_RX_PORT, BSP_PC_UART_RX_PIN);
	}
	else if(BSP_WIFI_UART == huart->Instance)
	{
		/* Peripheral clock disable */
		BSP_WIFI_UART_CLK_DISABLE();

		HAL_GPIO_DeInit(BSP_WIFI_UART_TX_PORT, BSP_WIFI_UART_TX_PIN);
		HAL_GPIO_DeInit(BSP_WIFI_UART_RX_PORT, BSP_WIFI_UART_RX_PIN);
	}
}


