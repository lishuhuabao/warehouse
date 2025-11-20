#ifndef __BSP_DEBUG_H_
#define __BSP_DEBUG_H_

#include "BspUart.h"

/*
**
*/
#define TRACE                                    printf 

#define BSP_DEBUG_UART                           USART1

//
#define BSP_DEBUG_UART_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define BSP_DEBUG_UART_CLK_DISABLE()             __HAL_RCC_USART1_CLK_DISABLE()		  

//		  
#define BSP_DEBUG_UART_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_DEBUG_UART_TX_PIN                    GPIO_PIN_6
#define BSP_DEBUG_UART_TX_PORT                   GPIOB
#define BSP_DEBUG_UART_TX_PIN_AF                 GPIO_AF7_USART1

#define	BSP_DEBUG_BAUD_RATE			           (115200)

#define DEBUG_TX_EN                    1   //wzh
#define TRACE_REC_DAT_LORA_AS_HEX_EN   1
#define TRACE_REC_DAT_LORA_AS_STR_EN   0
#define TRACE_REC_LORA_STEP_INFOR_EN   0
#define TRACE_SEND_LORA_PRO_DATA_EN    1
#define TRACE_SEND_LORA_REG_ADDR_EN    0
#define TRACE_GET_NET_DATA_EN          0
#define TRACE_KEY_VAL_EN               0
#define TRACE_LCD_STEP_EN              0
#define TRACE_ADDR_FIX_FOR_TEST_EN     0

/*
**
*/
extern UART_HandleTypeDef bspDebugUartHandler;


/*
**
*/
void BspDebugInit(void); //initialize uart for debug;
void BspDebugSleep(void);
void BspDebugAwake(void);
#endif


