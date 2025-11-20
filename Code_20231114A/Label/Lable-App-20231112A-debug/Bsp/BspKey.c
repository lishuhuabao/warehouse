#include "includes.h"

/*
**
*/
#define __KEY_SHORT_TIME    5 // 50ms
#define __KEY_LONG_TIME     500 //5s
#define __KEY_MORE_LONG_TIME   1000 //10S

#define __KEY_UP_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define __KEY_UP_PORT                     GPIOB
#define __KEY_UP_READ                     HAL_GPIO_ReadPin(__KEY_UP_PORT, BSP_KEY_UP_PIN) 


#define __KEY_DOWN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __KEY_DOWN_PORT                   GPIOB
#define __KEY_DOWN_READ                   HAL_GPIO_ReadPin(__KEY_DOWN_PORT, BSP_KEY_DOWN_PIN) 



/*
**
*/
static bspKeyValEnum __KeyScan(void);


/*
**
*/
void BspKeyInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;

	__KEY_UP_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin   = BSP_KEY_UP_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_IT_FALLING;
	__gpioInitSt.Pull  = GPIO_PULLUP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__KEY_UP_PORT, &__gpioInitSt);
	
	HAL_NVIC_SetPriority(BSP_KEY_UP_EXIT_IRQ, BSP_KEY_UP_EXIT_IRQ_PRE_LEV, BSP_KEY_UP_EXIT_IRQ_SUB_LEV);		
	HAL_NVIC_EnableIRQ(BSP_KEY_UP_EXIT_IRQ);


	__KEY_DOWN_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin   = BSP_KEY_DOWN_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_IT_FALLING;
	__gpioInitSt.Pull  = GPIO_PULLUP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__KEY_DOWN_PORT, &__gpioInitSt);

	HAL_NVIC_SetPriority(BSP_KEY_DOWN_EXIT_IRQ, BSP_KEY_DOWN_EXIT_IRQ_PRE_LEV, BSP_KEY_DOWN_EXIT_IRQ_SUB_LEV);		
	HAL_NVIC_EnableIRQ(BSP_KEY_DOWN_EXIT_IRQ);
}


/*
**
*/
static bspKeyValEnum __KeyScan(void)
{
	bspKeyValEnum __keyVal;
	static u16 preKeyVal;
	static u16 pressed;

	__keyVal = BSP_KEY_VAL_NONE;

	__keyVal |= ((!__KEY_UP_READ) << BSP_KEY_ID_UP) | 
	            ((!__KEY_DOWN_READ) << BSP_KEY_ID_DOWN);

	if(__keyVal)
	{
		if(preKeyVal != __keyVal)
		{
			preKeyVal = __keyVal;
			pressed   = 0;
		}
		else
		{
			if(pressed < 0xffff)
			{
				pressed++;
			}
			
			if(__KEY_SHORT_TIME == pressed)
			{
				return __keyVal;
			}
			else if(__KEY_LONG_TIME == pressed)
			{
				return ((bspKeyValEnum)(0x0F00 | __keyVal));
			}	
			else if(__KEY_MORE_LONG_TIME == pressed)
			{
				return ((bspKeyValEnum)(0x0F00 | __keyVal));
			}
		}
		ClrPowerEnterTime();
	}
	else
	{
		pressed = 0;
	}
	
	return BSP_KEY_VAL_NONE;
}



/*
**
*/
void BspKeyEvent(void)
{   
	bspKeyValEnum __keyVal;
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 10))
	{
		return;
	}


	__keyVal = __KeyScan();
	//TRACE("__keyCode is %d\r\n", __keyVal);
	
    if(__keyVal)
    {
    	HandlerKey(__keyVal);
    	//TRACE("__keyCode is %d\r\n", __keyVal);
    }
}





