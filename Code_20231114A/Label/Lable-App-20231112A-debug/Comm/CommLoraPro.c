#include "includes.h"

/*
**
*/
typedef enum{
	__INQ_TYPE_DEV_TYPE = 0,
	__INQ_TYPE_HW_VER = 1,
	__INQ_TYPE_TEST_VER = 2,
	__INQ_TYPE_RELEASE_VER = 3,
	__INQ_TYPE_DEV_ADDR = 4,
	__INQ_TYPE_WIGHT = 5,
	__INQ_TYPE_OPEN_TIMES = 6,

	__INQ_TYPE_TX_PWR = 9,
}inqTypeEnum;

typedef enum{
	__CTRL_LABLE_CLR_LED        = 0x00,
	__CTRL_LABLE_TYPE_LED        = 0x06,
	__CTRL_LABLE_TYPE_BUZ        = 0x07,
	__CTRL_LABLE_TYPE_CLR_SCREEN = 0x08,
	__CTRL_LABLE_TYPE_RESET      = 0x09,
}ctrlLableTypeEnum;

typedef enum{
	__SET_LABLE_PARA_TYPE_ADDR			  = 0x04,
	__SET_LABLE_PARA_TYPE_LED             = 0x06,
	__SET_LABLE_PARA_TYPE_BUZ             = 0x07,
	__SET_LABLE_PARA_TYPE_WAIT_SLEEP_TIME = 0x08,
	__SET_LABLE_PARA_TYPE_TX_PWR = 0x09,
}setLableParaTypeEnum;


typedef enum{
	BLACK_CHAR = 0x01,
	BLACK_ODC  = 0x02,
	BLACK_QRC  = 0x03,
	BLACK_PIC  = 0x04,
	
	RED_CHAR = 0x11,
	RED_ODC  = 0x12,
	RED_QRC  = 0x13,
	RED_PIC  = 0x14,
}disContextTypeEnum;



/*
**
*/
typedef struct{
	loraProAckCodeEnum cmd; //解码命令
	void (*handler)(u08 *buf, u16 len); //解码函数
}loraProDecodeHandlerSt;

typedef struct{
	loraProAckCodeEnum ackCodeE;
	u08 para[__LORA_PRO_PACKET_MAX_NUM];
}loraProAckSt;

ackCmdToNetSt ackCmdToNetS;
u08 resetByCmdFlag; //wzh
u08 addrChangeFlag; //wzh

/*
**
*/
static void __LoraProRecDatDecode(u08 *buf, u16 len); //LoraPro模块接收数据解码

static void __LoraProAckInqLableInfor(u08 *buf, u16 len);
static void __LoraProAckClrLableLED(u08 *buf, u16 len);
static void __LoraProAckCtrlLable(u08 *buf, u16 len);
static void __LoraProAckSetLablePara(u08 *buf, u16 len);

static void __LoraProAckCtrlLableDis(u08 *buf, u16 len);
static void __LoraProAckUpdateLable(u08 *buf, u16 len);

static void __LoraProAckCmd(u08 *buf, u16 len);
static void __LoraAckNetSendPacket(u08 *buf, u16 len);


//发送数据帧

static const loraProDecodeHandlerSt loraProdecodeHandlerStab[] = { //解码
	{__LORA_PRO_ACK_INQ_LABLE_INFOR,  __LoraProAckInqLableInfor}, 
	{__LORA_PRO_ACK_CTRL_LABLE,       __LoraProAckCtrlLable},
	{__LORA_PRO_ACK_CLR_LABLE_LED,	  __LoraProAckClrLableLED},
	{__LORA_PRO_ACK_SET_LABLE_PARA,   __LoraProAckSetLablePara},//
	{__LORA_PRO_ACK_CTRL_LABLE_DIS,   __LoraProAckCtrlLableDis},		
	{__LORA_PRO_ACK_UPDATE_LABLE,     __LoraProAckUpdateLable},
	{__LORA_PRO_ACK_CMD,              __LoraProAckCmd},	
};

