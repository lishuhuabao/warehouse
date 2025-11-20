#ifndef __BSP_LED_H_
#define __BSP_LED_H_

/*
**
*/
typedef enum{
	BSP_LED_TYPE_RED = 0,//
	BSP_LED_TYPE_GREEN, //
	BSP_LED_TYPE_BLUE,//
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
extern bspLedStaEnum bspLedSta[BSP_LED_TYPE_MAX];


/*
**
*/
void BspLedInit(void); //LED³õÊ¼»¯
void BspLedFlashSet(bspLedTypeEnum led, u32 times, u32 on, u32 off);
void BspLedFlashForceSet(bspLedTypeEnum led, u32 times, u32 on, u32 off);
void BspLedFlashForceReset(bspLedTypeEnum led);
void BspLedEvent(void);
void BspLedTest(void);
#endif


