#ifndef __BSP_BUZ_H_
#define __BSP_BUZ_H_

/*
**
*/
#define BSP_BUZ_TIMER     TIM2
#define BSP_BUZ_TIMER_CH  TIM_CHANNEL_1


#define BSP_BUZ_TIMER_CLK_ENABLE()  __HAL_RCC_TIM2_CLK_ENABLE()  
#define BSP_BUZ_TIMER_CLK_DISABLE() __HAL_RCC_TIM2_CLK_DISABLE()  

#define BSP_BUZ_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_BUZ_PORT               GPIOA
#define BSP_BUZ_PIN                GPIO_PIN_15
#define BSP_BUZ_PIN_AF             GPIO_AF1_TIM2


/*
**
*/
void BspBuzInit(void);
void BspBuzSleep(void);
void BspBuzAwake(void);
void BspBuzCall(u08 times, u16 onTime,  u16 offTime);
void BspBuzRunEvent(void);

#endif 

