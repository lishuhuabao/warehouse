#include "includes.h"

// NB86-G AT指令语法
//--------------------|------------------------|-------------------------------------
//  Test Command        AT+<cmd>=?               Check possible sub-parameter values
//  Read Command        AT+<cmd>?                Check current sub-parameter values
//  Set Command         AT+<cmd>=p1[,p2[,p3]]]   Set command
//  Execution Command   AT+<cmd>                 Execution command
//--------------------|------------------------|-------------------------------------

/*
**查询IMEI: AT+CGSN=1
**查询IMSI: AT+CIMI   
*/

/*
**
*/


#define __LORA_CMD_RESET             "AT+Z\r\n"
#define __LORA_CMD_ENTER_SET1        "+++"
#define __LORA_CMD_ENTER_SET2        "a"
#define __LORA_CMD_SET_WORK_MODE     "AT+WMODE=FP\r\n"
#define __LORA_CMD_SET_POWER_LR_MODE "AT+PMODE=LR\r\n"
#define __LORA_CMD_SET_POWER_WU_MODE "AT+PMODE=WU\r\n"
#define __LORA_CMD_SET_WAKE_TIME     "AT+WTM=500\r\n"
#define __LORA_CMD_SET_SPD_LEV       "AT+SPD=9\r\n"
#define __LORA_CMD_SET_CH            "AT+CH="
#define __LORA_CMD_SET_ADDR          "AT+ADDR="
#define __LORA_CMD_SET_IDLE_TIME     "AT+ITM=3\r\n"

#define __LORA_CMD_SET_REG_CH        "AT+CH=36\r\n"
#define __LORA_CMD_SET_REG_ADDR      "AT+ADDR=16\r\n"


#define __LORA_CONF_LABLE_REG_MODE_ADDR  0x0010
#define __LORA_CONF_LABLE_REG_MODE_CH    0x10

extern int key_test1;
extern int key_test2;
// ackCmdToNetSt ackCmdToNetS;

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

u08 loraRecFlag;

commLoraCtrlSt commLoraCtrlS;
loraSendPackListSt loraSendPackListStab[10];
netSetStaEnum netSetStaE;
u08 gobalAddrFlag;


//在无应答重发机制下发送指令
static void __LoraSendPacketRepeatWaitAck(loraReSendTypeEnum type, 
													  u08 resendTimes,
                                                      u32 interTime,
                                                      u32 delayTime,
                                                      u08 *buf,
                                                      u16 len);



static const loraCommSendHandlerSt loraCommSendHandlerStab[] = { 
	{__LORA_RE_SEND_PRO_DATA,                   __LoraSendProData},
};

static void __LoraResendErrHandler(loraReSendTypeEnum cmd);


/*
**接收Lora模块数据处理
*/
int afd;
void CommLoraRecDatHandler(void)
{
	#if TRACE_REC_DAT_LORA_AS_HEX_EN
		u16 i;
	#endif

 //afd=sx1278S.Settings.LoRaPacketHandler.Size - 3;
	if(0 == loraRecFlag)
	{
		return;
	}

	loraRecFlag = 0;
	
//	if( COMM_LORA_STEP_UPDATA == commLoraCtrlS.step )
//	{
//			UpdateDataProcess(&RxTxBuffer[3], sx1278S.Settings.LoRaPacketHandler.Size - 3);
//			ClrPowerEnterTime();
//	}

  if( COMM_LORA_STEP_PC_COMM == commLoraCtrlS.step )
   {
	    #if TRACE_REC_DAT_LORA_AS_HEX_EN
		  TRACE("rec data from lora is %d\r\n", sx1278S.Settings.LoRaPacketHandler.Size);
		  for(i = 0; i < sx1278S.Settings.LoRaPacketHandler.Size; i++)
		  {
		  	TRACE(" %02x", RxTxBuffer[i]);
	   	}
	   	TRACE("\r\n");
	    #endif

      CommLoraProRecDatDecode(&RxTxBuffer[3], sx1278S.Settings.LoRaPacketHandler.Size - 3);
      ClrPowerEnterTime();
  }
	
}

