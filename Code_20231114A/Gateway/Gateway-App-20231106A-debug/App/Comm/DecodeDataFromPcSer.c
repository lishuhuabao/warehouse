#include "includes.h"

toPcSerReSendPackListSt netToPcPackListStab[5];
toPcSerReSendPackListSt netToServerPackListStab[5];
/*
**
*/
typedef struct{
	CmdEnum cmd; //解码命令
	void (*handler)(u08 *buf, u16 len); //解码函数
}decodeDataFromPcSerHandlerSt;

typedef struct{
	CmdEnum cmd;
	u08 para[__FROM_PC_SERVER_PACKET_MAX_NUM];
}dataFromPcSerHandlerSt;

/*
**
*/
typedef struct{
	setNetParaTypeEnum type;
	u08 buf[__FROM_PC_SERVER_PACKET_MAX_NUM];
}setNetParaSt;

typedef struct{
	setLableParaTypeEnum type;
	u08 buf[__FROM_PC_SERVER_PACKET_MAX_NUM];
}setLableParaSt;

/*
**
*/

u16 slaveAddr;
u08 slaveCh;
u08 exitUpdata;

/*
**
*/
static void __PassOnCmdToLable(CmdEnum cmd, u08 *buf, u16 len);//网关转发PC命令数据，同时直接应答PC


static void __AckInqNetInfor(u08 *buf, u16 len);//查询网关基本信息
static void __AckInqLableInfor(u08 *buf, u16 len);//查询标签基本信息
static void __AckSetNetPara(u08 *buf, u16 len);//设置网关参数
static void __AckSetLablePara(u08 *buf, u16 len);//设置网关参数
static void __AckUpdateNet(u08 *buf, u16 len);//进入网关升级模式
static void __AckClrLabLED(u08 *buf, u16 len);//
static void __AckCtrlLab(u08 *buf, u16 len);//
static void __AckCtrlLabDis(u08 *buf, u16 len);//
static void __AckUpdataLable(u08 *buf, u16 len);//
static void __AckSetLabAddr(u08 *buf, u16 len);//
static void __AckAck(u08 *buf, u16 len);//

/*
**
*/
static void __PassOnCmdToPcSer(CmdEnum cmd, u08 *buf, u16 len); //转发标签的数据（申请地址分配）给PC
static void __ToPcSerReSendRegAddr(u08 *buf, u16 len);

static void __ClrSendToPcResend(void);
static void __ClrSendToServerResend(void);

const toPcSerReSendHandlerSt toPcSerReSendHandlerStab[] = { 
	{__TO_PC_SERVER_RESEND_REG_ADDR,  __ToPcSerReSendRegAddr},
};

static const decodeDataFromPcSerHandlerSt decodeDataFromPcSerHandlerStab[] = { //解码
	{__CMD_INQ_NET_INFOR,       __AckInqNetInfor}, //查询网关基本信息
	{__CMD_INQ_LABLE_INFOR,     __AckInqLableInfor},//查询标签基本信息
	{__CMD_SET_NET_PARA,        __AckSetNetPara},//设置网关基本参数
	{__CMD_SET_LABLE_PARA,      __AckSetLablePara}, //设置标签基本参数
	{__CMD_UPDATE_NET,          __AckUpdateNet},//进入网关升级模式
	{__CMD_CLR_LABEL_LED,          __AckClrLabLED},
	{__CMD_CTRL_LABEL,          __AckCtrlLab},
	{__CMD_CTRL_LABEL_DIS,      __AckCtrlLabDis}, 
	{__CMD_UPDATE_LABLE,        __AckUpdataLable},
	{__CMD_SET_LABLE_ADDR,      __AckSetLabAddr},
	{__CMD_ACK,                 __AckAck},
};


