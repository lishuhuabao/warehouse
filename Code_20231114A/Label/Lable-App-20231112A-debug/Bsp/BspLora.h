#ifndef		__BSP_LORA_H_
#define		__BSP_LORA_H_

#include "myConfig.h"
#include "BspUart.h"
#include "CommLora.h"

/*
**
*/
#define BSP_LORA_SPI                           SPI2

//
#define BSP_LORA_SPI_CLK_ENABLE()               __HAL_RCC_SPI2_CLK_ENABLE()
#define BSP_LORA_SPI_CLK_DISABLE()              __HAL_RCC_SPI2_CLK_DISABLE()	  

//		  
#define BSP_LORA_SPI_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_SCK_PIN                    GPIO_PIN_13
#define BSP_LORA_SPI_SCK_PORT                   GPIOB
#define BSP_LORA_SPI_SCK_PIN_AF                 GPIO_AF5_SPI2

//
#define BSP_LORA_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_MOSI_PIN                    GPIO_PIN_15
#define BSP_LORA_SPI_MOSI_PORT                   GPIOB
#define BSP_LORA_SPI_MOSI_PIN_AF                 GPIO_AF5_SPI2

//
#define BSP_LORA_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LORA_SPI_MISO_PIN                    GPIO_PIN_14
#define BSP_LORA_SPI_MISO_PORT                   GPIOB
#define BSP_LORA_SPI_MISO_PIN_AF                 GPIO_AF5_SPI2

#define __LORA_RST_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LORA_RST_PIN                    GPIO_PIN_11
#define __LORA_RST_PORT                   GPIOB
#define __LORA_RST_(x)                    HAL_GPIO_WritePin(__LORA_RST_PORT, __LORA_RST_PIN, (GPIO_PinState)(x))

#define __LORA_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __LORA_CS_PIN                    GPIO_PIN_8
#define __LORA_CS_PORT                   GPIOA
#define BSP_LORA_CS_(x)                    HAL_GPIO_WritePin(__LORA_CS_PORT, __LORA_CS_PIN, (GPIO_PinState)(x))

#define __LORA_DIO0_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_LORA_DIO0_PIN                  GPIO_PIN_11
#define __LORA_DIO0_PORT                   GPIOA
#define __LORA_DIO0_READ                   HAL_GPIO_ReadPin(__LORA_DIO0_PORT, BSP_LORA_DIO0_PIN)

#define __LORA_DIO0_EXIT_MODE             GPIO_MODE_IT_RISING
#define __LORA_DIO0_EXIT_IRQ              EXTI15_10_IRQn
#define __LORA_DIO0_EXIT_IRQ_PRE_LEV      0
#define __LORA_DIO0_EXIT_IRQ_SUB_LEV      0
#define BSP_LORA_DIO0_EXIT_IRQ_HANDLER    EXTI15_10_IRQHandler


#define __LORA_DIO3_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_LORA_DIO3_PIN                  GPIO_PIN_9
#define __LORA_DIO3_PORT                   GPIOA
#define __LORA_DIO3_READ                   HAL_GPIO_ReadPin(__LORA_DIO3_PORT, BSP_LORA_DIO3_PIN)

#define __LORA_DIO3_EXIT_MODE             GPIO_MODE_IT_RISING
#define __LORA_DIO3_EXIT_IRQ              EXTI9_5_IRQn
#define __LORA_DIO3_EXIT_IRQ_PRE_LEV      0
#define __LORA_DIO3_EXIT_IRQ_SUB_LEV      0
#define BSP_LORA_DIO3_EXIT_IRQ_HANDLER    EXTI9_5_IRQHandler


#define LORA                                        1       // [0: OFF, 1: ON]


/*
**
*/

typedef struct{
	u08 buf[__LORA_PACKET_MAX_NUM];
	u16 len;
}loraSendPacketSt;


extern u32 tes;
extern loraSendPacketSt loraSendPacketS;
//extern u16 netAddr;
//extern u08 netCh;


/*
**
*/
void BspLoraInit(void); 
void BspLoraUartSendPacket(u08 *buf, u16 len);
u08 BspLoraSpITransmit(u08 outData);
void BspLoraSendPacket(u08 *buf, u16 len);
void BspIapUartSendByte(u08 dat);
void BspLoraChSet(u08 ch);


#endif


