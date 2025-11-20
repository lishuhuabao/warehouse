#ifndef __BSP_ADC_H_
#define __BSP_ADC_H_

/*
**
*/
extern ADC_HandleTypeDef adcHandler; 

/*
**
*/
void BspAdcInit(void);
u16 BspGetAdc(u32 ch);   

#endif

