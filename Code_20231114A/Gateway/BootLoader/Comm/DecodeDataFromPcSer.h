#ifndef __DECODE_DATA_FROM_PC_SER_H_
#define __DECODE_DATA_FROM_PC_SER_H_


/*
**
*/
#define __FROM_PC_SERVER_PACKET_MAX_NUM        256   //Pc模块通讯数据包最大长度
#define __TO_PC_SERVER_PACKET_MAX_NUM          256
#define __TO_PC_SERVER_MAX_SEND_REP_TIMES      1 //无应答重发次数

#define __PC_SERVER_FRAME_START1          0x6A   //Pc模块通讯帧起始字节1
#define __PC_SERVER_FRAME_START2          0x01   //Pc模块通讯帧起始字节2

#define __NET_ADDR_START           0x01
#define __NET_ADDR_END             0xFF

/*
**
*/

typedef enum{
	__TO_PC_SERVER_RESEND_REG_ADDR,
	__TO_PC_SERVER_RESEND_MAX,  //最大重发事件个数
}toPcSerReSendTypeEnum; //重发指令

typedef enum{
	__TO_PC_SERVER_CMD_REG_ADDR = 0xF1,
}reqToPcSerCmdEnum;

typedef enum{ 
	__FROM_PC_SERVER_CMD_ACK        = 0xF0, //应答返回指令
}ackFromPcSerCmdEnum;  //发送指令


typedef enum{
	__CMD_INQ_NET_INFOR          = 0X00, //查询设备基本信息
	__CMD_INQ_LABLE_INFOR        = 0x01,
	__CMD_SET_NET_PARA           = 0x10, //设置网关基本参数
	__CMD_SET_LABLE_PARA         = 0x11, //设置标签基本参数
	__CMD_UPDATE_NET             = 0x20, //设置进入网关升级模式
	__CMD_CTRL_LABEL             = 0x31, //控制电子标签动作
	__CMD_CTRL_LABEL_DIS         = 0x35, 
	__CMD_UPDATE_LABLE           = 0x70, 
	__CMD_SET_LABLE_ADDR         = 0xF1,
	__CMD_ACK                    = 0xF0,
}CmdEnum;


typedef enum{
	__ACK_STA_OK      = 0x00,
	__ACK_STA_ERR     = 0x01,
	__ACK_STA_WAIT    = 0x02,
	__ACK_STA_TIMEOUT = 0x08,
	__ACK_STA_GOBAL   = 0x40,
	__ACK_STA_OTHER_ERR = 0X80,
	__ACK_STA_BUSY    = 0xFF,
}AckStaEnum;


typedef enum{
	__SET_LABLE_PARA_ADDR        = 0x04,
	__SET_LABLE_PARA_LED         = 0x06,
	__SET_LABLE_PARA_BUZ         = 0x07,
	__SET_LABLE_PARA_WAIT_SLEEP  = 0x08,
	__SET_LABLE_PARA_TX_PWR      = 0x09,
}setLableParaTypeEnum;

typedef enum{
	__CTRL_LABLE_LED = 0x06,
	__CTRL_LABLE_BUZ = 0x07,
}ctrlLableTypeEnum;

typedef enum{
	__SET_NET_PARA_ADDR = 0x04,
	__SET_NET_PARA_LED  = 0x06,
	__SET_NET_PARA_BUZ  = 0x07,
	__SET_NET_PARA_TX_PWR = 0x09,
}setNetParaTypeEnum;

typedef enum{
	__INQ_TYPE_DEV_TYPE = 0,
	__INQ_TYPE_HW_VER = 1,
	__INQ_TYPE_TEST_VER = 2,
	__INQ_TYPE_RELEASE_VER = 3,
	__INQ_TYPE_DEV_ADDR = 4,
	__INQ_TYPE_WIGHT = 5,
	__INQ_TYPE_OPEN_TIMES = 6,
	__INQ_TYPE_TX_PWR     = 9,
}inqTypeEnum;


typedef struct{
	toPcSerReSendTypeEnum sendType;
	void(* sendPackt)(u08 *buf, u16 len); //发送指令函数
	u08 buf[__TO_PC_SERVER_PACKET_MAX_NUM];
	u08 len;
	u08 reSendTimes;
	u32 sendDelay;
	u32 timeInterval;
	u32 cnt;
	u32 enCnt;
}toPcSerReSendPackListSt;

extern toPcSerReSendPackListSt netToPcPackListStab[5];
extern toPcSerReSendPackListSt netToServerPackListStab[5];

typedef struct{
	toPcSerReSendTypeEnum sendType;
	void(* sendPackt)(u08 *buf, u16 len); //发送指令函数
}toPcSerReSendHandlerSt;

extern const toPcSerReSendHandlerSt toPcSerReSendHandlerStab[];

typedef enum{
	AutoSel = 0,
	SendToPc,
	SendToWifi,
}DestinationForUart;

extern u16 slaveAddr;
extern u08 slaveCh;
extern u08 exitUpdata;


/*
**
*/
u16 __CrcCheck1021(u08 *buf, u16 len);
void __DecodeDataFromPcSer(u08 *buf, u16 len);

void __SendAckToPcSer(CmdEnum cmd, u08 *buf, u16 len,DestinationForUart des);
void __SendAckStaToPcSer(CmdEnum cmd, AckStaEnum sta,DestinationForUart des);

void __SendToServerPacketRepeatWaitAck(toPcSerReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len);
void __SendToPcPacketRepeatWaitAck(toPcSerReSendTypeEnum type, u08 resendTimes, u32 interTime, u32 delayTime, u08 *buf, u16 len);

void PcSerUartSendPacket(u08 *buf, u16 len,DestinationForUart des);
u08 GetChByAddr(u08 addr);

#endif