/*
**校验 码
*/
u16 __LoraProCheckCode(u08 *buf, u16 len)
{
	u16 __crc;
	u08 i;

	__crc = 0;
	while (len--)	
	{
		__crc ^= ((*(buf++)) << 8);
		for(i = 0;i < 8;i++)
		{
			if(__crc & 0x8000)
			{
				__crc = (__crc << 1) ^ 0x1021;
			}
			else
			{
				__crc <<= 1;
			}
		}	
	}
	return (__crc) ;
}


/*
**接收云平台数据解码
*/
static void __LoraProRecDatDecode(u08 *buf, u16 len)
{
	u08 i;
	loraProAckSt __loraProAckS;

	
	memcpy((void *)&__loraProAckS, (const void *)buf, len + 1);
	
	for(i = 0; i < sizeof(loraProdecodeHandlerStab) / sizeof(loraProdecodeHandlerStab[0]); i++)
	{
		if(__loraProAckS.ackCodeE == loraProdecodeHandlerStab[i].cmd)
		{
			loraProdecodeHandlerStab[i].handler(__loraProAckS.para, len); //from ack status 
			break;
		}
	}
}

//查询设备基本信息
static void __LoraProAckInqLableInfor(u08 *buf, u16 len)
{
	inqTypeEnum __inqTypeE;
	//u08 __buf[__LORA_PRO_PACKET_MAX_NUM] = {0};

	__inqTypeE = (inqTypeEnum)buf[2];
	OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);

	switch(__inqTypeE)
	{
		case __INQ_TYPE_RELEASE_VER:
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_INQ_LABLE_INFOR;
			__LoraAckNetSendPacket((u08 *)Version, strlen(Version)); 
			break;

		case __INQ_TYPE_TX_PWR:	
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_INQ_LABLE_INFOR;
			__LoraAckNetSendPacket((u08 *)&sx1278S.Settings.LoRa.Power, sizeof(sx1278S.Settings.LoRa.Power)); 
			break;
			
		default:
			break;
	}
}


/*
**
*/
static void __LoraProAckClrLableLED(u08 *buf, u16 len)
{
	ctrlLableTypeEnum __type;
	u16 __onTimes;
	u16 __offTimes;
	u08 __flashTimes;
	u08 __ledIndex;

	#if TRACE_GET_NET_DATA_EN
		u08 i;
		TRACE("get net data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif

	__type = (ctrlLableTypeEnum)buf[2];

	OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);

	//0   1   2   3   4   5   6   7   8
	//00 03 06 02 01 0b b7 00 00
	
	switch(__type)
	{
		case __CTRL_LABLE_CLR_LED:
			__flashTimes =0;
			__onTimes=0;
			__offTimes=0;
			BspLedFlashForceSet(BSP_LED_TYPE_RED, (u32)0, (u32)__onTimes, (u32)__offTimes);
			BspLedFlashForceSet(BSP_LED_TYPE_GREEN, (u32)0, (u32)__onTimes, (u32)__offTimes);
			BspLedFlashForceSet(BSP_LED_TYPE_BLUE, (u32)0, (u32)__onTimes, (u32)__offTimes);
			
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_CLR_LABLE_LED;
			__LoraAckNetSendPacket(buf, 0);		
			
			//TRACE("颜色is: %d, 次数 is: %d, 亮灯 is %d, 灭灯 is %d\r\n",
			      //buf[3], __flashTimes, __onTimes, __offTimes);
			      
			break;
		default:break;
	}
}