/*
**校验 码
*/
u16 __CrcCheck1021(u08 *buf, u16 len)
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
**
*/
void __SendAckToPcSer(CmdEnum cmd, u08 *buf, u16 len, DestinationForUart des) //网关直接应答PC
{
	u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	u16 __ccs;

	__buf[0] = __PC_SERVER_FRAME_START1;
	__buf[1] = __PC_SERVER_FRAME_START2; //packet header

	__buf[2] = ((len + 1) >> 8); //packet length MSB
	__buf[3] = ((len + 1) & 0xff); //packet length LSB

	__buf[4] = __CMD_ACK;
	
	__buf[5] = cmd;

	memcpy(&__buf[6], buf, len);
	

	__ccs = __CrcCheck1021(__buf, 6 + len);

	if(__ACK_STA_OK == __buf[6])
	{
		TRACE("Riss is %ddB\r\n", (s16)(((u16)__buf[9] << 8) | (u16)__buf[10]));
		TRACE("Bat is %dmV\r\n", ((u16)__buf[11] << 8) | (u16)__buf[12]);
	}
	
	__buf[6 + len] = __ccs >> 8;
	__buf[7 + len] = __ccs & 0xff;

	PcSerUartSendPacket(__buf, 8 + len, 1);//des
}

/*
**
*/
static void __PassOnCmdToLable(CmdEnum ackCode, u08 *buf, u16 len)//网关转发PC命令数据，同时直接应答PC
{
	u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	u16 __crc;
	
	if(NET_SET_STA_WAIT == netSetStaE)
	{
		__SendAckStaToPcSer(ackCode, __ACK_STA_WAIT, AutoSel);
		TRACE("net lora is wait!\r\n");
		return;
	}
	__SendAckStaToPcSer(ackCode, __ACK_STA_WAIT, AutoSel);

	__buf[0] = buf[0]; //addr MSB
	__buf[1] = buf[1]; //addr LSB

	if(0xFF == buf[0] && 0xFF == buf[1])
	{
		__buf[2] = buf[2];
	}
	else
	{
		__buf[2] = GetChByAddr(buf[0]); //ch
	}
	//__buf[2] = dataSaveS.netCh; //ch

	__buf[3] = __PC_SERVER_FRAME_START1;
	__buf[4] = __PC_SERVER_FRAME_START2;
	
	__buf[5] = (len - 1) >> 8; //L MSB
	__buf[6] = (len - 1); //L LSB
	__buf[7] = ackCode;
	//__buf[8] = buf[0];
	//__buf[9] = buf[1];

	__buf[8] = dataSaveS.netAddr >> 8;
	__buf[9] = dataSaveS.netAddr & 0xff;

	memcpy(&__buf[10], &buf[3], len - 3);

	__crc = __CrcCheck1021(&__buf[3], len + 4);

	__buf[len + 7] = (u08)(__crc >> 8);
	__buf[len + 8] = (u08)(__crc);

	CommLoraSendPro(__buf, len + 9);
}

/*
**接收云平台数据解码
*/
void __DecodeDataFromPcSer(u08 *buf, u16 len)
{
	u08 i;
	dataFromPcSerHandlerSt __dataFromPcSerHandlerS;
	
	memcpy((void *)&__dataFromPcSerHandlerS, (const void *)buf, len + 1);	
	
	for(i = 0; i < sizeof(decodeDataFromPcSerHandlerStab) / sizeof(decodeDataFromPcSerHandlerStab[0]); i++)
	{
		if(__dataFromPcSerHandlerS.cmd == decodeDataFromPcSerHandlerStab[i].cmd)
		{
			decodeDataFromPcSerHandlerStab[i].handler(__dataFromPcSerHandlerS.para, len); 
			break;
		}
	}
}

