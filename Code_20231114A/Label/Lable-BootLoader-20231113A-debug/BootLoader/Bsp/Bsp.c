#include "includes.h"

/*
**
*/
#define __IDLE1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __IDLE1_PIN                    GPIO_PIN_3      //Float
#define __IDLE1_PORT                   GPIOB
#define __IDLE1_(x)                    HAL_GPIO_WritePin(__IDLE1_PORT, __IDLE1_PIN, (GPIO_PinState)(x)) 
	
#define __IDLE2_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __IDLE2_PIN                    GPIO_PIN_7      //Float
#define __IDLE2_PORT                   GPIOB
#define __IDLE2_(x)                    HAL_GPIO_WritePin(__IDLE2_PORT, __IDLE2_PIN, (GPIO_PinState)(x)) 
	
#define __IDLE3_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __IDLE3_PIN                    GPIO_PIN_12     //Lora Di01
#define __IDLE3_PORT                   GPIOA
#define __IDLE3_(x)                    HAL_GPIO_WritePin(__IDLE3_PORT, __IDLE3_PIN, (GPIO_PinState)(x)) 
	
#define __IDLE4_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __IDLE4_PIN                    GPIO_PIN_10    //Lora Di04
#define __IDLE4_PORT                   GPIOA
#define __IDLE4_(x)                    HAL_GPIO_WritePin(__IDLE4_PORT, __IDLE4_PIN, (GPIO_PinState)(x)) 

#define __IDLE5_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __IDLE5_PIN                    GPIO_PIN_13   //JTAG Swdio                                        
#define __IDLE5_PORT                   GPIOA
#define __IDLE5_(x)                    HAL_GPIO_WritePin(__IDLE5_PORT, __IDLE5_PIN, (GPIO_PinState)(x)) 

#define __IDLE6_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define __IDLE6_PIN                    GPIO_PIN_14   //JTAG Swclk                                        
#define __IDLE6_PORT                   GPIOA
#define __IDLE6_(x)                    HAL_GPIO_WritePin(__IDLE6_PORT, __IDLE6_PIN, (GPIO_PinState)(x)) 

#define __IDLE7_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __IDLE7_PIN                    GPIO_PIN_6  //Debug                                       
#define __IDLE7_PORT                   GPIOB
#define __IDLE7_(x)                    HAL_GPIO_WritePin(__IDLE7_PORT, __IDLE7_PIN, (GPIO_PinState)(x)) 

#define __IDLE8_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __IDLE8_PIN                    GPIO_PIN_12  //Adc                                       
#define __IDLE8_PORT                   GPIOB
#define __IDLE8_(x)                    HAL_GPIO_WritePin(__IDLE8_PORT, __IDLE8_PIN, (GPIO_PinState)(x))

#define __IDLE9_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define __IDLE9_PIN                    GPIO_PIN_0  //ENCRY I2C_SCL                                      
#define __IDLE9_PORT                   GPIOH
#define __IDLE9_(x)                    HAL_GPIO_WritePin(__IDLE9_PORT, __IDLE9_PIN, (GPIO_PinState)(x))

#define __IDLE10_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define __IDLE10_PIN                    GPIO_PIN_1  //ENCRY I2C_SDA                                      
#define __IDLE10_PORT                   GPIOH
#define __IDLE10_(x)                    HAL_GPIO_WritePin(__IDLE10_PORT, __IDLE10_PIN, (GPIO_PinState)(x))

#define __ENCRY_PWR_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define __ENCRY_PWR_PIN                    GPIO_PIN_13
#define __ENCRY_PWR_PORT                   GPIOC
#define __ENCRY_PWR_(x)                    HAL_GPIO_WritePin(__ENCRY_PWR_PORT, __ENCRY_PWR_PIN, (GPIO_PinState)(x)) 


//#define __IDLE5_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
//#define __IDLE5_PIN                    GPIO_PIN_9          //Lora Di03
//#define __IDLE5_PORT                   GPIOA
//#define __IDLE5_(x)                    HAL_GPIO_WritePin(__IDLE5_PORT, __IDLE5_PIN, (GPIO_PinState)(x))
 

typedef enum{
	__IDLE_PIN_1 = 0,
	__IDLE_PIN_2,
	__IDLE_PIN_3,
	__IDLE_PIN_4,
	__IDLE_PIN_5,
	__IDLE_PIN_6,
	__IDLE_PIN_7,
	__IDLE_PIN_8,
	__IDLE_PIN_9,
	__IDLE_PIN_10,
	__IDLE_PIN_MAX
}idlePinEnum;

