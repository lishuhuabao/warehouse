#include "includes.h"

u08 loraRecFlag;

commLoraCtrlSt commLoraCtrlS;
loraSendPackListSt loraSendPackListStab[10];
netSetStaEnum netSetStaE;
u08 gobalAddrFlag;


#define __LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_GREEN_PIN                    GPIO_PIN_5
#define __LED_GREEN_PORT                   GPIOB
#define __LED_GREEN_(x)                    HAL_GPIO_WritePin(__LED_GREEN_PORT, __LED_GREEN_PIN, (GPIO_PinState)(x)) 
#define __LED_GREEN_TOGGLE()               HAL_GPIO_TogglePin(__LED_GREEN_PORT, __LED_GREEN_PIN) 
/*
**接收Lora模块数据处理
*/
void CommLoraRecDatHandler(void)
{
	#if TRACE_REC_DAT_LORA_AS_HEX_EN
		u16 i;
	#endif
	__LED_GREEN_(1);

	if(0 == loraRecFlag)
	{
		return;
	}

	HAL_Delay(20);
	__LED_GREEN_(0);
	loraRecFlag = 0;
	
	if( COMM_LORA_STEP_UPDATA == commLoraCtrlS.step )
	{
			UpdateDataProcess(&RxTxBuffer[3], sx1278S.Settings.LoRaPacketHandler.Size - 3);
			usr.waitTimes = 0; //ClrPowerEnterTime();
	}

//  if( COMM_LORA_STEP_PC_COMM == commLoraCtrlS.step )
//   {
//	    #if TRACE_REC_DAT_LORA_AS_HEX_EN
//		  TRACE("rec data from lora is %d\r\n", sx1278S.Settings.LoRaPacketHandler.Size);
//		  for(i = 0; i < sx1278S.Settings.LoRaPacketHandler.Size; i++)
//		  {
//		  	TRACE(" %02x", RxTxBuffer[i]);
//	   	}
//	   	TRACE("\r\n");
//	    #endif
//
//     CommLoraProRecDatDecode(&RxTxBuffer[3], sx1278S.Settings.LoRaPacketHandler.Size - 3);
//      ClrPowerEnterTime();
//  }
	
}


u08 GetChByAddr(u08 addr)
{
	return(addr % 128);
}





