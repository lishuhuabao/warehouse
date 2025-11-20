#ifndef __BSP_GT21L16S2W_H_
#define __BSP_GT21L16S2W_H_

/*
**
*/
#define BSP_ZK_GT_SPI                        SPI1
#define BSP_ZK_GT_SPI_CLK_EN()               __HAL_RCC_SPI1_CLK_ENABLE()
#define BSP_ZK_GT_SPI_CLK_DIS()              __HAL_RCC_SPI1_CLK_DISABLE()


#define BSP_ZK_GT_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_ZK_GT_SCK_PIN                    GPIO_PIN_5
#define BSP_ZK_GT_SCK_PORT                   GPIOA
#define BSP_ZK_GT_SCK_AF                     GPIO_AF5_SPI1
#define BSP_ZK_GT_SCK_(x)                    HAL_GPIO_WritePin(BSP_ZK_GT_SCK_PORT, BSP_ZK_GT_SCK_PIN, (GPIO_PinState)(x))


#define BSP_ZK_GT_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_ZK_GT_MISO_PIN                    GPIO_PIN_6
#define BSP_ZK_GT_MISO_PORT                   GPIOA
#define BSP_ZK_GT_MISO_AF                     GPIO_AF5_SPI1
#define BSP_ZK_GT_MISO_(x)                    HAL_GPIO_WritePin(BSP_ZK_GT_MISO_PORT, BSP_ZK_GT_MISO_PIN, (GPIO_PinState)(x))


#define BSP_ZK_GT_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_ZK_GT_MOSI_PIN                    GPIO_PIN_7
#define BSP_ZK_GT_MOSI_PORT                   GPIOA
#define BSP_ZK_GT_MOSI_AF                     GPIO_AF5_SPI1
#define BSP_ZK_GT_MOSI_(x)                    HAL_GPIO_WritePin(BSP_ZK_GT_MOSI_PORT, BSP_ZK_GT_MOSI_PIN, (GPIO_PinState)(x))


#define BSP_ZK_GT_PWR_CTRL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_ZK_GT_PWR_CTRL_PIN                    GPIO_PIN_4
#define BSP_ZK_GT_PWR_CTRL_PORT                   GPIOA
#define BSP_ZK_GT_PWR_CTRL_(x)                    HAL_GPIO_WritePin(BSP_ZK_GT_PWR_CTRL_PORT, BSP_ZK_GT_PWR_CTRL_PIN, (GPIO_PinState)(x))

typedef enum{
	BSP_GT_CHAR_TYPE_HZ_11X12 = 0,
	BSP_GT_CHAR_TYPE_HZ_15X16,
	BSP_GT_CHAR_TYPE_HZ_6X12,
	BSP_GT_CHAR_TYPE_HZ_8X16,
	BSP_GT_CHAR_TYPE_ASC_5X7,
	BSP_GT_CHAR_TYPE_ASC_7X8,
	BSP_GT_CHAR_TYPE_ASC_6X12,
	BSP_GT_CHAR_TYPE_ASC_8X16,
	BSP_GT_CHAR_TYPE_ASC_16X32,
	//BSP_GT_CHAR_TYPE_ASC_12_ARIAL,
	//BSP_GT_CHAR_TYPE_ASC_16_ARIAL,
	BSP_GT_CHAR_TYPE_MAX,
}bspGtCharTypeEnum;

typedef enum{
	BSP_GT_CHAR_YANG_WEN = 0xFF,
	BSP_GT_CHAR_YIN_WEN = 0x00,
}bspGtCharYangYinWenTypeEnum;


/*
**
*/
typedef struct{
	bspGtCharTypeEnum type;
	void (* gtGetImageFunc)(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin);
	u08 codeLen;
	u08 disWidth;
	u08 disHigh;
}bspGtCharSt;


extern const bspGtCharSt bspGtCharStab[];

/*
**
*/
void BspGt21l16S2wInit(void);
void BspGtGetHz11x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); //11*16
void BspGtGetHz15x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); //15*16
void BspGtGetHz6x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); //6*16
void BspGtGetHz8x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); //8*16
void BspGtGetAsc5x7Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); // 5 * 8
void BspGtGetAsc7x8Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); // 7 * 8
void BspGtGetAsc6x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); // 6 * 16
void BspGtGetAsc8x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); // 8* 16
void BspGtGetAsc16x32Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin); // 8* 16

void BspGt21l16S2wSleep(void);
void BspGt21l16S2wAwake(void);






#endif

