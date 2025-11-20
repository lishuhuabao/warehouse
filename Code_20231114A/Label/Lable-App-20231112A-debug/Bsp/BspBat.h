#ifndef __BSP_BAT_H_
#define __BSP_BAT_H_

/*
**
*/
#define BSP_BAT_VOLT_ADC_CHANNEL  ADC_CHANNEL_18

#define BSP_BAT_ADC                     ADC1
#define BSP_BAT_ADC_CLK_EN              __HAL_RCC_ADC1_CLK_ENABLE() 
#define BSP_BAT_ADC_CLK_DIS             __HAL_RCC_ADC1_CLK_DISABLE() 

#define BSP_BAT_VOLT_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE() 
#define BSP_BAT_VOLT_PIN                GPIO_PIN_12
#define BSP_BAT_VOLT_PORT               GPIOB

/*
**
*/
typedef struct{
	u32 volt;
	u08 eleQuantity;
}bspBatSt;

extern bspBatSt bspBatS;
extern bspBatSt preBspBatS;


/*
**
*/
void BspBatInit(void);
u16 BspBatGetVolt(void);
void BspBatGetVoltEvent(void);
void BspBatAdcPortInit(void);
void BspBatAdcPortDeInit(void);
void BspBatCheckSleep(void);
void BspBatCheckAwake(void);

#endif