//查询设备基本信息
static void __AckInqNetInfor(u08 *buf, u16 len)
{
	inqTypeEnum __inqType;
	u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	u08 __ptr;

	__inqType = (inqTypeEnum)buf[0];

	switch(__inqType)
	{
		case __INQ_TYPE_DEV_ADDR:
			__ptr        = 0;
			__buf[__ptr] = __ACK_STA_OK;
			__ptr++;
			OsVar16IntoBuf(dataSaveS.netAddr, &__buf[1]);
			__ptr += 2;
			
			__SendAckToPcSer(__CMD_INQ_NET_INFOR, __buf, __ptr, AutoSel);
			break;
			
		case __INQ_TYPE_TX_PWR:
			__ptr        = 0;
			__buf[__ptr] = __ACK_STA_OK;
			__ptr++;
			__buf[__ptr] = (u08)SX1276.Settings.LoRa.Power;
			__ptr++;
			__SendAckToPcSer(__CMD_INQ_NET_INFOR, __buf, __ptr, AutoSel);
  			break;

  		case __INQ_TYPE_RELEASE_VER:
  			__ptr        = 0;
			__buf[__ptr] = __ACK_STA_OK;
			__ptr++;

			memcpy(&__buf[__ptr], fwRelVer, strlen((const char *)fwRelVer));
			__ptr += strlen((const char *)fwRelVer);
			__SendAckToPcSer(__CMD_INQ_NET_INFOR, __buf, __ptr, AutoSel);
  			break;
  			
		default:
			break;
	}
}

/*
**
*/
static void __AckInqLableInfor(u08 *buf, u16 len)
{
	inqTypeEnum __inqTypeE;

	memcpy(&__inqTypeE, &buf[3], len - 3);

	switch(__inqTypeE)
	{
		case __INQ_TYPE_RELEASE_VER:
			__PassOnCmdToLable(__CMD_INQ_LABLE_INFOR, buf, len);
			break;

		case __INQ_TYPE_TX_PWR:
			__PassOnCmdToLable(__CMD_INQ_LABLE_INFOR, buf, len);
			break;

		default:	
			break;
	}

}


/*
**
*/
static void __AckSetNetPara(u08 *buf, u16 len)
{
	setNetParaSt __setNetParaS;
	u16 __netAddr;
	u08 __ackBuf[1] = {0};

	memcpy(&__setNetParaS, buf, len);

	OsBufIntoVar16(&__netAddr, __setNetParaS.buf);

	switch(__setNetParaS.type)
	{
		case __SET_NET_PARA_ADDR:
			if(__netAddr < __NET_ADDR_START || __netAddr >  __NET_ADDR_END) 
			{
				__ackBuf[0] = (u08)__ACK_STA_ERR;
				__SendAckToPcSer(__CMD_SET_NET_PARA, __ackBuf, 1, AutoSel);
 			}
			else
			{
				dataSaveS.netAddr  = __netAddr;
				dataSaveS.netCh    = GetChByAddr(__netAddr & 0xFF);
				// BspLoraChSet(dataSaveS.netCh);	
				__SendAckStaToPcSer(__CMD_SET_NET_PARA, __ACK_STA_OK, AutoSel);
			}
			break;

		case __SET_NET_PARA_LED:
			break;

		case __SET_NET_PARA_BUZ:
			break;

		case __SET_NET_PARA_TX_PWR:
			if((s08)__setNetParaS.buf[0] < 1 || (s08)__setNetParaS.buf[0] > 20)
			{
				__ackBuf[0] = (u08)__ACK_STA_ERR;
				__SendAckToPcSer(__CMD_SET_NET_PARA, __ackBuf, 1, AutoSel);	
			}
			else
			{
				dataSaveS.txPwr = (s08)__setNetParaS.buf[0];
				SX1276SetRfTxPower(dataSaveS.txPwr);
				__ackBuf[0] = (u08)__ACK_STA_OK;
				__SendAckToPcSer(__CMD_SET_NET_PARA, __ackBuf, 1, AutoSel);	
			}
			break;

		default:	
			break;
	}

}

