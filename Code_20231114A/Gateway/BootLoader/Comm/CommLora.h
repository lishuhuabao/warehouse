#ifndef __COMM_LORA_H_
#define __COMM_LORA_H_

/*
**
*/
//#define __LORA_MAX_SEND_REP_TIMES     3 //无应答重发次数
#define __LORA_MAX_SEND_REP_TIMES     1 //无应答重发次数


#define CONF_LABLE_ADDR_MODE_LORA_ADDR  0x0010
#define CONF_LABLE_ADDR_MODE_LORA_CH    0x10

#define __LORA_PACKET_MAX_NUM        256   //Lora模块通讯数据包最大长度


typedef enum{
	COMM_LORA_STEP_INIT = 0,
	COMM_LORA_STEP_PC_COMM,
	COMM_LORA_STEP_UPDATA,
	COMM_LORA_STEP_MAX
}commLoraStepEnum;


typedef enum{
	__LORA_RE_SEND_PRO_DATA = 0,
	
	__LORA_MAX_RE_SEND//最大重发事件个数
}loraReSendTypeEnum; //重发指令

typedef enum{
	NET_SET_STA_INIT = 0,
	NET_SET_STA_WAIT,
}netSetStaEnum;

/*
**
*/

typedef struct{
	commLoraStepEnum step;
}commLoraCtrlSt;

typedef struct{
	loraReSendTypeEnum sendType;
	void(* sendPackt)(u08 *buf, u16 len); //发送指令函数
	u08 buf[__LORA_PACKET_MAX_NUM];
	u08 len;
	u08 reSendTimes;
	u32 sendDelay;
	u32 timeInterval;
	u32 cnt;
	u32 enCnt;
}loraSendPackListSt;

extern commLoraCtrlSt commLoraCtrlS;
extern loraSendPackListSt loraSendPackListStab[5];
extern netSetStaEnum netSetStaE;
extern u08 gobalAddrFlag;
extern u08 loraRecFlag;

/*
**
*/
void CommLoraRecDatHandler(void); 
void CommLoraReSendEvent(void);
void EnterSetLableAddrMode(void);
void ExitSetLableAddrMode(void);
void EnterUpdataLable(void);
void ExitUpdateLable(void);

void CommLoraSendPro(u08 *buf, u16 len);
void CommLoraSendUpdata(u08 *buf, u16 len);
void __ClrLoraResend(void);

#endif

