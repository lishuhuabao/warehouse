#ifndef __BSP_DEBUG_H_
#define __BSP_DEBUG_H_

#include "BspUart.h"

/*
**
*/
#define TRACE                          printf 
	
#define BSP_DEBUG_UART                           USART4 // Net2.2     // USART1 //Net2.1
	
	//
#define BSP_DEBUG_UART_CLK_ENABLE()              __HAL_RCC_USART4_CLK_ENABLE() // Net2.2 // __HAL_RCC_USART1_CLK_ENABLE() //Net2.1
#define BSP_DEBUG_UART_CLK_DISABLE()             __HAL_RCC_USART4_CLK_DISABLE()// Net2.2 // __HAL_RCC_USART1_CLK_DISABLE() //Net2.1	  
	
	//		  
#define BSP_DEBUG_UART_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_DEBUG_UART_TX_PIN                    GPIO_PIN_0 //Net2.2  // GPIO_PIN_9 //Net2.1
#define BSP_DEBUG_UART_TX_PORT                   GPIOA
#define BSP_DEBUG_UART_TX_PIN_AF                 GPIO_AF4_USART4 //Net2.2 // GPIO_AF1_USART1 //Net2.1
	
#define	BSP_DEBUG_BAUD_RATE			           (115200) // (460800)


#define TRACE_LORA_REC_DAT_AS_HEX_EN   1
#define TRACE_LORA_REC_DAT_AS_STR_EN   0

#define TRACE_REC_PC_DAT_AS_HEX_EN     1
#define TRACE_REC_LORA_STEP_INFOR_EN   0
#define TRACE_SEND_LORA_PRO_EN         0
#define TRACE_GET_LABLE_DAT_EN         0
#define TRACE_GET_LABLE_ACK_EN         0
#define TRACE_LORA_NOW_FREQ_EN         0


/*
**
*/
void BspDebugInit(void); //initialize uart for debug;

#endif


