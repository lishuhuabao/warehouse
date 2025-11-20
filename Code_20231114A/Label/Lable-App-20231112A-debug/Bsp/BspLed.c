#include "includes.h"

/*
**
*/
// 

#define __LED_RED_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_RED_PIN                    GPIO_PIN_4
#define __LED_RED_PORT                   GPIOB
#define __LED_RED_(x)                    HAL_GPIO_WritePin(__LED_RED_PORT, __LED_RED_PIN, (GPIO_PinState)(x)) 
#define __LED_RED_TOGGLE()               HAL_GPIO_TogglePin(__LED_RED_PORT, __LED_RED_PIN) 

#define __LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_GREEN_PIN                    GPIO_PIN_5
#define __LED_GREEN_PORT                   GPIOB
#define __LED_GREEN_(x)                    HAL_GPIO_WritePin(__LED_GREEN_PORT, __LED_GREEN_PIN, (GPIO_PinState)(x)) 
#define __LED_GREEN_TOGGLE()               HAL_GPIO_TogglePin(__LED_GREEN_PORT, __LED_GREEN_PIN) 

#define __LED_BLUE_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_BLUE_PIN                    GPIO_PIN_10
#define __LED_BLUE_PORT                   GPIOB
#define __LED_BLUE_(x)                    HAL_GPIO_WritePin(__LED_BLUE_PORT, __LED_BLUE_PIN, (GPIO_PinState)(x)) 
#define __LED_BLUE_TOGGLE()               HAL_GPIO_TogglePin(__LED_BLUE_PORT, __LED_BLUE_PIN) 


/*
**
*/
static bspSimPwmSt ledS[BSP_LED_TYPE_MAX];
bspLedStaEnum bspLedSta[BSP_LED_TYPE_MAX];

/*
**
*/
static void __LedPortInit(void); //LED端口初始化

static void __LedMid1OutLow(void);
static void __LedMid2OutLow(void);
static void __LedMid3OutLow(void);

static void __LedMid1OutHigh(void);
static void __LedMid2OutHigh(void);
static void __LedMid3OutHigh(void);

static void __LedMid1GpioClkEn(void);
static void __LedMid2GpioClkEn(void);
static void __LedMid3GpioClkEn(void);



static void (* ledOutLow[])(void) = {
	__LedMid1OutLow,
	__LedMid2OutLow,
	__LedMid3OutLow,
};

static void (* ledOutHigh[])(void) = {
	__LedMid1OutHigh,
	__LedMid2OutHigh,
	__LedMid3OutHigh,
};

static const gpioPortSt ledPcPortStab[] = {
	{__LedMid1GpioClkEn, __LED_RED_PORT, __LED_RED_PIN}, //
	{__LedMid2GpioClkEn, __LED_GREEN_PORT, __LED_GREEN_PIN}, //
	{__LedMid3GpioClkEn, __LED_BLUE_PORT, __LED_BLUE_PIN},//
};

/*
**LED初始化
*/
void BspLedInit(void)
{
	bspLedTypeEnum __typ;
	
	__LedPortInit(); //LED端口初始化

	for(__typ = BSP_LED_TYPE_RED; __typ < BSP_LED_TYPE_MAX; __typ++)
	{
		ledS[__typ].runDuty = 10; //10ms
		ledS[__typ].outHigh = ledOutHigh[__typ];
		ledS[__typ].outLow  = ledOutLow[__typ];
	
		BspSimPwmParaReset(&ledS[__typ]);

		//BspLedFlashForceSet(__typ, 2, 500, 500);
	}
}



//LED端口初始化
static void __LedPortInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;
	bspLedTypeEnum __typ;

	for(__typ = BSP_LED_TYPE_RED; __typ < BSP_LED_TYPE_MAX; __typ++)
	{
		ledPcPortStab[__typ].ClkEn();

		//configure  pin as output without pull-up and push-down  resiter,
		//high port speed;
		__gpioInitSt.Pin   = ledPcPortStab[__typ].pin;
		__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
		__gpioInitSt.Pull  = GPIO_NOPULL;
		__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(ledPcPortStab[__typ].gpio, &__gpioInitSt);
	}

	__LED_RED_(0);
	__LED_GREEN_(0);
	__LED_BLUE_(0);
}

/*
**
*/
static void __LedMid1OutLow(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_RED] = BSP_LED_STA_OFF;
	#else
		__LED_RED_(0);
	#endif
}

/*
**
*/
static void __LedMid2OutLow(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_GREEN] = BSP_LED_STA_OFF;
	#else
		__LED_GREEN_(0);
	#endif
}

/*
**
*/
static void __LedMid3OutLow(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_BLUE] = BSP_LED_STA_OFF;
	#else
		__LED_BLUE_(0);
	#endif
}

/*
**
*/
static void __LedMid1OutHigh(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_RED] = BSP_LED_STA_ON;
	#else
		__LED_RED_(1);
	#endif
}

/*
**
*/
static void __LedMid2OutHigh(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_GREEN] = BSP_LED_STA_ON;
	#else
		__LED_GREEN_(1);
	#endif
}

/*
**
*/
static void __LedMid3OutHigh(void)
{
	#if BSP_LED_PWM_EN
		bspLedSta[BSP_LED_TYPE_BLUE] = BSP_LED_STA_ON;
	#else
		__LED_BLUE_(1);
	#endif
}

/*
**
*/
static void __LedMid1GpioClkEn(void)
{
	__LED_RED_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedMid2GpioClkEn(void)
{
	__LED_GREEN_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedMid3GpioClkEn(void)
{
	__LED_BLUE_GPIO_CLK_ENABLE();
}


/*
**
*/
void BspLedEvent(void)
{
	bspLedTypeEnum __typ;

	for(__typ = BSP_LED_TYPE_RED; __typ < BSP_LED_TYPE_MAX; __typ++)
	{
		BspSimPwmRunEvent(&ledS[__typ]);
	}
	
}



/*
**
*/
void BspLedFlashSet(bspLedTypeEnum led, u32 times, u32 on, u32 off)
{
	if(led >= BSP_LED_TYPE_MAX)
	{
		return;
	}
	BspSimPwmCall(&ledS[led], times, on, off);
}

/*
**
*/
void BspLedFlashForceSet(bspLedTypeEnum led, u32 times, u32 on, u32 off)
{
	ledS[led].sta = SIM_PWM_STA_OK;
	BspLedFlashSet(led, times,  on,  off);
}



/*
**
*/
void BspLedFlashForceReset(bspLedTypeEnum led)
{
	BspSimPwmParaReset(&ledS[led]);
}


/*
**
*/
void BspLedTest(void)
{
	static osDelaySt __osDelayS;
	static u32 __cnt;

	if(__cnt > 3000)
	{
		return;
	}
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	if(0 == __cnt)
	{
		BspLedFlashForceSet(BSP_LED_TYPE_RED, 1, 500, 200);
		BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 0, 500, 500);
		BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 0, 500, 500);
	}
	else if(1000 == __cnt)
	{
		BspLedFlashForceSet(BSP_LED_TYPE_RED, 0, 500, 500);
		BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 1, 500, 200);
		BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 0, 500, 500);

	}
	else if(3000 == __cnt)
	{
		BspLedFlashForceSet(BSP_LED_TYPE_RED, 0, 500, 500);
		BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 0, 500, 500);
		BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 1, 500, 200);
	}
	__cnt++;
}




