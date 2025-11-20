#ifndef __BSP_WIFI_H_
#define __BSP_WIFI_H_

#include "BspUart.h"

/*
**
*/
	
#define BSP_WIFI_UART                           USART1 // Net2.2
	
	//
#define BSP_WIFI_UART_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE() // Net2.2
#define BSP_WIFI_UART_CLK_DISABLE()             __HAL_RCC_USART1_CLK_DISABLE()// Net2.2
	
	//		  
#define BSP_WIFI_UART_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_WIFI_UART_TX_PIN                    GPIO_PIN_6 //Net2.2  
#define BSP_WIFI_UART_TX_PORT                   GPIOB
#define BSP_WIFI_UART_TX_PIN_AF                 GPIO_AF0_USART1 //Net2.2 

#define BSP_WIFI_UART_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_WIFI_UART_RX_PIN                    GPIO_PIN_7 //Net2.2  
#define BSP_WIFI_UART_RX_PORT                   GPIOB
#define BSP_WIFI_UART_RX_PIN_AF                 GPIO_AF0_USART1 //Net2.2


#define BSP_WIFI_RST_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_WIFI_RST_PIN                    GPIO_PIN_15 //Net2.2  
#define BSP_WIFI_RST_PORT                   GPIOA
#define BSP_WIFI_RST_(x)                    HAL_GPIO_WritePin(BSP_WIFI_RST_PORT, BSP_WIFI_RST_PIN, (GPIO_PinState)(x)) 
	
#define	BSP_WIFI_BAUD_RATE			                (115200) // (460800)

//
#define BSP_WIFI_UART_IRQ                       USART1_IRQn

//
#define BSP_WIFI_UART_IRQ_HANDLER               USART1_IRQHandler

/*
**
*/
void BspWifiInit(void); //initialize uart for wifi Esp12F;
void BspWifiUartSendPacket(u08 *buf, u16 len);

extern UART_HandleTypeDef bspWifiUartHandler;
extern uartSt bspWifiUartS;//

#endif