/*
**
*/
static void __LoraProAckCtrlLable(u08 *buf, u16 len)
{
	ctrlLableTypeEnum __type;
	u16 __onTimes;
	u16 __offTimes;
	u08 __flashTimes;
	u08 __ledIndex;

	#if TRACE_GET_NET_DATA_EN
		u08 i;
		TRACE("get net data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif

	__type = (ctrlLableTypeEnum)buf[2];

	OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);

	//0   1   2   3   4   5   6   7   8
	//00 03 06 02 01 0b b7 00 00
	
	switch(__type)
	{
		case __CTRL_LABLE_TYPE_LED:
			__ledIndex = buf[3];
			__flashTimes = buf[4];
			OsBufIntoVar16(&__onTimes, &buf[5]);
			OsBufIntoVar16(&__offTimes, &buf[7]); // &buf[6]);  //wzh
			BspLedFlashForceSet((bspLedTypeEnum)__ledIndex, (u32)__flashTimes, (u32)__onTimes, (u32)__offTimes);
			
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_CTRL_LABLE;
			__LoraAckNetSendPacket(buf, 0);		
			
			//TRACE("颜色is: %d, 次数 is: %d, 亮灯 is %d, 灭灯 is %d\r\n",
			      //buf[3], __flashTimes, __onTimes, __offTimes);
			      
			break;

		case __CTRL_LABLE_TYPE_CLR_SCREEN:
			BspLcdWf29Clr(BSP_LCD_DIS_COLOR_TYPE_WB);
//			BspLcdWf29Clr(BSP_LCD_DIS_COLOR_TYPE_WR);
			bspLcdWfS.refreshFlag = BSP_LCD_REFRESH_READY;
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_CTRL_LABLE;
			__LoraAckNetSendPacket(buf, 0); 
			break;

		case __CTRL_LABLE_TYPE_RESET:
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_CTRL_LABLE;
			resetByCmdFlag      = 1;
			__LoraAckNetSendPacket(buf, 0);
			break;
			
		default:	
			break;
	}
}

/*
**
*/
static void __LoraProAckSetLablePara(u08 *buf, u16 len)
{
	setLableParaTypeEnum __type;

	#if TRACE_GET_NET_DATA_EN
		u08 i;
		TRACE("get net data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif
	// 0   1   2   3   4
	// 00 03 04 05 63
	
	__type = (setLableParaTypeEnum)buf[2];
	OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);
	
	switch(__type)
	{
		case __SET_LABLE_PARA_TYPE_WAIT_SLEEP_TIME:
			
			OsBufIntoVar16(&dataSaveS.waitSleepTime, &buf[3]);
			#if TRACE_GET_NET_DATA_EN
				TRACE("dataSaveS.waitSleepTime is %d\r\n", dataSaveS.waitSleepTime);
			#endif	
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_SET_LABLE_PARA;
			__LoraAckNetSendPacket(buf, 0); 	
			break;

		case __SET_LABLE_PARA_TYPE_ADDR:
//						EPD_Reset();                //复位
//			delay(50);
			OsBufIntoVar16(&dataSaveS.addr, &buf[3]);
			dataSaveS.ch = GetChByAddr(dataSaveS.addr >> 8);
			BspLcdWfRefreshAddr();
			bspLcdWfS.refreshFlag = BSP_LCD_REFRESH_READY;
			firstPwrFlag = 0x55;
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_SET_LABLE_PARA;
			__LoraAckNetSendPacket(buf, 0); 
			break;
			
		case __SET_LABLE_PARA_TYPE_TX_PWR:
			dataSaveS.txPwr = buf[3];
			Sx1278SetRfTxPower(dataSaveS.txPwr);
			ackCmdToNetS.ackCmd = __LORA_PRO_ACK_SET_LABLE_PARA;
			__LoraAckNetSendPacket(buf, 0); 
			break;

		default:	
			break;
	}
}

