#ifndef __BSP_LCD_WF29_H_
#define __BSP_LCD_WF29_H_

/*
**
*/
//#define BSP_LCD_WF_BUSY_IRQ_HANDLER   EXTI9_5_IRQHandler   // wzh
#define BSP_LCD_WF29_BUSY_PIN         GPIO_PIN_9


typedef enum{
	BSP_LCD_DIS_COLOR_TYPE_WB = 0,
	BSP_LCD_DIS_COLOR_TYPE_WR,
}bspLcdDisColorTypeEnum;

typedef enum{
	BSP_LCD_REFRESH_NONE = 0,
	BSP_LCD_REFRESH_READY,
	BSP_LCD_REFRESH_START
}bspLcdRefreshEnmum;

/*
**
*/
typedef struct{
	u16 cnt;
	u08 step;
	u16 times;
	bspLcdRefreshEnmum refreshFlag;
}bspLcdWfSt;

extern bspLcdWfSt bspLcdWfS;
extern const char Version[];

/*
**
*/

void BspLcdWf29Test(void);
void BspLcdWf29ShowPic(bspLcdDisColorTypeEnum type);
void BspLcdWf29FillChar(bspLcdDisColorTypeEnum color, bspGtCharTypeEnum charType, bspGtCharYangYinWenTypeEnum yangYin, u16 x, u16 y, const u08 *charCode, u16 len);
void BspLcdWf29FillPic(bspLcdDisColorTypeEnum color, u16 x, u16 y, const u08 *pic, u16 width, u16 high);
void BspLcdWf29FillArea(bspLcdDisColorTypeEnum color, u16 x, u16 y, u16 width, u16 high, u08 dat);
void BspLcdWf29Init(void);
void BspLcdWf29Clr(bspLcdDisColorTypeEnum type);
void BspRefreshDisAddr(void);
void BspLcdRefreshEvent(void);
void BspLcdWfBusyEint(void);
void BspLcdWfBusyNoEint(void);
void BspLcdWfRefreshAddr(void);

#endif

