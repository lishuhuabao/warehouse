#ifndef		__BSP_PC_H_
#define		__BSP_PC_H_

#include "myConfig.h"
#include "BspUart.h"

/*
**
*/
#define BSP_PC_UART                           USART2

//
#define BSP_PC_UART_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define BSP_PC_UART_CLK_DISABLE()             __HAL_RCC_USART2_CLK_DISABLE()		  

//		  
#define BSP_PC_UART_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_PC_UART_TX_PIN                    GPIO_PIN_2
#define BSP_PC_UART_TX_PORT                   GPIOA
#define BSP_PC_UART_TX_PIN_AF                 GPIO_AF1_USART2

//
#define BSP_PC_UART_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_PC_UART_RX_PIN                    GPIO_PIN_3
#define BSP_PC_UART_RX_PORT                   GPIOA
#define BSP_PC_UART_RX_PIN_AF                 GPIO_AF1_USART2


//
#define	BSP_PC_BAUD_RATE			            (115200)

//
#define BSP_PC_UART_IRQ                      USART2_IRQn

//
#define BSP_PC_UART_IRQ_HANDLER              USART2_IRQHandler


/*
**
*/
extern UART_HandleTypeDef bspPcUartHandler;
extern uartSt bspPcUartS;//


/*
**
*/
void BspPcInit(void); 
void BspPcUartSendPacket(u08 *buf, u16 len);


#endif