/*
**
*/
static void __LoraProAckCtrlLableDis(u08 *buf, u16 len)
{
	u16 i;

	disContextTypeEnum __disType;
	u16 __disX;
	u16 __disY;
	u16 __disWidth;
	u16 __disHigh;
	
	bspGtCharTypeEnum __charType;
	bspGtCharYangYinWenTypeEnum __yangYinWen;

	u16 __contextLen;
	bspLcdDisColorTypeEnum __disColor;

	#if TRACE_GET_NET_DATA_EN
		TRACE("get lable data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif
	//00 03 01 00 14 00 08 00 00 00 00 00 06 bc db b8 f1 a3 ba 11 00 35 00 08 06 ff 00 00 00 05 31 32 2e 35 30 05 00 14 00 20 00 1e 00 28 00 01 00

	OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);
		
	BspGt21l16S2wAwake(); // wzh
//				EPD_Reset();                //复位
//			delay(50);
	i = 2; 
	while(i < len)
	{
		__disType = (disContextTypeEnum)buf[i];
		OsBufIntoVar16(&__disX, &buf[i + 1]);
		OsBufIntoVar16(&__disY, &buf[i + 3]);
		
		OsBufIntoVar16(&__contextLen, &buf[i + 9]);

		if(__disType & BIT(4))
		{
//			__disColor = BSP_LCD_DIS_COLOR_TYPE_WR;
		}
		else
		{
			__disColor = BSP_LCD_DIS_COLOR_TYPE_WB;
		}
		
		if(1 == (__disType & 0x0F)) //character
		{
			__charType	 = (bspGtCharTypeEnum)buf[i + 5];
			__yangYinWen = (bspGtCharYangYinWenTypeEnum)buf[i + 6];
			BspLcdWf29FillChar(BSP_LCD_DIS_COLOR_TYPE_WB, __charType, BSP_GT_CHAR_YANG_WEN, __disX, __disY, &buf[i + 11], __contextLen);
			i += (11 + __contextLen);
		}
	#if 0
		else if(4 == (__disType & 0x0F)) //picture
		{
			OsBufIntoVar16(&__disWidth, &buf[i + 5]);
			OsBufIntoVar16(&__disHigh, &buf[i + 7]);
			BspLcdWf29FillPic(BSP_LCD_DIS_COLOR_TYPE_WB, __disX, __disY, &buf[i + 11], __disWidth, __disHigh);
			i += (11 + __contextLen);
		}
	#endif
		else if(5 == (__disType & 0x0F)) //fill
		{
			OsBufIntoVar16(&__disWidth, &buf[i + 5]);
			OsBufIntoVar16(&__disHigh, &buf[i + 7]);

			BspLcdWf29FillArea(BSP_LCD_DIS_COLOR_TYPE_WB, __disX, __disY, __disWidth, __disHigh, buf[i + 11]);
			i += (11 + __contextLen);
		}
	}
	BspGt21l16S2wSleep();  //wzh	
	bspLcdWfS.refreshFlag = BSP_LCD_REFRESH_READY;

	ackCmdToNetS.ackCmd = __LORA_PRO_ACK_CTRL_LABLE_DIS;
	__LoraAckNetSendPacket(buf, 0); 
}


/*
**
*/
static void __LoraProAckUpdateLable(u08 *buf, u16 len)
{
  OsBufIntoVar16(&dataSaveS.netAddr, &buf[0]);
	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xff);
	
	ackCmdToNetS.ackCmd = __LORA_PRO_ACK_UPDATE_LABLE;
	//ackCmdToNetS.buf[0] = (u08)__LORA_PRO_CMD_ACK_STA_OK;
	//OsVar16IntoBuf(dataSaveS.addr, &ackCmdToNetS.buf[1]);
	//OsVar16IntoBuf(sx1278S.Settings.LoRaPacketHandler.RssiValue, &ackCmdToNetS.buf[3]);
	//ackCmdToNetS.len	= 5;
	updateParaSaveS.updateFlag = 1;
	__LoraAckNetSendPacket(buf, 0); 
	//SystemResetForUpdate(); //wzh
	//BspIntFlashWrite(FLASH_UPDATE_FLAG_START_ADDR, (u08 *)&updateParaSaveS.updateFlag, sizeof(updateParaSaveS.updateFlag)); //wzh
	BspSaveDataV2(FLASH_UPDATE_FLAG_START_ADDR, 0, (u08 *)&updateParaSaveS.updateFlag, sizeof(updateParaSaveS.updateFlag));
	
	if(gobalAddrFlag) //wzh
	{
		__set_PRIMASK(1);  //关中断；
		NVIC_SystemReset();
	}
}



/*
** f1 00 05 63 00 35
*/
static void __LoraProAckCmd(u08 *buf, u16 len)
{
	loraProAckCodeEnum __cmd;
	
	#if TRACE_GET_NET_DATA_EN
		u08 i;
		TRACE("Get Ack From Net:\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x",  buf[i]);
		}
		TRACE("\r\n");
	#endif
	
	__cmd = (loraProAckCodeEnum)buf[0];
	__ClrLoraResend();
	switch(__cmd)
	{
		case __LORA_PRO_ACK_REG_ADDR:
						break;

		default:
			break;
	}
}

