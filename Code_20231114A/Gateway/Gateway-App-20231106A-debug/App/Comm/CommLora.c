#include "includes.h"

/*
**
*/
typedef struct{
	u08 *recStr; //解码命令
	void (*ackHandler)(u08 *buf, u16 len); //解码函数
}loraCommRecHandlerSt;

typedef struct{
	loraReSendTypeEnum sendCmd;
	void(* sendPackt)(u08 *buf, u16 len); //发送指令函数
}loraCommSendHandlerSt;


commLoraCtrlSt commLoraCtrlS;



loraSendPackListSt loraSendPackListStab[5];
netSetStaEnum netSetStaE;
u08 gobalAddrFlag;
u08 loraRecFlag;

/*
**
*/
static  void __LoraSendProData(u08 *buf, u16 len);//


//在无应答重发机制下发送指令
static void __LoraSendPacketRepeatWaitAck(loraReSendTypeEnum type, 
													  u08 resendTimes,
                                                      u32 interTime,
                                                      u32 delayTime,
                                                      u08 *buf,
                                                      u16 len);

static const loraCommSendHandlerSt loraCommSendHandlerStab[] = { 
	{__LORA_RE_SEND_PRO_DATA,                                __LoraSendProData},

};

static void __LoraResendErrHandler(loraReSendTypeEnum cmd);


/*
**接收Lora模块数据处理
*/
void CommLoraRecDatHandler(void)
{
	#if TRACE_LORA_REC_DAT_AS_HEX_EN
		u16 i;
	#endif 

	if(0 == loraRecFlag)
	{
		return;
	}

	loraRecFlag = 0;
		
	BspLedFlashSet(BSP_LED_TYPE_LORA_RXD, 2, 100, 100);//BspSimPwmCall(&ledS[led], times, on, off);
		
	if( COMM_LORA_STEP_PC_COMM == commLoraCtrlS.step )
	{
			#if TRACE_LORA_REC_DAT_AS_HEX_EN
		    TRACE("rec data from label is:\r\n");
		    for(i = 0; i < SX1276.Settings.LoRaPacketHandler.Size; i++)
	      	{
			      TRACE(" %02x", RxTxBuffer[i]);
	        }
		    TRACE("\r\n");
	     #endif	
      CommLoraProRecDatDecode(&RxTxBuffer[3], SX1276.Settings.LoRaPacketHandler.Size-3);
	} 
	else if(COMM_LORA_STEP_UPDATA == commLoraCtrlS.step)
	{
			#if TRACE_LORA_REC_DAT_AS_HEX_EN
				TRACE("Receive Data from label When Update As Hex is :\r\n");
				for(i = 0; i < SX1276.Settings.LoRaPacketHandler.Size; i++)
	      	{
			      TRACE(" %02x", RxTxBuffer[i]);
	        }
		    TRACE("\r\n");
			#endif
			PcSerUartSendPacket(&RxTxBuffer[3], SX1276.Settings.LoRaPacketHandler.Size-3, AutoSel);
	}
}

/*
**无应答重发
*/
//run every 1ms
void CommLoraReSendEvent(void)
{
	static osDelaySt __osDelayS;

	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}
	
	if(loraSendPackListStab[0].cnt) 
    {
    	if(0 == loraSendPackListStab[0].cnt % loraSendPackListStab[0].timeInterval &&
    		loraSendPackListStab[0].cnt <= loraSendPackListStab[0].enCnt)
		{
			loraCommSendHandlerStab[loraSendPackListStab[0].sendType].sendPackt(loraSendPackListStab[0].buf, loraSendPackListStab[0].len);
		}
		
		loraSendPackListStab[0].cnt--;
		
		if(0 == loraSendPackListStab[0].cnt)
		{
			TRACE("SEND is not receve is \r\n");//网关发送指令，但没有接收到标签板返回的指令，则返回08
			
			__LoraResendErrHandler((loraReSendTypeEnum)0);
			//memset(&loraSendPackListStab, 0, sizeof(loraSendPackListStab));
		}
	}
}


