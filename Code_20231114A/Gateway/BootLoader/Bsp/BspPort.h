#ifndef		__BSP_PORT_H_
#define		__BSP_PORT_H_

/*
**
*/
typedef enum{
	BSP_PIN_INPUT = 0,
	BSP_PIN_OUTPUT
}bspPinModEnum;

typedef struct{
	void (*ClkEn)(void);
	GPIO_TypeDef* gpio; //¶Ë¿Ú
	u16 pin;	 //Òý½Å
}gpioPortSt; 


/*
**
*/
void BspPinModeSwitch(GPIO_TypeDef  *gpio, u32 pin, bspPinModEnum mod);

#endif