static void __AckSetLablePara(u08 *buf, u16 len)
{
	setLableParaSt __setLableParaS;
	u08 __ackBuf[1] = {0};
	u16 __waitTime;
	u16 __labAddr;

	//0   1  2  3   4  5
	//b7 ff 00 08 00 0a
	
	memcpy(&__setLableParaS, &buf[3], len - 3);


	switch(__setLableParaS.type)
	{
		case __SET_LABLE_PARA_LED:
			break;

		case __SET_LABLE_PARA_BUZ:
			break;

		case __SET_LABLE_PARA_WAIT_SLEEP:
			OsBufIntoVar16(&__waitTime, __setLableParaS.buf);
			
			//TRACE("__waitTime is %d\r\n", __waitTime);
			
			if(__waitTime < 5)
			{
				__ackBuf[0] = (u08)__ACK_STA_OTHER_ERR;
				__SendAckToPcSer(__CMD_SET_LABLE_PARA, __ackBuf, 1, AutoSel);
			}
			else
			{
				__PassOnCmdToLable(__CMD_SET_LABLE_PARA, buf, len);
			}
			break;

		case __SET_LABLE_PARA_ADDR:
			OsBufIntoVar16(&__labAddr, __setLableParaS.buf);
			
			if(__labAddr <= 0x000F)
			{
				__ackBuf[0] = (u08)__ACK_STA_OTHER_ERR;
				__SendAckToPcSer(__CMD_SET_LABLE_PARA, __ackBuf, 1, AutoSel);
			}
			else
			{
				__PassOnCmdToLable(__CMD_SET_LABLE_PARA, buf, len);
			}
			break;

		case __SET_LABLE_PARA_TX_PWR:
			if((s08)__setLableParaS.buf[0] < 1 || (s08)__setLableParaS.buf[0] > 20)
			{
				__ackBuf[0] = (u08)__ACK_STA_OTHER_ERR;
				__SendAckToPcSer(__CMD_SET_LABLE_PARA, __ackBuf, 1, AutoSel);
			}
			else
			{
				__PassOnCmdToLable(__CMD_SET_LABLE_PARA, buf, len);
			}	
			break;

		default:	
			break;
	}

}


/*
**进入网关升级模式
*/
static void __AckUpdateNet(u08 *buf, u16 len)
{
	if (1 == buf[0])
	{
     updateParaSaveS.updateFlag = 1;
	   BspSaveDataV2(FLASH_UPDATE_FLAG_START_ADDR, 0, (u08 *)&updateParaSaveS.updateFlag, sizeof(updateParaSaveS.updateFlag));
	   
		 __SendAckStaToPcSer(__CMD_UPDATE_NET, __ACK_STA_OK, AutoSel);		
		
		 HAL_Delay(500);   // 需延迟一段时间，通过wifi连接的服务端才能接收到应答
		 __set_PRIMASK(1);  //关中断；
		 NVIC_SystemReset();	
		
  }
}


/*
**
*/
static void __AckClrLabLED(u08 *buf, u16 len)
{
	__PassOnCmdToLable(__CMD_CLR_LABEL_LED, buf, len);
}


/*
**
*/
static void __AckCtrlLab(u08 *buf, u16 len)
{
	__PassOnCmdToLable(__CMD_CTRL_LABEL, buf, len);
}


/*
**
*/
static void __AckCtrlLabDis(u08 *buf, u16 len)
{
	__PassOnCmdToLable(__CMD_CTRL_LABEL_DIS, buf, len);
}