/*
**在无应答重发机制下发送数据包
*/
static void __LoraSendPacketRepeatWaitAck(loraReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len)
{
	u08 i; 
	u08 __tabSize;

	__ClrLoraResend();
	__tabSize = sizeof(loraSendPackListStab) / sizeof(loraSendPackListStab[0]) - 1;
	
	for(i = 0; i < __tabSize; i++)
	{
		if(0 == loraSendPackListStab[i].cnt)
		{
			break;
		}
	}

	if(i == __tabSize)
	{
		TRACE("发送缓存已满!\r\n");
		return;
	}
	
	loraSendPackListStab[i].sendType =	type;
	loraSendPackListStab[i].cnt 	 =	resendTimes* interTime + delayTime;
	loraSendPackListStab[i].enCnt	 =	resendTimes* interTime;
	loraSendPackListStab[i].timeInterval   = interTime;
	loraSendPackListStab[i].reSendTimes    = resendTimes;
	loraSendPackListStab[i].sendDelay	   = delayTime;
	memcpy(loraSendPackListStab[i].buf, buf, len);
	loraSendPackListStab[i].len  = len;
}


/*
**
*/
static void __LoraResendErrHandler(loraReSendTypeEnum cmd)
{
	if(gobalAddrFlag)
	{
		__SendAckStaToPcSer((CmdEnum)loraSendPackListStab[0].buf[7], __ACK_STA_GOBAL, AutoSel);
	}
	else
	{
		__SendAckStaToPcSer((CmdEnum)loraSendPackListStab[0].buf[7], __ACK_STA_TIMEOUT, AutoSel);
	}

	memset(&loraSendPackListStab, 0, sizeof(loraSendPackListStab));
	//BspLoraHwReset();
}





/*
**
*/
void __ClrLoraResend(void)
{
	u08 i; 
	u08 __tabSize;

	__tabSize = sizeof(loraSendPackListStab) / sizeof(loraSendPackListStab[0]);


	for(i = 0; i < __tabSize - 1; i++)
	{
		memcpy(&loraSendPackListStab[i], &loraSendPackListStab[i + 1], sizeof(loraSendPackListStab[i]));
		if(0 == loraSendPackListStab[i + 1].cnt)
		{
			  break;
		}
	}
}


/*
**
*/
void EnterSetLableAddrMode(void)
{

}

/*
**
*/
void ExitSetLableAddrMode(void)
{

}

/*
**
*/
void EnterUpdataLable(void)
{

}
void ExitUpdateLable(void)
{
	
}

/*
**  b7 ff 37 6a 01 00 09 31 00 03 06 02 01 03 20 01 2c d5 27
*/
static void __LoraSendProData(u08 *buf, u16 len)
{
	#if TRACE_SEND_LORA_PRO_EN
		u08 i;
		
		TRACE("__LoraSendProData:\r\n");
	
		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
	#endif
	// BspLoraChSet(buf[2]);
	BspLoraSendPacket(buf, len);
}


/*
**
*/
void CommLoraSendPro(u08 *buf, u16 len)
{
	if(0xFF == buf[0] && 0xFF == buf[1])
	{
		gobalAddrFlag = 1;
	}
	else
	{
		gobalAddrFlag = 0;
	}

	__LoraSendPacketRepeatWaitAck(__LORA_RE_SEND_PRO_DATA, __LORA_MAX_SEND_REP_TIMES, 2000, 0, buf, len);
	//commLoraCtrlS.step = COMM_LORA_STEP_PC_COMM; //wzh
}


/*
**
*/
void CommLoraSendUpdata(u08 *buf, u16 len)
{	
	u08 __buf[__LORA_PACKET_MAX_NUM] = {0};
	//u16 i;

	__buf[0] = (u08)(slaveAddr >> 8);
	__buf[1] = (u08)(slaveAddr & 0xFF);
	
	__buf[2] = 0xFF ; //GetChByAddr(__buf[0]);

	memcpy(&__buf[3], buf, len);
	// BspLoraChSet(slaveCh);
	BspLoraSendPacket(__buf, len + 3);
}


