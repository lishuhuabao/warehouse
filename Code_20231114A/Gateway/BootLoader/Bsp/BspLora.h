#ifndef		__BSP_LORA_H_
#define		__BSP_LORA_H_

#include "myConfig.h"
#include "BspUart.h"
#include "CommLora.h"

/*
**
*/
#define BSP_LORA_SPI                           SPI2

#define __LORA_RST_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __LORA_RST_PIN                    GPIO_PIN_10
#define __LORA_RST_PORT                   GPIOA
#define __LORA_RST_(x)                    HAL_GPIO_WritePin(__LORA_RST_PORT, __LORA_RST_PIN, (GPIO_PinState)(x))


#define BSP_LORA_SPI_CLK_ENABLE()               __HAL_RCC_SPI2_CLK_ENABLE()
#define BSP_LORA_SPI_CLK_DISABLE()              __HAL_RCC_SPI2_CLK_DISABLE()	  

//		  
#define BSP_LORA_SPI_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_SCK_PIN                    GPIO_PIN_13
#define BSP_LORA_SPI_SCK_PORT                   GPIOB
#define BSP_LORA_SPI_SCK_PIN_AF                 GPIO_AF0_SPI2

//
#define BSP_LORA_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_MOSI_PIN                    GPIO_PIN_15
#define BSP_LORA_SPI_MOSI_PORT                   GPIOB
#define BSP_LORA_SPI_MOSI_PIN_AF                 GPIO_AF0_SPI2

//
#define BSP_LORA_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_MISO_PIN                    GPIO_PIN_14
#define BSP_LORA_SPI_MISO_PORT                   GPIOB
#define BSP_LORA_SPI_MISO_PIN_AF                 GPIO_AF0_SPI2

#define __LORA_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __LORA_CS_PIN                    GPIO_PIN_8
#define __LORA_CS_PORT                   GPIOA
#define __LORA_CS_(x)                    HAL_GPIO_WritePin(__LORA_CS_PORT, __LORA_CS_PIN, (GPIO_PinState)(x))

#define __LORA_DIO0_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_LORA_DIO0_PIN                  GPIO_PIN_13
#define __LORA_DIO0_PORT                   GPIOC
#define __LORA_DIO0_READ                   HAL_GPIO_ReadPin(__LORA_DIO0_PORT, BSP_LORA_DIO0_PIN)

#define __LORA_DIO0_EXIT_MODE             GPIO_MODE_IT_RISING
#define __LORA_DIO0_EXIT_IRQ              EXTI4_15_IRQn
#define __LORA_DIO0_EXIT_IRQ_PRE_LEV      0
#define __LORA_DIO0_EXIT_IRQ_SUB_LEV      0
#define BSP_LORA_DIO0_EXIT_IRQ_HANDLER    EXTI4_15_IRQHandler



#define __LORA_DIO1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define __LORA_DIO1_PIN                    GPIO_PIN_14
#define __LORA_DIO1_PORT                   GPIOC
#define __LORA_DIO1_READ                   HAL_GPIO_ReadPin(__LORA_DIO1_PORT, __LORA_DIO1_PIN)
	
	
#define __LORA_DIO3_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __LORA_DIO3_PIN                    GPIO_PIN_11
#define __LORA_DIO3_PORT                   GPIOA
#define __LORA_DIO3_READ                   HAL_GPIO_ReadPin(__LORA_DIO3_PORT, __LORA_DIO3_PIN)
	
#define __LORA_DIO4_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __LORA_DIO4_PIN                    GPIO_PIN_12
#define __LORA_DIO4_PORT                   GPIOA
#define __LORA_DIO4_READ                   HAL_GPIO_ReadPin(__LORA_DIO4_PORT, __LORA_DIO4_PIN)
	

/*
**
*/

typedef struct{
	u08 buf[__LORA_PACKET_MAX_NUM];
	u16 len;
}loraSendPacketSt;

extern loraSendPacketSt loraSendPacketS;

/*
**
*/
void BspLoraInit(void); 
void BspLoraHwReset(void);
u08 SpiInOut(u08 InputData);
void BspLoraChSet(u08 ch);
void BspLoraSendPacket(u08 *buf, u16 len);
u32 BspLoraGetFreqByCh(u08 ch);
#endif


