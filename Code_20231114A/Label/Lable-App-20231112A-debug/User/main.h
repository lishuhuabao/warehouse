#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/*
**
*/
extern  u08 rstRes;
extern const char DisplayVersion[];
extern const char Version[];
/*
**
*/
void _Error_Handler(char *, int);
void SystemClock_Config(void);
void GetResetReasion(void);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)



#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


