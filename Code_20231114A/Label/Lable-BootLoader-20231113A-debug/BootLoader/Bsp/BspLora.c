#include "includes.h"

//398+信道号就是此时的频率，单位Mhz

/*
**
*/
#define BUF_LEN 255

/*
**
*/
SPI_HandleTypeDef loraSpiHandler;



/* LoRa */
u16 LoRaBufSize;			// RF buffer size
u08 LoRaBuffer[BUF_LEN];	// RF buffer
u32 tes;
loraSendPacketSt loraSendPacketS;
//u16 netAddr;
//u08 netCh;



/*
**
*/
static void __LoraPortInit(void);

void OnMaster(void);
void OnSlave(void);


/*
**
*/
void BspLoraUartSendPacket(u08 *buf, u16 len)
{

}

/*
**
*/
void BspLoraInit(void)
{
	__LoraPortInit();
	
	loraSpiHandler.Instance               = BSP_LORA_SPI;
	loraSpiHandler.Init.Mode              = SPI_MODE_MASTER;
	loraSpiHandler.Init.Direction         = SPI_DIRECTION_2LINES;
	loraSpiHandler.Init.DataSize          = SPI_DATASIZE_8BIT;
	loraSpiHandler.Init.CLKPolarity       = SPI_POLARITY_LOW;
	loraSpiHandler.Init.CLKPhase          = SPI_PHASE_1EDGE;
	loraSpiHandler.Init.NSS               = SPI_NSS_SOFT;
	loraSpiHandler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // wzhSPI_BAUDRATEPRESCALER_16; //
	loraSpiHandler.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	loraSpiHandler.Init.TIMode            = SPI_TIMODE_DISABLE;
	loraSpiHandler.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	loraSpiHandler.Init.CRCPolynomial     = 7;
	if (HAL_SPI_Init(&loraSpiHandler) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_SPI_ENABLE(&loraSpiHandler);  //使能SPI
	
	commLoraCtrlS.step = COMM_LORA_STEP_PC_COMM;
  SX1276AppInit();
}


/*
**
*/
static void __LoraPortInit(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__LORA_RST_GPIO_CLK_ENABLE();
	
	__LORA_RST_(1);

	__gpioInitSt.Pin       = __LORA_RST_PIN;
	__gpioInitSt.Mode      = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull      = GPIO_NOPULL;
	__gpioInitSt.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(__LORA_RST_PORT, &__gpioInitSt);

	__LORA_CS_GPIO_CLK_ENABLE();
	
	BSP_LORA_CS_(1);

	__gpioInitSt.Pin       = __LORA_CS_PIN;
	__gpioInitSt.Mode      = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull      = GPIO_NOPULL;
	__gpioInitSt.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(__LORA_CS_PORT, &__gpioInitSt);

	__LORA_DIO0_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin	   = BSP_LORA_DIO0_PIN;
	__gpioInitSt.Mode	   = GPIO_MODE_IT_RISING;
	__gpioInitSt.Pull	   = GPIO_PULLDOWN;
	__gpioInitSt.Speed	 = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(__LORA_DIO0_PORT, &__gpioInitSt);

  HAL_NVIC_SetPriority(__LORA_DIO0_EXIT_IRQ, __LORA_DIO0_EXIT_IRQ_PRE_LEV, __LORA_DIO0_EXIT_IRQ_SUB_LEV);      
  HAL_NVIC_EnableIRQ(__LORA_DIO0_EXIT_IRQ);             //使能中断线0
	
	//wzh
	__LORA_DIO3_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin	   = BSP_LORA_DIO3_PIN;
	__gpioInitSt.Mode	   = GPIO_MODE_IT_RISING;
	__gpioInitSt.Pull	   = GPIO_PULLDOWN;
	__gpioInitSt.Speed	 = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(__LORA_DIO3_PORT, &__gpioInitSt);

  HAL_NVIC_SetPriority(__LORA_DIO3_EXIT_IRQ, __LORA_DIO3_EXIT_IRQ_PRE_LEV, __LORA_DIO3_EXIT_IRQ_SUB_LEV);      
  HAL_NVIC_EnableIRQ(__LORA_DIO3_EXIT_IRQ);             //使能中断线0
  
}

/*
**
*/
void BspLoraChSet(u08 ch)	
{
	SX1276SetFreq((398 + (u32)ch) * 1000000);
	//SX1276SetFreq(398500000 + ((u32)ch) * 1000000);  //wzh

	#if TRACE_LORA_NOW_FREQ_EN
		TRACE("lora is %d\r\n", SX1276LoRaGetRFFrequency());
	#endif
}

/*
**
*/
u08 BspLoraSpITransmit( u08 outData )
{
    u08 txData = outData;
    u08 rxData = 0;
    HAL_SPI_TransmitReceive(&loraSpiHandler, &txData, &rxData, 1, 100);
    return rxData;
}


/*
**
*/
void BspLoraSendPacket(u08 *buf, u16 len)
{
	memcpy(loraSendPacketS.buf, buf, len);
	loraSendPacketS.len = len;
	TaskStart(TASK_SEND_DATA);//启动SEND任务
}

#define __LED_RED_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_RED_PIN                    GPIO_PIN_10
#define __LED_RED_PORT                   GPIOB
#define __LED_RED_(x)                    HAL_GPIO_WritePin(__LED_RED_PORT, __LED_RED_PIN, (GPIO_PinState)(x)) 
#define __LED_RED_TOGGLE()               HAL_GPIO_TogglePin(__LED_RED_PORT, __LED_RED_PIN) 

#define __LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_GREEN_PIN                    GPIO_PIN_5
#define __LED_GREEN_PORT                   GPIOB
#define __LED_GREEN_(x)                    HAL_GPIO_WritePin(__LED_GREEN_PORT, __LED_GREEN_PIN, (GPIO_PinState)(x)) 
#define __LED_GREEN_TOGGLE()               HAL_GPIO_TogglePin(__LED_GREEN_PORT, __LED_GREEN_PIN) 

#define __LED_BLUE_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_BLUE_PIN                    GPIO_PIN_4
#define __LED_BLUE_PORT                   GPIOB
#define __LED_BLUE_(x)                    HAL_GPIO_WritePin(__LED_BLUE_PORT, __LED_BLUE_PIN, (GPIO_PinState)(x)) 
#define __LED_BLUE_TOGGLE()               HAL_GPIO_TogglePin(__LED_BLUE_PORT, __LED_BLUE_PIN) 
void BspIapUartSendByte(u08 dat)
{
	u08 __buf[4] = {0};
	
	if(gobalAddrFlag)
	{
		loraSendFlag = false;
		if (dat == NAK)
		{
			__UpdateResHandler(UPDATE_ERR);
		}
		return;
	}
	__LED_RED_(1);
	HAL_Delay(20);
	__LED_RED_(0);
	
//	OsVar16IntoBuf(dataSaveS.addr, __buf);
//	OsVar16IntoBuf(0x0006, __buf);
	
	__buf[0] = 0;
	__buf[1] = dataSaveS.ch;
	__buf[2] = dataSaveS.ch;
	
	__buf[3] = dat;

	BspLoraSendPacket(__buf, sizeof(__buf));
}

