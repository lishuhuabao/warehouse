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

int key1_test;
int key2_test;
extern ackCmdToNetSt ackCmdToNetS;
void HandlerKey(bspKeyValEnum keyVal)
{
	switch (keyVal)
	{
	case BSP_KEY_VAL_UP:
		__KeyUp();
		// TimerStop(2);		   // Rx over
		// TimerStop(1);		   // Tx over
		// TimerStop(3);		   // CAD over
		Sx1276TimerStart(1);   // #define TX_TIMEOUT_TIMER  1
		SX1276SetOpMode(0x02); // Tx mode
		// ———————————按键返回指令设置————————————//
		ackCmdToNetS.buf[0] = 0x00;
		OsVar16IntoBuf(dataSaveS.addr, &ackCmdToNetS.buf[1]);
		ackCmdToNetS.buf[2] = GetChByAddr(dataSaveS.netCh);
		ackCmdToNetS.buf[3] = 0x6A;
		ackCmdToNetS.buf[4] = 0x01;
		ackCmdToNetS.buf[5] = 0x00;
		ackCmdToNetS.buf[6] = 0x08;
		ackCmdToNetS.buf[7] = 0xF0; // 应答返回
		ackCmdToNetS.buf[8] = 0x33;
		ackCmdToNetS.buf[9] = 0x80;							  // 其他错误返回
		ackCmdToNetS.buf[10] = ((dataSaveS.addr >> 8) & 0x00FF); // 地址
		ackCmdToNetS.buf[11] = dataSaveS.addr & 0x00FF;
		ackCmdToNetS.buf[12] = 0xFF;
		ackCmdToNetS.buf[13] = 0xCA;
		ackCmdToNetS.buf[14] = 0x00;
		ackCmdToNetS.buf[15] = 0x00;
		ackCmdToNetS.buf[16] = ((__LoraProCheckCode(ackCmdToNetS.buf + 3, 13) >> 8) & 0x00FF); // 生成校验码
		ackCmdToNetS.buf[17] = (__LoraProCheckCode(ackCmdToNetS.buf + 3, 13) & 0x00FF);		 // 将ackCmdToNetS.buf生成的校验码赋给ackCmdToNetS.buf
		// ———————————按键返回指令设置————————————//
		BspLoraSendPacket(ackCmdToNetS.buf, 18);
		//			__LoraSendProData(ackCmdToNetS.buf, ackCmdToNetS.len + 18);//Lora发送指令函数
		SX1276SetOpMode(RF_OPMODE_SLEEP);
		TimerStop(1); // Tx over
		Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
				sx1278S.Settings.State = RF_IDLE;
		// OnTxDone();
		// SX1276StartCad();
		// TaskStart(TASK_START_CAD);
		//			SX1276Init();
		//			OnTxDone();
		//			SX1276StartCad();
		//			test_1 = 0;
		break;

	case BSP_KEY_VAL_DOWN:
		__KeyDown();
		// TimerStop(2);		   // Rx over
		// TimerStop(1);		   // Tx over
		// TimerStop(3);		   // CAD over
		Sx1276TimerStart(1);   // #define TX_TIMEOUT_TIMER  1
		SX1276SetOpMode(0x02); // Tx mode

		ackCmdToNetS.buf[0] = 0x00;
		OsVar16IntoBuf(dataSaveS.addr, &ackCmdToNetS.buf[1]);
		ackCmdToNetS.buf[2] = GetChByAddr(dataSaveS.netCh);
		ackCmdToNetS.buf[3] = 0x6A;
		ackCmdToNetS.buf[4] = 0x01;
		ackCmdToNetS.buf[5] = 0x00;
		ackCmdToNetS.buf[6] = 0x08;
		ackCmdToNetS.buf[7] = 0xF0; // 应答返回
		ackCmdToNetS.buf[8] = 0x34;
		ackCmdToNetS.buf[9] = 0x00;							  // 标签板正确执行指令返回
		ackCmdToNetS.buf[10] = ((dataSaveS.addr >> 8) & 0x00FF); // 地址
		ackCmdToNetS.buf[11] = dataSaveS.addr & 0x00FF;
		ackCmdToNetS.buf[12] = 0xFF;
		ackCmdToNetS.buf[13] = 0xCA;
		ackCmdToNetS.buf[14] = 0x00;
		ackCmdToNetS.buf[15] = 0x00;
		ackCmdToNetS.buf[16] = ((__LoraProCheckCode(ackCmdToNetS.buf + 3, 13) >> 8) & 0x00FF); // 校验码
		ackCmdToNetS.buf[17] = (__LoraProCheckCode(ackCmdToNetS.buf + 3, 13) & 0x00FF);
		BspLoraSendPacket(ackCmdToNetS.buf, 18);
		//			__LoraSendProData(ackCmdToNetS.buf, ackCmdToNetS.len + 18);//Lora发送指令函数
		SX1276SetOpMode(RF_OPMODE_SLEEP);
		TimerStop(1); // Tx over
		Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
				sx1278S.Settings.State = RF_IDLE;
		// OnTxDone();
		// SX1276StartCad();
		// TaskStart(TASK_START_CAD);
		//			__LoraSendProData(ackCmdToNetS.buf, ackCmdToNetS.len + 18);
		//			SX1276SetOpMode( RF_OPMODE_SLEEP );
		//			OnTxDone();
		//			SX1276StartCad();
		//			test_1 = 0;
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
int key_test1;
static void __KeyUp(void)
{
#if TRACE_KEY_VAL_EN
	TRACE("KeyUp is Press!\r\n");
#endif
	key_test1 = 1;
	BspBuzCall(1, 100, 100);
}

/*
**
*/
int key_test2;
static void __KeyDown(void)
{
#if TRACE_KEY_VAL_EN
	TRACE("KeyDown is Press!\r\n");
#endif
	key_test2 = 1;
	BspBuzCall(1, 100, 100);
//	BspLedFlashForceSet(BSP_LED_TYPE_RED, 0, 0, 0);
//	BspLedFlashForceSet(BSP_LED_TYPE_GREEN, 0, 0, 0);
//	BspLedFlashForceSet(BSP_LED_TYPE_BLUE, 0, 0, 0);

	// wzh test
	// u08 __buf[18] = {0x00,0x08,0x08,0x6A,0x01,0x00,0x08,0xF0,0x31,0x00,0x08,0x01,0xFF,0xC6,0x0A,0x5F,0x96,0x4A};
	// CommLoraSendPro(__buf,18U);

	// TRACE("id is %08x\r\n", READ_REG(*((u32 *)ID_BASE_ADDR)));
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

	// BspKeyInit();
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
