#ifndef __COMM_LORA_PRO_H_
#define __COMM_LORA_PRO_H_

#include "myConfig.h"  //wzh

/*
**
*/
#define __LORA_PRO_FRAME_START1          0x6A   //LoraPro模块通讯帧起始字节1
#define __LORA_PRO_FRAME_START2          0x01   //LoraPro模块通讯帧起始字节2

#define __LORA_PRO_PACKET_MAX_NUM        256   //LoraPro模块通讯数据包最大长度


typedef enum{
	__LORA_PRO_ACK_INQ_LABLE_INFOR = 0X01, //
	__LORA_PRO_ACK_SET_LABLE_PARA  = 0x11, //
	__LORA_PRO_ACK_CTRL_LABLE      = 0x31, //
	__LORA_PRO_ACK_CTRL_LABLE_DIS  = 0x35, //
	__LORA_PRO_ACK_UPDATE_LABLE    = 0x70, //
	__LORA_PRO_ACK_CMD             = 0xF0,
	__LORA_PRO_ACK_REG_ADDR        = 0xF1,
}loraProAckCodeEnum;

typedef enum{ 
	__LORA_PRO_CMD_ACK        = 0xF0, //应答返回指令
	__LORA_PRO_CMD_REG_ADDR   = 0xF1, //申请分配地址
}loraProCmdEnum;  //发送指令

/*
**
*/
typedef struct{
	loraProAckCodeEnum ackCmd;
	u08 buf[__LORA_PRO_PACKET_MAX_NUM];
	u16 len;
}ackCmdToNetSt;

extern ackCmdToNetSt ackCmdToNetS;
extern u08 resetByCmdFlag;
extern u08 addrChangeFlag;

/*
**
*/
void CommLoraProRecDatDecode(u08 *dat, u16 len);
void CommLcdReSendEvent(void);
u16 __LoraProCheckCode(u08 *buf, u16 len); //校验码

#endif

