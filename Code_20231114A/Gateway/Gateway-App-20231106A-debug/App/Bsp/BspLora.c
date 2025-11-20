#include "includes.h"

/*
**
*/
SPI_HandleTypeDef loraSpiHandler;
loraSendPacketSt loraSendPacketS;


/*
**
*/
static void __LoraPortInit(void);

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
	loraSpiHandler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	loraSpiHandler.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	loraSpiHandler.Init.TIMode            = SPI_TIMODE_DISABLE;
	loraSpiHandler.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	loraSpiHandler.Init.CRCPolynomial     = 7;
	if (HAL_SPI_Init(&loraSpiHandler) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_SPI_ENABLE(&loraSpiHandler);  //使能SPI

	BspLoraHwReset();//wzh
	SX1276AppInit();
}


/*
**
*/
static void __LoraPortInit(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};

	__LORA_RST_GPIO_CLK_ENABLE();
	
	__LORA_RST_(1);

	__gpioInitSt.Pin       = __LORA_RST_PIN;
	__gpioInitSt.Mode      = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull      = GPIO_NOPULL;
	__gpioInitSt.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(__LORA_RST_PORT, &__gpioInitSt);

	__LORA_CS_GPIO_CLK_ENABLE();
	
	__LORA_CS_(1);

	__gpioInitSt.Pin       = __LORA_CS_PIN;
	__gpioInitSt.Mode      = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Pull      = GPIO_NOPULL;
	__gpioInitSt.Speed     = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(__LORA_CS_PORT, &__gpioInitSt);

	__LORA_DIO0_GPIO_CLK_ENABLE();

	__gpioInitSt.Pin	   = BSP_LORA_DIO0_PIN;
	__gpioInitSt.Mode	   = GPIO_MODE_IT_RISING;
	__gpioInitSt.Pull	   = GPIO_PULLDOWN;
	__gpioInitSt.Speed	   = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(__LORA_DIO0_PORT, &__gpioInitSt);

    HAL_NVIC_SetPriority(__LORA_DIO0_EXIT_IRQ, __LORA_DIO0_EXIT_IRQ_PRE_LEV, __LORA_DIO0_EXIT_IRQ_SUB_LEV);      
    HAL_NVIC_EnableIRQ(__LORA_DIO0_EXIT_IRQ);             //使能中断线0
}



/*
**
*/
void BspLoraHwReset(void)
{
	commLoraCtrlS.step      = COMM_LORA_STEP_PC_COMM;//wzh COMM_LORA_STEP_INIT;
	memset(&loraSendPackListStab, 0, sizeof(loraSendPackListStab));

}

/*
**
*/
u08 SpiInOut(u08 InputData)
{
	u08 Rxdata;

	HAL_SPI_TransmitReceive(&loraSpiHandler,&InputData,&Rxdata,1, 1000); 

	return Rxdata;	
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
void BspLoraSendPacket(u08 *buf, u16 len)
{
	memcpy(loraSendPacketS.buf, buf, len);
	loraSendPacketS.len = len;
	spTaskStart(TASK_SEND_DATA);//启动SEND任务
}

/*
**
*/
u32 BspLoraGetFreqByCh(u08 ch)
{
	 return ((398 + (u32)ch) * 1000000);
	//return (398500000 + ((u32)ch) * 1000000);  //wzh
}

