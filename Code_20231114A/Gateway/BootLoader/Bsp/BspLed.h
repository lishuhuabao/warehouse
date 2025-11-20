#ifndef __BSP_LED_H_
#define __BSP_LED_H_

/*
**
*/
typedef enum{
	BSP_LED_TYPE_PC_TXD = 0, //
	BSP_LED_TYPE_PC_RXD,//
	BSP_LED_TYPE_WIFI_TXD, //
	BSP_LED_TYPE_WIFI_RXD,//
	BSP_LED_TYPE_LORA_TXD, //
	BSP_LED_TYPE_LORA_RXD,//	
	BSP_LED_TYPE_MAX //
}bspLedTypeEnum;

typedef enum{
	BSP_LED_STA_ON = 0, //LED ÁÁ
	BSP_LED_STA_OFF,    //LEDÃð
	BSP_LED_STA_TOGGLE  //LED×´Ì¬·­×ª
}bspLedStaEnum;

/*
**
*/
void BspLedInit(void); //LED³õÊ¼»¯
void BspLedFlashSet(bspLedTypeEnum led, u08 times, u16 on, u16 off);
void BspLedEvent(void);

#endif


