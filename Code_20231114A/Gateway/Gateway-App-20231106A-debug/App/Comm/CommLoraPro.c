#include "includes.h"

/*
**
*/
#define __LORA_PRO_PACKET_MAX_NUM        256   //LoraPro模块通讯数据包最大长度

#define __LORA_PRO_FRAME_START1          0x6A   //LoraPro模块通讯帧起始字节1
#define __LORA_PRO_FRAME_START2          0x01   //LoraPro模块通讯帧起始字节2

typedef enum{
	__LORA_PRO_ACK_INQ_LABLE_INFOR = 0X01, //
	__LORA_PRO_ACK_CTRL_LABLE      = 0x31, //
	__LORA_PRO_ACK_CLR_LABLE_LED   = 0x32, //
	__LORA_PRO_ACK_UPDATE          = 0x70, 
	__LORA_PRO_ACK_REG_ADDR        = 0xF1, //
	__LORA_PRO_ACK_CMD             = 0xF0, //
}loraProAckCodeEnum;


typedef enum{ 
	__LORA_PRO_CMD_ACK        = 0xF0, //应答返回指令
}loraProCmdEnum;  //发送指令

typedef enum{
	__LORA_PRO_CMD_ACK_STA_OK = 0,
	__LORA_PRO_CMD_ACK_STA_ERR = 1,
	__LORA_PRO_CMD_ACK_STA_WAIT = 2,
}loraProCmdAckStaEnum;


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

/*
**
*/
static u16 __LoraProCheckCode(u08 *buf, u16 len); //校验码
static void __LoraProRecDatDecode(u08 *buf, u16 len); //LoraPro模块接收数据解码

static void __LoraProAckInqLableInfor(u08 *buf, u16 len);
static void __LoraProAckClrLableLED(u08 *buf, u16 len);
static void __LoraProAckCtrlLable(u08 *buf, u16 len);
static void __LoraProAckUpdate(u08 *buf, u16 len);
static void __LoraProAckRegAddr(u08 *buf, u16 len);
static void __LoraProAckCmd(u08 *buf, u16 len);



//

static const loraProDecodeHandlerSt loraProdecodeHandlerStab[] = { //解码
	{__LORA_PRO_ACK_INQ_LABLE_INFOR,  __LoraProAckInqLableInfor}, 
	{__LORA_PRO_ACK_CTRL_LABLE,       __LoraProAckCtrlLable},
	{__LORA_PRO_ACK_CLR_LABLE_LED,       __LoraProAckClrLableLED},
	{__LORA_PRO_ACK_UPDATE,           __LoraProAckUpdate},
	{__LORA_PRO_ACK_REG_ADDR,         __LoraProAckRegAddr},
	{__LORA_PRO_ACK_CMD,              __LoraProAckCmd},
}; 

/*
**校验 码
*/
static u16 __LoraProCheckCode(u08 *buf, u16 len)
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
	
	memcpy((void *)&__loraProAckS, (const void *)buf, len);

	
	for(i = 0; i < sizeof(loraProdecodeHandlerStab) / sizeof(loraProdecodeHandlerStab[0]); i++)
	{
		if(__loraProAckS.ackCodeE == loraProdecodeHandlerStab[i].cmd)
		{
			__ClrLoraResend();
			loraProdecodeHandlerStab[i].handler(__loraProAckS.para, len - 1); //from ack status 
			break;
		}
	}
}

//查询设备基本信息
static void __LoraProAckInqLableInfor(u08 *buf, u16 len)
{
	inqTypeEnum __inqTypeE;
	
	memcpy(&__inqTypeE, buf, 1);
	
	TRACE("__inqTypeE is %d\r\n", __inqTypeE);
	
}


/*
**
*/
static void __LoraProAckClrLableLED(u08 *buf, u16 len)
{
	#if TRACE_GET_LABLE_DAT_EN
		u08 i;
		
		TRACE("get lable data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE("buf %d  %02x\r\n", i, buf[i]);
		}
	#endif
	
	__SendAckStaToPcSer(__CMD_CLR_LABEL_LED, __ACK_STA_OK, AutoSel);
}