/*
**
*/
void CommLoraProRecDatDecode(u08 *dat, u16 len)
{
	u16 __packLen = 0;
	u16 __ccs = 0;
	u16 i;
	u16 __cnt = 0;

	for(i = 0; i < len; i++)
	{
		if(0 == __cnt) //header1
		{
			if(__LORA_PRO_FRAME_START1 == dat[__cnt])
			{
				__cnt++;
			}
			else
			{
				__cnt = 0;
			}

		} 
		else if(1 == __cnt) //header2
		{
			if(__LORA_PRO_FRAME_START2 == dat[__cnt])
			{
				__cnt++;
			}
			else
			{
				__cnt = 0;
			}
		}
		else if(2 == __cnt) //data lenth MSB
		{
			__packLen = ((u16)dat[__cnt]) << 8;
			__cnt++;
		}
		else if(3 == __cnt) //data lenth LSB
		{
			__packLen |=  (u16)dat[__cnt];

			if(__packLen < __LORA_PRO_PACKET_MAX_NUM)
			{
				__cnt++;
			}
			else
			{
				__cnt = 0;
			}
		}
		else if(4 == __cnt) //command
		{
			__cnt++;
		}
		else if(__cnt < 5 + __packLen) //data parameter
		{
			__cnt++;
		}
		else if(__cnt == 5 + __packLen) //ccs LSB
		{
			__ccs = ((u16)dat[__cnt]) << 8;
			__cnt++;
		}
		else if(__cnt == 6 + __packLen) //ccs MSB
		{
			__ccs |= (u16)dat[__cnt];

			if(__ccs == __LoraProCheckCode(dat, __packLen + 5))
			{
				__LoraProRecDatDecode(&dat[4], __packLen);
			}
			__cnt = 0;
		}
	}
}

/*
**
*/
int look;
static void __LoraAckNetSendPacket(u08 *buf, u16 len)
{
	u08 __buf[__LORA_PACKET_MAX_NUM] = {0};

	if(gobalAddrFlag)
	{
		if(BSP_LCD_REFRESH_READY == bspLcdWfS.refreshFlag)
		{
			bspLcdWfS.refreshFlag = BSP_LCD_REFRESH_NONE;
			bspLcdWfS.times++;
		}
		return;
	}

	ackCmdToNetS.buf[0] = (u08)__LORA_PRO_CMD_ACK_STA_OK;
	OsVar16IntoBuf(dataSaveS.addr, &ackCmdToNetS.buf[1]);
	
	//TRACE("loraStrlen is %d\r\n", sx1278S.Settings.LoRaPacketHandler.RssiValue);
	//TRACE("loraStrlen is %02x\r\n", sx1278S.Settings.LoRaPacketHandler.RssiValue);
	look=sx1278S.Settings.LoRaPacketHandler.RssiValue;
	
	OsVar16IntoBuf(sx1278S.Settings.LoRaPacketHandler.RssiValue, &ackCmdToNetS.buf[3]);
	//OsVar16IntoBuf(BspBatGetVolt(), &ackCmdToNetS.buf[5]);	
	ackCmdToNetS.buf[5] = 0x00;
	ackCmdToNetS.buf[6] = 0x00;
	
	memcpy(&ackCmdToNetS.buf[7], buf, len);

	ackCmdToNetS.len	= 7 + len;


	commLoraCtrlS.step = COMM_LORA_STEP_PC_COMM;

	OsVar16IntoBuf(dataSaveS.netAddr, __buf);
	
	__buf[2] = GetChByAddr(dataSaveS.netCh);

	__buf[3] = __LORA_PRO_FRAME_START1;
	__buf[4] = __LORA_PRO_FRAME_START2;

	__buf[5] = (ackCmdToNetS.len + 1) >> 8;
	__buf[6] = ackCmdToNetS.len + 1;

	__buf[7] = __LORA_PRO_ACK_CMD;

	memcpy(&__buf[8], &ackCmdToNetS.ackCmd, ackCmdToNetS.len + 1);
	OsVar16IntoBuf(__LoraProCheckCode(&__buf[3], ackCmdToNetS.len + 6), &__buf[ackCmdToNetS.len + 9]);
	__LoraSendProData(__buf, ackCmdToNetS.len + 11);
}

