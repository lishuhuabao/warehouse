#include "includes.h"

/*
**
*/
void BspPinModeSwitch(GPIO_TypeDef  *gpio, u32 pin, bspPinModEnum mod)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(BSP_PIN_INPUT == mod)
	{
		//Input pull-up, no external interrupt
		GPIO_InitStruct.Pin       = pin;
		GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(gpio, &GPIO_InitStruct);
	}
	else
	{	//Output push-pull, low level, 10MHz 
		GPIO_InitStruct.Pin       = pin;
		GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(gpio, &GPIO_InitStruct);
	}


}






