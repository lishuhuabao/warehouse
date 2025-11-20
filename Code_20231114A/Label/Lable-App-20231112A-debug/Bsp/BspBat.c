#include "includes.h"
/*
**
*/
//https://zhidao.baidu.com/question/562289667.html
#define __BAT_VOLT_MAX          13500
#define __BAT_VOLT_MIN          11680


/*
**
*/
bspBatSt bspBatS;
bspBatSt preBspBatS;



/*
**
*/
void BspBatInit(void)
{

}

/*
**
*/
u16 BspBatGetVolt(void)
{
	u32 __val[4] = {0};
	u32 __aveVal;
	u32 __voltVal; //mv
	u08 i;
	u08 j;
	BspBatCheckAwake();
	for(j = 0; j < 4; j++)
	{
		__val[j] = 0;
		for(i = 0; i < 64; i++)
		{
			__val[j] += (u32)BspGetAdc(BSP_BAT_VOLT_ADC_CHANNEL);
		}
		__val[j] >>= 3;
	}
	//TRACE("电池 ADC val is %d\r\n", __val);

	__aveVal = 0;
	for(j = 0; j < 4; j++)
	{
		__aveVal += __val[j];
	}

	__aveVal >>= 2;
	
	__voltVal = __aveVal * 3300 * 2 / 32767;
	BspBatCheckSleep();
	//TRACE("电池电压is %dmV\r\n", __voltVal);
	
	return __voltVal;
}

/*
**
*/
void BspBatGetVoltEvent(void)
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1000))
	{
		return;
	}

	
	bspBatS.volt = BspBatGetVolt();
	
	

	TRACE("电池电压is %dmV\r\n", bspBatS.volt);
}

/*
**
*/
void BspBatAdcPortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	BSP_BAT_VOLT_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Pin  = BSP_BAT_VOLT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BSP_BAT_VOLT_PORT, &GPIO_InitStruct);
}

/*
**
*/
void BspBatAdcPortDeInit(void)
{
	HAL_GPIO_DeInit(BSP_BAT_VOLT_PORT, BSP_BAT_VOLT_PIN);
}


/*
**
*/
void BspBatCheckSleep(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	HAL_ADC_DeInit(&adcHandler);
	
	//HAL_GPIO_WritePin(BSP_BAT_VOLT_PORT, BSP_BAT_VOLT_PIN, (GPIO_PinState)(1));

	GPIO_InitStruct.Pin  = BSP_BAT_VOLT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BSP_BAT_VOLT_PORT, &GPIO_InitStruct);
}

/*
**
*/
void BspBatCheckAwake(void)
{
	BspAdcInit();
	BspBatAdcPortInit();
}