/*
**
*/
static void __LoraProAckCtrlLable(u08 *buf, u16 len)
{
	#if TRACE_GET_LABLE_DAT_EN
		u08 i;
		
		TRACE("get lable data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE("buf %d  %02x\r\n", i, buf[i]);
		}
	#endif
	
	__SendAckStaToPcSer(__CMD_CTRL_LABEL, __ACK_STA_OK, AutoSel);
}

/*
**
*/
static void __LoraProAckUpdate(u08 *buf, u16 len)
{
	#if TRACE_GET_LABLE_DAT_EN
		u08 i;
		
		TRACE("get lable data!\r\n");
		for(i = 0; i < len; i++)
		{
			TRACE("buf %d  %02x\r\n", i, buf[i]);
		}
	#endif
	__SendAckStaToPcSer(__CMD_UPDATE_LABLE, __ACK_STA_OK, AutoSel);	
}




/*
**
*/
void CommLoraProRecDatDecode(u08 *buf, u16 len)
{
	u16 __cnt = 0;
	u16 __packLen = 0;
	u16 __ccs = 0;
	u16 i;

	for(i = 0; i < len; i++)
	{
		buf[__cnt] = buf[i];
		
		if(0 == __cnt) //header1
		{
			if(__LORA_PRO_FRAME_START1 == buf[__cnt])
			{
				__cnt++;
			}
		} 
		else if(1 == __cnt) //header2
		{
			if(__LORA_PRO_FRAME_START2 == buf[__cnt])
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
			__packLen = ((u16)buf[__cnt]) << 8;
			__cnt++;
		}
		else if(3 == __cnt) //data lenth LSB
		{
			__packLen |=  (u16)buf[__cnt];

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
		else if(__cnt == 5 + __packLen) //ccs MSB
		{
			__ccs = ((u16)buf[__cnt]) << 8;
			__cnt++;
		}
		else if(__cnt == 6 + __packLen) //ccs LSB
		{
			__ccs |= (u16)buf[__cnt];

			
			if(__ccs == __LoraProCheckCode(buf, __packLen + 5))
			{
				__LoraProRecDatDecode(&buf[4], __packLen + 1);
			}
			else
			{
				TRACE("LoraProRec Data CCS is error!\r\n");
			}
			__cnt = 0;
		}
	}
	__cnt = 0;
}


/*
**6A 01 00 06 F1 01 23 45 67 00 25 8F C6
*/
static void __LoraProAckRegAddr(u08 *buf, u16 len)
{
	__SendToPcPacketRepeatWaitAck(__TO_PC_SERVER_RESEND_REG_ADDR, __TO_PC_SERVER_MAX_SEND_REP_TIMES, 3000, 0, buf, len);
}

/*
**31 00 00 64
*/
static void __LoraProAckCmd(u08 *buf, u16 len)
{
	#if TRACE_GET_LABLE_ACK_EN
		u16 i;
	#endif

	loraProAckCodeEnum __ackCmd;
	
	__ClrLoraResend();

	#if TRACE_GET_LABLE_ACK_EN

		TRACE("ack cmd is :\r\n");

		for(i = 0; i < len; i++)
		{
			TRACE(" %02x", buf[i]);
		}
		TRACE("\r\n");
	#endif

	__ackCmd = (loraProAckCodeEnum)buf[0];

	switch(__ackCmd)
	{
		case __LORA_PRO_ACK_UPDATE:
			//__LoraProAckUpdate(buf, len);
		  commLoraCtrlS.step = COMM_LORA_STEP_UPDATA; //wzh
			break;

		default:
			break;
	}

	__SendAckToPcSer((CmdEnum)__ackCmd, &buf[1], len - 1, AutoSel);
}