/*
**
*/
static void __AckUpdataLable(u08 *buf, u16 len)
{
	#if TRACE_REC_PC_DAT_AS_HEX_EN
		u16 i;
	#endif
	
	if(NET_SET_STA_WAIT == netSetStaE)
	{
		__SendAckStaToPcSer(__CMD_UPDATE_LABLE, __ACK_STA_WAIT, AutoSel);
		TRACE("net lora is wait!\r\n");
		return;
	}
	OsBufIntoVar16(&slaveAddr, &buf[0]);
  slaveCh = GetChByAddr(buf[0]); //wzh
	
	if(0xFFFF == slaveAddr)
	{
		//__SendAckStaToPcSer(__CMD_UPDATE_LABLE, __ACK_STA_OTHER_ERR, AutoSel); //wzh
		//return; //
		slaveCh = buf[2];
		commLoraCtrlS.step = COMM_LORA_STEP_UPDATA;
	}
	//else //wzh
	//{ //wzh
	//	__SendAckStaToPcSer(__CMD_UPDATE_LABLE, __ACK_STA_WAIT, AutoSel);
	//} //wzh

	#if TRACE_REC_PC_DAT_AS_HEX_EN
		TRACE("Ack Pc is:\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif
	if(1 == buf[3])
	{
		//TRACE("进入标签升级模式!\r\n");
		
		__PassOnCmdToLable(__CMD_UPDATE_LABLE, buf, len);

	}
	else if(0 == buf[3])
	{
		//TRACE("退出标签升级模式!\r\n");
		//EnterUpdataLable(); //wzh
		ExitUpdateLable();//wzh
	}
}


/*
** 00 05 63 00 35
*/
static void __AckSetLabAddr(u08 *buf, u16 len)
{
	u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	u16 __crc;

	__SendAckStaToPcSer(__CMD_SET_LABLE_ADDR, __ACK_STA_WAIT, AutoSel);

	__buf[0] = CONF_LABLE_ADDR_MODE_LORA_ADDR >> 8;
	__buf[1] = CONF_LABLE_ADDR_MODE_LORA_ADDR;

	__buf[2] = GetChByAddr(CONF_LABLE_ADDR_MODE_LORA_CH);

	__buf[3] = __PC_SERVER_FRAME_START1;
	__buf[4] = __PC_SERVER_FRAME_START2;
	
	__buf[5] = (len + 1) >> 8; //L MSB
	__buf[6] = (len + 1); //L LSB

	__buf[7] = __CMD_ACK;
	__buf[8] = __CMD_SET_LABLE_ADDR;

	memcpy(&__buf[9], buf, len);

	__crc = __CrcCheck1021(&__buf[3], len + 6);

	__buf[len + 9] = (u08)(__crc >> 8);
	__buf[len + 10] = (u08)(__crc);

	CommLoraSendPro(__buf, len + 11);
}

/*
**
*/
static void __AckAck(u08 *buf, u16 len)
{
	CmdEnum __ackCmd;

	TRACE("Get Ack From PC!\r\n");
	__ClrSendToPcResend();
	
	__ackCmd = (CmdEnum)buf[0];

	switch(__ackCmd)
	{
		case __CMD_SET_LABLE_ADDR:
			__AckSetLabAddr(&buf[1], len - 1);
			break;

		default:
			break;
	}
}



/*
**
*/
static void __PassOnCmdToPcSer(CmdEnum cmd, u08 *buf, u16 len) //转发标签的数据（申请地址分配）给PC
{
	u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	u16 __ccs;

	__buf[0] = __PC_SERVER_FRAME_START1;
	__buf[1] = __PC_SERVER_FRAME_START2; //packet header

	__buf[2] = ((len) >> 8); //packet length MSB
	__buf[3] = ((len) & 0xff); //packet length LSB
	
	__buf[4] = cmd;

	memcpy(&__buf[5], buf, len);
	

	__ccs = __CrcCheck1021(__buf, 5 + len);
	
	
	__buf[5 + len] = __ccs >> 8;
	__buf[6 + len] = __ccs & 0xff;

	PcSerUartSendPacket(__buf, 7 + len, AutoSel);
}

/*
**
*/
static void __ToPcSerReSendRegAddr(u08 *buf, u16 len)
{
	__PassOnCmdToPcSer((CmdEnum)__TO_PC_SERVER_CMD_REG_ADDR, buf, len);
}


/*
**
*/
void __SendAckStaToPcSer(CmdEnum cmd, AckStaEnum sta,DestinationForUart des)
{
	u08 __ackBuf[1] = {0};

	__ackBuf[0] = (u08)sta;
	__SendAckToPcSer(cmd, __ackBuf, 1, des);
}
/*
**
*/
void __SendToPcPacketRepeatWaitAck(toPcSerReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len)
{
	u08 i; 
	u08 __tabSize;

	__ClrSendToPcResend();
	
	__tabSize = sizeof(netToPcPackListStab) / sizeof(netToPcPackListStab[0]) - 1;
	
	for(i = 0; i < __tabSize; i++)
	{
		if(0 == netToPcPackListStab[i].cnt)
		{
			break;
		}
	}

	if(i == __tabSize)
	{
		TRACE("发送缓存已满!\r\n");
		return;
	}
	
	netToPcPackListStab[i].sendType       =	type;
	netToPcPackListStab[i].cnt 	          =	resendTimes* interTime + delayTime;
	netToPcPackListStab[i].enCnt	        =	resendTimes* interTime;
	netToPcPackListStab[i].timeInterval   = interTime;
	netToPcPackListStab[i].reSendTimes    = resendTimes;
	netToPcPackListStab[i].sendDelay	    = delayTime;
	memcpy(netToPcPackListStab[i].buf, buf, len);
	netToPcPackListStab[i].len            = len;
}


/*
**
*/
static void __ClrSendToPcResend(void)
{
	u08 i; 
	u08 __tabSize;

	__tabSize = sizeof(netToPcPackListStab) / sizeof(netToPcPackListStab[0]);


	for(i = 0; i < __tabSize - 1; i++)
	{
		memcpy(&netToPcPackListStab[i], &netToPcPackListStab[i + 1], sizeof(netToPcPackListStab[i]));
		if(0 == netToPcPackListStab[i + 1].cnt)
		{
			  break;
		}
	}
}

/*
**
*/
void __SendToServerPacketRepeatWaitAck(toPcSerReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len)
{
	u08 i; 
	u08 __tabSize;

	__ClrSendToServerResend();
	
	__tabSize = sizeof(netToServerPackListStab) / sizeof(netToServerPackListStab[0]) - 1;
	
	for(i = 0; i < __tabSize; i++)
	{
		if(0 == netToServerPackListStab[i].cnt)
		{
			break;
		}
	}

	if(i == __tabSize)
	{
		TRACE("发送缓存已满!\r\n");
		return;
	}
	
	netToServerPackListStab[i].sendType       =	type;
	netToServerPackListStab[i].cnt 	          =	resendTimes* interTime + delayTime;
	netToServerPackListStab[i].enCnt	        =	resendTimes* interTime;
	netToServerPackListStab[i].timeInterval   = interTime;
	netToServerPackListStab[i].reSendTimes    = resendTimes;
	netToServerPackListStab[i].sendDelay	    = delayTime;
	memcpy(netToServerPackListStab[i].buf, buf, len);
	netToServerPackListStab[i].len            = len;
}


/*
**
*/
static void __ClrSendToServerResend(void)
{
	u08 i; 
	u08 __tabSize;

	__tabSize = sizeof(netToServerPackListStab) / sizeof(netToServerPackListStab[0]);

	for(i = 0; i < __tabSize - 1; i++)
	{
		memcpy(&netToServerPackListStab[i], &netToServerPackListStab[i + 1], sizeof(netToServerPackListStab[i]));
		if(0 == netToServerPackListStab[i + 1].cnt)
		{
			  break;
		}
	}
}



void PcSerUartSendPacket(u08 *buf, u16 len, DestinationForUart des)
{
	 if ( ((des == AutoSel) && (true == bspPcUartS.busyflag)) || (des == SendToPc) )
   {
	    BspPcUartSendPacket(buf,len);        
	 }
	 else
	 {
			BspWifiUartSendPacket(buf,len); 
	 }	
}

/*
**398+ch Mhz
**430-440Mhz
**ch:32--42
**net:32-35
**lable:net + 3
**32, 35, 38,  41
*/
u08 GetChByAddr(u08 addr)
{
	return(addr % 128);
}
