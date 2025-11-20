#include "includes.h"


/*
**
*/
static void __KeyUp(void);
static void __KeyDown(void);
static void __KeyUpDown(void);
static void __KeyLongUpDown(void);
static void __KeyMoreLongUpDown(void);

/*
**
*/
void HandlerKey(bspKeyValEnum keyVal)
{
	switch(keyVal)
	{
		case BSP_KEY_VAL_UP:
			__KeyUp();
			break;

		case BSP_KEY_VAL_DOWN:
			__KeyDown();
			break;

		case BSP_KEY_VAL_UP_DOWN:
			__KeyUpDown();
			break;

		case BSP_KEY_VAL_LONG_UP_DOWN:	
			__KeyLongUpDown();
			break;
			
		case BSP_KEY_VAL_MORE_LONG_UP_DOWN:	
			__KeyMoreLongUpDown();
			break;

			
		default:
			break;
	}
}

/*
**
*/
static void __KeyUp(void)
{
	#if TRACE_KEY_VAL_EN
		TRACE("KeyUp is Press!\r\n");
	#endif

	BspBuzCall(1, 100, 100);
}

/*
**
*/
static void __KeyDown(void)
{
	#if TRACE_KEY_VAL_EN
		TRACE("KeyDown is Press!\r\n");
	#endif
	BspBuzCall(1, 100, 100);
	BspLedFlashForceSet(BSP_LED_TYPE_RED, 0, 0, 0);
	BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 0, 0, 0);
	BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 0, 0, 0);
	
	// wzh test
	//u08 __buf[18] = {0x00,0x08,0x08,0x6A,0x01,0x00,0x08,0xF0,0x31,0x00,0x08,0x01,0xFF,0xC6,0x0A,0x5F,0x96,0x4A};
	//CommLoraSendPro(__buf,18U);	
	
	//TRACE("id is %08x\r\n", READ_REG(*((u32 *)ID_BASE_ADDR)));
}


/*
**
*/
static void __KeyUpDown(void)
{
	#if TRACE_KEY_VAL_EN
		TRACE("KeyUpDown is Press!\r\n");
	#endif
}

/*
**
*/
static void __KeyLongUpDown(void)
{
	#if TRACE_KEY_VAL_EN
		TRACE("KeyLongUpDown is Press!\r\n");
	#endif
	
	//BspKeyInit();
	BspLedFlashForceSet(BSP_LED_TYPE_RED, 0xFF, 0, 0);
	BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 0xFF, 0, 0);
	BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 0xFF, 0, 0);
		
}

/*
**
*/
static void __KeyMoreLongUpDown(void)
{
	#if TRACE_KEY_VAL_EN
		TRACE("System is Reset By Key!\r\n");
	#endif

	NVIC_SystemReset();
}

	



