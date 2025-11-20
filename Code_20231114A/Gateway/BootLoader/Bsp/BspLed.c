#include "includes.h"

/*
**
*/
//
#define __LED_LORA_TXD_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_LORA_TXD_PIN                    GPIO_PIN_2
#define __LED_LORA_TXD_PORT                   GPIOB
#define __LED_LORA_TXD_(x)                    HAL_GPIO_WritePin(__LED_LORA_TXD_PORT, __LED_LORA_TXD_PIN, (GPIO_PinState)(x)) 

#define __LED_LORA_RXD_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_LORA_RXD_PIN                    GPIO_PIN_10
#define __LED_LORA_RXD_PORT                   GPIOB
#define __LED_LORA_RXD_(x)                    HAL_GPIO_WritePin(__LED_LORA_RXD_PORT, __LED_LORA_RXD_PIN, (GPIO_PinState)(x)) 

#define __LED_PC_TXD_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_PC_TXD_PIN                      GPIO_PIN_0
#define __LED_PC_TXD_PORT                     GPIOB
#define __LED_PC_TXD_(x)                      HAL_GPIO_WritePin(__LED_PC_TXD_PORT, __LED_PC_TXD_PIN, (GPIO_PinState)(x)) 

#define __LED_PC_RXD_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_PC_RXD_PIN                      GPIO_PIN_1
#define __LED_PC_RXD_PORT                     GPIOB
#define __LED_PC_RXD_(x)                      HAL_GPIO_WritePin(__LED_PC_RXD_PORT, __LED_PC_RXD_PIN, (GPIO_PinState)(x)) 

#define __LED_WIFI_TXD_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_WIFI_TXD_PIN                    GPIO_PIN_12
#define __LED_WIFI_TXD_PORT                   GPIOB
#define __LED_WIFI_TXD_(x)                    HAL_GPIO_WritePin(__LED_WIFI_TXD_PORT, __LED_WIFI_TXD_PIN, (GPIO_PinState)(x)) 

#define __LED_WIFI_RXD_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_WIFI_RXD_PIN                    GPIO_PIN_11
#define __LED_WIFI_RXD_PORT                   GPIOB
#define __LED_WIFI_RXD_(x)                    HAL_GPIO_WritePin(__LED_WIFI_RXD_PORT, __LED_WIFI_RXD_PIN, (GPIO_PinState)(x)) 


/*
**
*/
static bspSimPwmSt ledS[BSP_LED_TYPE_MAX];

/*
**
*/
static void __LedPortInit(void); //LED端口初始化

static void __LedPcTxdOutLow(void);
static void __LedPcRxdOutLow(void);
static void __LedLoraTxdOutLow(void);
static void __LedLoraRxdOutLow(void);
static void __LedWifiTxdOutLow(void);
static void __LedWifiRxdOutLow(void);




static void __LedPcTxdOutHigh(void);
static void __LedPcRxdOutHigh(void);
static void __LedLoraTxdOutHigh(void);
static void __LedLoraRxdOutHigh(void);
static void __LedWifiTxdOutHigh(void);
static void __LedWifiRxdOutHigh(void);


static void __LedPcTxdGpioClkEn(void);
static void __LedPcRxdGpioClkEn(void);
static void __LedLoraTxdGpioClkEn(void);
static void __LedLoraRxdGpioClkEn(void);
static void __LedWifiTxdGpioClkEn(void);
static void __LedWifiRxdGpioClkEn(void);



static void (* ledOutLow[])(void) = {
	__LedPcTxdOutLow,
	__LedPcRxdOutLow,
	__LedLoraTxdOutLow,
	__LedLoraRxdOutLow,
	__LedWifiTxdOutLow,
	__LedWifiRxdOutLow,
};

static void (* ledOutHigh[])(void) = {
	__LedPcTxdOutHigh,
	__LedPcRxdOutHigh,
	__LedLoraTxdOutHigh,
	__LedLoraRxdOutHigh,
	__LedWifiTxdOutHigh,
	__LedWifiRxdOutHigh,
};