/*
**
*/
static void __IdlePinInit(void);
static void __Idle1GpioClkEn(void);
static void __Idle2GpioClkEn(void);
static void __Idle3GpioClkEn(void);
static void __Idle4GpioClkEn(void);
static void __Idle5GpioClkEn(void);
static void __Idle6GpioClkEn(void);
static void __Idle7GpioClkEn(void);
static void __Idle8GpioClkEn(void);
static void __Idle9GpioClkEn(void);
static void __Idle10GpioClkEn(void);
static void __Idle1GpioOutLow(void);
static void __Idle2GpioOutLow(void);
static void __Idle3GpioOutLow(void);
static void __Idle4GpioOutLow(void);
static void __Idle5GpioOutLow(void);
static void __Idle6GpioOutLow(void);
static void __Idle7GpioOutLow(void);
static void __Idle8GpioOutLow(void);
static void __Idle9GpioOutLow(void);
static void __Idle10GpioOutLow(void);

static const gpioPortSt idlePortStab[] = {
	{__Idle1GpioClkEn,  __IDLE1_PORT,  __IDLE1_PIN,  __Idle1GpioOutLow}, //
	{__Idle2GpioClkEn,  __IDLE2_PORT,  __IDLE2_PIN,  __Idle2GpioOutLow}, //
	{__Idle3GpioClkEn,  __IDLE3_PORT,  __IDLE3_PIN,  __Idle3GpioOutLow},//
	{__Idle4GpioClkEn,  __IDLE4_PORT,  __IDLE4_PIN,  __Idle4GpioOutLow},
	{__Idle5GpioClkEn,  __IDLE5_PORT,  __IDLE5_PIN,  __Idle5GpioOutLow},
	{__Idle6GpioClkEn,  __IDLE6_PORT,  __IDLE6_PIN,  __Idle6GpioOutLow}, //
	{__Idle7GpioClkEn,  __IDLE7_PORT,  __IDLE7_PIN,  __Idle7GpioOutLow}, //
	{__Idle8GpioClkEn,  __IDLE8_PORT,  __IDLE8_PIN,  __Idle8GpioOutLow},//
	{__Idle9GpioClkEn,  __IDLE9_PORT,  __IDLE9_PIN,  __Idle9GpioOutLow},
	{__Idle10GpioClkEn, __IDLE10_PORT, __IDLE10_PIN, __Idle10GpioOutLow},
};

/*
**
*/
void BspInit(void)
{
  //BspDebugInit();
//  BspRtcInit();
	BspDataRecordInit();
	BspLoraInit();
//  __IdlePinInit();
}

/*
**out 0 :150uA
**out 1 :150uA
**FLOAT IN:500UA
**up in:150uA
**down in:150UA
*/
static void __IdlePinInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;
	u08 i;

	for(i = 0; i < __IDLE_PIN_MAX; i++)
	{
		idlePortStab[i].ClkEn();

		__gpioInitSt.Pin   = idlePortStab[i].pin;
		__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
		__gpioInitSt.Pull  = GPIO_PULLDOWN;
		__gpioInitSt.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(idlePortStab[i].gpio, &__gpioInitSt);	
		
		idlePortStab[i].OutLow();
	}
	
	__ENCRY_PWR_GPIO_CLK_ENABLE(); 
	__gpioInitSt.Pin   = __ENCRY_PWR_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull  = GPIO_PULLDOWN;//GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(__ENCRY_PWR_PORT, &__gpioInitSt);	 
  __ENCRY_PWR_(0);//__ENCRY_PWR_(1);
	
}

/*
**
*/
static void __Idle1GpioClkEn(void)
{
	__IDLE1_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle2GpioClkEn(void)
{
	__IDLE2_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle3GpioClkEn(void)
{
	__IDLE3_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle4GpioClkEn(void)
{
	__IDLE4_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle5GpioClkEn(void)
{
	__IDLE5_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle6GpioClkEn(void)
{
	__IDLE6_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle7GpioClkEn(void)
{
	__IDLE7_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle8GpioClkEn(void)
{
	__IDLE8_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle9GpioClkEn(void)
{
	__IDLE9_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle10GpioClkEn(void)
{
	__IDLE10_GPIO_CLK_ENABLE();
}

/*
**
*/
static void __Idle1GpioOutLow(void)
{
	__IDLE1_(0);
}

/*
**
*/
static void __Idle2GpioOutLow(void)
{
	__IDLE2_(0);
}

/*
**
*/
static void __Idle3GpioOutLow(void)
{
	__IDLE3_(0);
}

/*
**
*/
static void __Idle4GpioOutLow(void)
{
	__IDLE4_(0);
}

/*
**
*/
static void __Idle5GpioOutLow(void)
{
	__IDLE5_(0);
}

/*
**
*/
static void __Idle6GpioOutLow(void)
{
	__IDLE6_(0);
}

/*
**
*/
static void __Idle7GpioOutLow(void)
{
	__IDLE7_(0);
}

/*
**
*/
static void __Idle8GpioOutLow(void)
{
	__IDLE8_(0);
}

/*
**
*/
static void __Idle9GpioOutLow(void)
{
	__IDLE9_(0);
}

/*
**
*/
static void __Idle10GpioOutLow(void)
{
	__IDLE10_(0);
}
