#include "includes.h"
/*
**
*/


#define __BUZ_ON   __BuzCtrl(__BUZ_STA_ON)
#define __BUZ_OFF  __BuzCtrl(__BUZ_STA_OFF)

#define __BUZ_(x) HAL_GPIO_WritePin(BSP_BUZ_PORT, BSP_BUZ_PIN, (GPIO_PinState)x);


//#define __BUZ_PWM_PERIOD      4000 //4K
//#define __BUZ_PWM_PERIOD      8000 //2K
#define __BUZ_PWM_PERIOD      6000 //2K7




typedef enum{
	__BUZ_STA_ON = 0,
	__BUZ_STA_OFF
}buzStaEnum;

/*
**
*/
static  bspSimPwmSt buzS;
TIM_HandleTypeDef   buzTimerHandler;
TIM_OC_InitTypeDef 	buzTimerChHandler;

/*
**
*/
static void __BuzCtrl(buzStaEnum sta);
static void __BuzPwmInit(u16 arr,u16 psc);
static void __BuzOn(void);
static void __BuzOff(void);

/*
**
*/
void BspBuzInit(void)
{
	__BuzPwmInit(250-1,2-1);

	buzS.outHigh = __BuzOn;
	buzS.outLow  = __BuzOff;
	buzS.runDuty = 10; //10ms
	BspSimPwmParaReset(&buzS);
	
//	BspBuzCall(1, 50, 0);
}

/*
**
*/
void BspBuzAwake(void)
{
	//GPIO_Init(__BUZ_PORT, __BUZ_PIN, GPIO_Mode_Out_PP_Low_Fast);
	__BuzPwmInit(250-1,2-1);
}



/*
**
*/
void BspBuzSleep(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};
	
	HAL_TIM_PWM_DeInit(&buzTimerHandler);
	
	
	BSP_BUZ_GPIO_CLK_ENABLE(); 
	
	__BUZ_(0);
	
	__gpioInitSt.Pin   = BSP_BUZ_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_BUZ_PORT, &__gpioInitSt);
}


/*
**
*/
static void __BuzPwmInit(u16 arr,u16 psc)
{  
	buzTimerHandler.Instance           = BSP_BUZ_TIMER; 		
	buzTimerHandler.Init.Prescaler     = psc;	  
	buzTimerHandler.Init.CounterMode   = TIM_COUNTERMODE_UP;
	buzTimerHandler.Init.Period        = arr;		  
	buzTimerHandler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(&buzTimerHandler);	   
	
	buzTimerChHandler.OCMode     = TIM_OCMODE_PWM1; 
	buzTimerChHandler.Pulse      = arr / 2;			
	buzTimerChHandler.OCPolarity = TIM_OCPOLARITY_LOW; 
	HAL_TIM_PWM_ConfigChannel(&buzTimerHandler, &buzTimerChHandler, BSP_BUZ_TIMER_CH);
	
	HAL_TIM_PWM_Start(&buzTimerHandler, BSP_BUZ_TIMER_CH);
		   
}



/*
**
*/
static void __BuzCtrl(buzStaEnum sta)
{
	static buzStaEnum __preSta;

	if(__preSta == sta)
	{
		return;
	}
	__preSta = sta;
	
	if(__BUZ_STA_ON == sta)
	{
		BspBuzAwake();
		HAL_TIM_PWM_Start(&buzTimerHandler, BSP_BUZ_TIMER_CH);
	}
	else
	{
		//HAL_TIM_PWM_Stop(&buzTimerHandler, BSP_BUZ_TIMER_CH);
		//__BUZ_(0);
		BspBuzSleep();
	}
}

/*
**
*/
void BspBuzCall(u08 times, u16 onTime,  u16 offTime)
{
	BspSimPwmCall(&buzS, times, onTime, offTime);
}

/*
**
*/
void BspBuzRunEvent(void)
{
	BspSimPwmRunEvent(&buzS);
}


/*
**
*/
static void __BuzOn(void)
{
	__BUZ_ON;
}

/*
**
*/
static void __BuzOff(void)
{
	__BUZ_OFF;
}


