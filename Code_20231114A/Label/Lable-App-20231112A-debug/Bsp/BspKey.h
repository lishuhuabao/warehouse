#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

/*
**
*/
typedef enum{
	BSP_KEY_ID_UP = 0,
	BSP_KEY_ID_DOWN,
	BSP_KEY_MAX
}bspKeyIdEnum;

typedef enum{
	BSP_KEY_VAL_NONE    = 0,
	BSP_KEY_VAL_UP	    = (1 << BSP_KEY_ID_UP),
	BSP_KEY_VAL_DOWN	= (1 << BSP_KEY_ID_DOWN),
	BSP_KEY_VAL_UP_DOWN = ((1 << BSP_KEY_ID_UP) | (1 << BSP_KEY_ID_DOWN)),
	BSP_KEY_VAL_LONG_UP_DOWN      = (0x0100 | (1 << BSP_KEY_ID_UP) | (1 << BSP_KEY_ID_DOWN)),//0xF000
	BSP_KEY_VAL_MORE_LONG_UP_DOWN = (0x0F00 | (1 << BSP_KEY_ID_UP) | (1 << BSP_KEY_ID_DOWN)),//0xFF00
}bspKeyValEnum;

typedef enum{
	SW_INPUT_STA_UNPRESS = 0,
	SW_INPUT_STA_PRESSED,
}keyStaEnum;

#define BSP_KEY_UP_PIN                      GPIO_PIN_1
#define BSP_KEY_DOWN_PIN                    GPIO_PIN_2


#define BSP_KEY_UP_EXIT_IRQ                 EXTI1_IRQn
#define BSP_KEY_DOWN_EXIT_IRQ               EXTI2_IRQn

#define BSP_KEY_UP_EXIT_IRQ_PRE_LEV      0
#define BSP_KEY_UP_EXIT_IRQ_SUB_LEV      0
#define BSP_KEY_UP_IRQ_HANDLER           EXTI1_IRQHandler

#define BSP_KEY_DOWN_EXIT_IRQ_PRE_LEV      0
#define BSP_KEY_DOWN_EXIT_IRQ_SUB_LEV      0
#define BSP_KEY_DOWN_IRQ_HANDLER           EXTI2_IRQHandler



/*
**
*/
void BspKeyInit(void);
void BspKeyEvent(void);


#endif