/*
**无应答重发
*/
//run every 1ms
void CommLoraReSendEvent(void)
{
	static osDelaySt __osDelayS;

	//do every 1ms
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
			loraSendPackListStab[0].cnt--;
		
		if(0 == loraSendPackListStab[0].cnt)
		{
			//TRACE("SEND is not receve is \r\n");
			memset(&loraSendPackListStab, 0, sizeof(loraSendPackListStab));
			__LoraResendErrHandler((loraReSendTypeEnum)0);
		}
		}
	}
	else
	{
//			if(key_test1==1)
//			{
//				ackCmdToNetS1.buf[0]=0x00;
//				ackCmdToNetS1.buf[1]=0x00;
//				ackCmdToNetS1.buf[2]=0x00;
//				ackCmdToNetS1.buf[3]=0x6A;
//				ackCmdToNetS1.buf[4]=0x01;
//				ackCmdToNetS1.buf[5]=0x00;
//				ackCmdToNetS1.buf[6]=0x08;
//				ackCmdToNetS1.buf[7]=0xF0;
//				ackCmdToNetS1.buf[8]=0x33;
//				ackCmdToNetS1.buf[9]=0x80;
//				ackCmdToNetS1.buf[10]=((dataSaveS.addr >> 8) & 0x00FF);
//				ackCmdToNetS1.buf[11]=dataSaveS.addr & 0x00FF;	
//				ackCmdToNetS1.buf[12]=0xFF;  
//				ackCmdToNetS1.buf[13]=0xCA;//00 00 00 6A 01 00 08 F0 33 80 xx xx FF CA 00 00 xx xx 其他错误返回指令
//				ackCmdToNetS1.buf[14]=0x00;
//				ackCmdToNetS1.buf[15]=0x00;	//ackCmdToNetS1.buf用于生成校验码

//				ackCmdToNetS2.buf[0]=0x00;
//				ackCmdToNetS2.buf[1]=0x00;
//				ackCmdToNetS2.buf[2]=0x00; 
//				ackCmdToNetS2.buf[3]=0x6A;
//				ackCmdToNetS2.buf[4]=0x01;  
//				ackCmdToNetS2.buf[5]=0x00;
//				ackCmdToNetS2.buf[6]=0x08;
//				ackCmdToNetS2.buf[7]=0xF0;//应答返回
//				ackCmdToNetS2.buf[8]=0x33;
//				ackCmdToNetS2.buf[9]=0x80;//其他错误返回
//				ackCmdToNetS2.buf[10]=((dataSaveS.addr >> 8) & 0x00FF);//地址
//				ackCmdToNetS2.buf[11]=dataSaveS.addr & 0x00FF;
//				ackCmdToNetS2.buf[12]=0xFF;
//				ackCmdToNetS2.buf[13]=0xCA;
//				ackCmdToNetS2.buf[14]=0x00;
//				ackCmdToNetS2.buf[15]=0x00;	
//				ackCmdToNetS2.buf[16]=((__LoraProCheckCode(ackCmdToNetS1.buf, 16)>>8)& 0x00FF);//生成校验码
//				ackCmdToNetS2.buf[17]=(__LoraProCheckCode(ackCmdToNetS1.buf, 16)& 0x00FF);//将ackCmdToNetS1.buf生成的校验码赋给ackCmdToNetS2.buf	
//				__LoraSendProData(ackCmdToNetS2.buf, ackCmdToNetS2.len + 18);//Lora发送指令函数		
	}
}


/*
**在无应答重发机制下发送数据包
*/
static void __LoraSendPacketRepeatWaitAck(loraReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len)
{
	u08 i; 
	u08 __tabSize;

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
		//TRACE("发送缓存已满!\r\n");
		return;
	}
	//TRACE("i is %d\r\n", i);
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
void __LoraSendProData(u08 *buf, u16 len)
{
	
	BspLoraSendPacket(buf, len);
}


/*
**
*/
void CommLoraSendPro(u08 *buf, u16 len)
{
	//__LoraSendPacketRepeatWaitAck(__LORA_RE_SEND_PRO_DATA, __LORA_MAX_SEND_REP_TIMES, 5000, 0, buf, len); //wzh
	__LoraSendPacketRepeatWaitAck(__LORA_RE_SEND_PRO_DATA, 2, 1000, 0, buf, len); //wzh
}

/*
**
*/
u08 GetChByAddr(u08 addr)
{
	return(addr % 128);
}





