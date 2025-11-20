#include "includes.h"

/*
**
*/ 
#define SYSTEM_CLOCK  48000000 //系统主频 48MHz
#define SIM_BAUD      460800   //模拟UART 波特率 460800

#if    48000000 == SYSTEM_CLOCK
	#if 460800 == SIM_BAUD
		#define SIM_BUAD_START_DELAY 		10
		#define SIM_BUAD_DATA_DELAY			10
		#define SIM_BUAD_STOP_DELAY			10
	#endif
#endif

//模拟TXD引脚
#define __SIM_TXD_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define __SIM_TXD_PIN                GPIO_PIN_6
#define __SIM_TXD_PORT               GPIOB

/*
**
*/


/*
**
*/
static void __SimUartDelay(u08 tt); //模拟UART延时
static void __SimTxdLow(void);//模拟TXD电平置0
static void __SimTxdHigh(void);//模拟TXD电平置1
static void (* __SimTxdLev[])(void) = {
	__SimTxdLow,
	__SimTxdHigh,
};



/*
**模拟UART初始化
*/
void BspSimUartInit(void)
{
	GPIO_InitTypeDef __gpioInitSt;

	/* Clock Enable */
	__SIM_TXD_GPIO_CLK_ENABLE();
	
	__SIM_TXD_PORT->BSRR = (u32)__SIM_TXD_PIN;
	
	//configure  pin as output without pull-up and push-down  resiter,
	//high port speed;
	__gpioInitSt.Pin   = __SIM_TXD_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull  = GPIO_NOPULL;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__SIM_TXD_PORT, &__gpioInitSt);	
}

/*
**模拟UART发送一个字节
*/
void BspSimUartSendByte(char dat)
{
	__disable_irq();
	
	__SimTxdLev[1]();
	__SimUartDelay(SIM_BUAD_START_DELAY);
	
	__SimTxdLev[0]();
    __SimUartDelay(SIM_BUAD_START_DELAY); //start
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
	__SimTxdLev[dat & 0X01](); //bit 0
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 1
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();
	
	__SimTxdLev[dat & 0X01](); //bit 2
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 3
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 4
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 5
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 6
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();

	__SimTxdLev[dat & 0X01](); //bit 7
	dat >>= 1;
	__SimUartDelay(SIM_BUAD_DATA_DELAY);
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	__SimTxdLev[1]();
    __SimUartDelay(SIM_BUAD_STOP_DELAY);
    __NOP();

    __SimTxdLev[1]();
    __SimUartDelay(SIM_BUAD_STOP_DELAY);
    __NOP();
    
	__enable_irq();
}


/*
**模拟UART延时
*/
static void __SimUartDelay(u08 tt)
{
	while(tt--)
	{
		 ;
	}
}

/*
**模拟TXD置0
*/
static void __SimTxdLow(void)
{
	__SIM_TXD_PORT->BRR = (u32)__SIM_TXD_PIN;
}

/*
**模拟TXD置1
*/
static void __SimTxdHigh(void)
{
	__SIM_TXD_PORT->BSRR = (u32)__SIM_TXD_PIN;
}