static const gpioPortSt ledPcPortStab[] = {
	{__LedPcTxdGpioClkEn,   __LED_PC_TXD_PORT,   __LED_PC_TXD_PIN}, //
	{__LedPcRxdGpioClkEn,   __LED_PC_RXD_PORT,   __LED_PC_RXD_PIN}, //
	{__LedLoraTxdGpioClkEn, __LED_LORA_TXD_PORT, __LED_LORA_TXD_PIN}, //
	{__LedLoraRxdGpioClkEn, __LED_LORA_RXD_PORT, __LED_LORA_RXD_PIN},//
	{__LedWifiTxdGpioClkEn, __LED_WIFI_TXD_PORT, __LED_WIFI_TXD_PIN}, //
	{__LedWifiRxdGpioClkEn, __LED_WIFI_RXD_PORT, __LED_WIFI_RXD_PIN},//
};


/*
**LED初始化
*/
void BspLedInit(void)
{
	bspLedTypeEnum __typ;
	
	__LedPortInit(); //LED端口初始化

	for(__typ = BSP_LED_TYPE_PC_TXD; __typ < BSP_LED_TYPE_MAX; __typ++)
	{
		ledS[__typ].runDuty = 10; //10ms
		ledS[__typ].outHigh = ledOutHigh[__typ];
		ledS[__typ].outLow  = ledOutLow[__typ];
	
		BspSimPwmParaReset(&ledS[__typ]);

		BspLedFlashSet(__typ, 2, 500, 500);
	}	
	//BspLedFlashSet(BSP_LED_TYPE_PC_RXD, 2, 500, 500);
}



//LED端口初始化
static void __LedPortInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;
	bspLedTypeEnum __typ;

	for(__typ = BSP_LED_TYPE_PC_TXD; __typ < BSP_LED_TYPE_MAX; __typ++)
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
}

/*
**
*/
static void __LedPcTxdOutLow(void)
{
	__LED_PC_TXD_(1);
}

/*
**
*/
static void __LedPcRxdOutLow(void)
{
	__LED_PC_RXD_(1);
}

/*
**
*/
static void __LedLoraTxdOutLow(void)
{
	__LED_LORA_TXD_(1);
}

static void __LedLoraRxdOutLow(void)
{
	__LED_LORA_RXD_(1);
}


/*
**
*/
static void __LedWifiTxdOutLow(void)
{
	__LED_WIFI_TXD_(1);
}

static void __LedWifiRxdOutLow(void)
{
	__LED_WIFI_RXD_(1);
}


static void __LedPcTxdOutHigh(void)
{
	__LED_PC_TXD_(0);
}

/*
**
*/
static void __LedPcRxdOutHigh(void)
{
	__LED_PC_RXD_(0);
}

/*
**
*/
static void __LedLoraTxdOutHigh(void)
{
	__LED_LORA_TXD_(0);
}

static void __LedLoraRxdOutHigh(void)
{
	__LED_LORA_RXD_(0);
}

/*
**
*/
static void __LedWifiTxdOutHigh(void)
{
	__LED_WIFI_TXD_(0);
}

/*
**
*/
static void __LedWifiRxdOutHigh(void)
{
	__LED_WIFI_RXD_(0);
}




/*
**
*/
static void __LedPcTxdGpioClkEn(void)
{
	__LED_PC_TXD_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedPcRxdGpioClkEn(void)
{
	__LED_PC_RXD_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedLoraTxdGpioClkEn(void)
{
	__LED_LORA_TXD_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedLoraRxdGpioClkEn(void)
{
	__LED_LORA_RXD_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedWifiTxdGpioClkEn(void)
{
	__LED_WIFI_TXD_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __LedWifiRxdGpioClkEn(void)
{
	__LED_WIFI_RXD_GPIO_CLK_ENABLE();
}


/*
**
*/
void BspLedEvent(void)
{
	bspLedTypeEnum __typ;

	for(__typ = BSP_LED_TYPE_PC_TXD; __typ < BSP_LED_TYPE_MAX; __typ++)
	{
		BspSimPwmRunEvent(&ledS[__typ]);
	}
}



/*
**
*/
void BspLedFlashSet(bspLedTypeEnum led, u08 times, u16 on, u16 off)
{
	if(led >= BSP_LED_TYPE_MAX)
	{
		return;
	}
	
	BspSimPwmCall(&ledS[led], times, on, off);
}


