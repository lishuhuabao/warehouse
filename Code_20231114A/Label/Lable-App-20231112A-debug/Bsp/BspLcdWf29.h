#ifndef __BSP_LCD_WF29_H_
#define __BSP_LCD_WF29_H_

/*
**
*/
//#define BSP_LCD_WF_BUSY_IRQ_HANDLER   EXTI9_5_IRQHandler   // wzh
#define BSP_LCD_WF29_BUSY_PIN         GPIO_PIN_9
#define EPD_W21_SPI_SPEED     2

typedef enum{
	BSP_LCD_DIS_COLOR_TYPE_WB = 0,
	BSP_LCD_DIS_COLOR_TYPE_WR ,
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


/*-----------------------------new---------------------------------------------------*/
void EPD_W21_WriteCMD(unsigned char command);
void EPD_W21_WriteDATA(unsigned char data);
void SPI_Write(unsigned char byte) ;
void EPD_Reset(void);
void EPD_init(void);
void lut_GC(void);
void lcd_chkstatus(void);
void EPD_refresh(void);
void PIC_display(unsigned char NUM);
void PIC_display_GC(const unsigned char* picData);
void SPI_Delay(unsigned char xrate);
void lut_DU(void);
void PIC_display_DU(const unsigned char* picData);
void EPD_sleep(void);
void PIC_Image(const unsigned char* picData);


//颜色，字体,x坐标，y坐标，显示内容，长度
void new_LcdWf29FillChar(bspLcdDisColorTypeEnum color, bspGtCharTypeEnum charType, u16 x, u16 y, const u08 *charCode, u16 len);

#define PIC_WHITE 0x01
#define PIC_BLACK 0x02
#define PIC_Source_Line 0x03
#define PIC_Gate_Line 0x04
#define PIC_Chessboard 0x05
#define PIC_LEFT_BLACK_RIGHT_WHITE 0x06
#define PIC_UP_BLACK_DOWN_WHITE 0x07
#define PIC_Frame 0x08
#define PIC_DotInv 0x09
#define PIC_Crosstalk 0x10

#endif

