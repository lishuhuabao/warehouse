#ifndef __COMM_LORA_H_
#define __COMM_LORA_H_

/*
**
*/
#define COMM_LORA_BUF_LEN_TYPE     u08

#define __LORA_MAX_SEND_REP_TIMES     3 //无应答重发次数

#define __LORA_PACKET_MAX_NUM        256   //Lora模块通讯数据包最大长度

#define __LORA_NO_SLEEP              1


typedef enum{
	COMM_LORA_STEP_INIT = 0,
	COMM_LORA_STEP_ENTER_SET1,
	COMM_LORA_STEP_ENTER_SET2,
	COMM_LORA_STEP_SET_WK_MODE,
	COMM_LORA_STEP_SET_P_MODE,
	COMM_LORA_STEP_SET_WAKE_TIME,
	COMM_LORA_STEP_SET_SPD_LEV,
	COMM_LORA_STEP_SET_CH,
	COMM_LORA_STEP_SET_ADDR,
	COMM_LORA_STEP_SET_IDLE_TIME,
	COMM_LORA_STEP_RESET,

	COMM_LORA_STEP_ENTER_SET1_BY_REG_ADDR,
	COMM_LORA_STEP_ENTER_SET2_BY_REG_ADDR,
	COMM_LORA_STEP_SET_WU_MODE_BY_REG_ADDR,
	COMM_LORA_STEP_SET_ADDR_BY_REG_ADDR,
	COMM_LORA_STEP_SET_CH_BY_REG_ADDR,
	COMM_LORA_STEP_RESET_BY_REG_ADDR,

	COMM_LORA_STEP_ENTER_SET1_BY_SET_ADDR,
	COMM_LORA_STEP_ENTER_SET2_BY_SET_ADDR,
	COMM_LORA_STEP_SET_LR_MODE_BY_SET_ADDR,
	COMM_LORA_STEP_SET_ADDR_BY_SET_ADDR,
	COMM_LORA_STEP_SET_CH_BY_SET_ADDR,
	COMM_LORA_STEP_RESET_BY_SET_ADDR,

	COMM_LORA_STEP_ENTER_SET1_BY_INTO_WU_MODE,
	COMM_LORA_STEP_ENTER_SET2_BY_INTO_WU_MODE,
	COMM_LORA_STEP_SET_WU_MODE_BY_INTO_WU_MODE,
	COMM_LORA_STEP_RESET_BY_INTO_WU_MODE,

	COMM_LORA_STEP_ENTER_SET1_BY_INTO_LR_MODE,
	COMM_LORA_STEP_ENTER_SET2_BY_INTO_LR_MODE,
	COMM_LORA_STEP_SET_LR_MODE_BY_INTO_LR_MODE,
	COMM_LORA_STEP_RESET_BY_INTO_LR_MODE,

	COMM_LORA_STEP_PC_COMM,
	COMM_LORA_STEP_UPDATA,
	COMM_LORA_STEP_MAX
}commLoraStepEnum;


typedef enum{
	__LORA_RE_SEND_PRO_DATA =0,
	__LORA_MAX_RE_SEND//最大重发事件个数
}loraReSendTypeEnum; //重发指令

typedef enum{
	NET_SET_STA_INIT = 0,
	NET_SET_STA_WAIT,
}netSetStaEnum;

typedef enum{
	LORA_WORK_WU_MODE = 0,
	LORA_WORK_LR_MODE,
}loraPwrModeEnum;

typedef enum{
	__LORA_PRO_CMD_ACK_STA_OK = 0,
	__LORA_PRO_CMD_ACK_STA_ERR = 1,
	__LORA_PRO_CMD_ACK_STA_WAIT = 2,
}loraProCmdAckStaEnum;


/*
**
*/
typedef struct{
	commLoraStepEnum step;
	u08 para[__LORA_PACKET_MAX_NUM];
	loraPwrModeEnum loraPwrMode;
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

extern loraSendPackListSt loraSendPackListStab[10];

extern netSetStaEnum netSetStaE;
extern u08 loraRecFlag;
extern u08 gobalAddrFlag;

/*
**
*/
void CommLoraRecDatHandler(void); 
void CommLoraReSendEvent(void);

void CommLoraSendPro(u08 *buf, u16 len);

void CommLoraEnterSetByRegAddr(u08 *buf, u16 len);
void CommLoraEnterSetBySetAddr(u08 *buf, u16 len);
void CommLoraEnterSetByIntoWuMode(u08 *buf, u16 len);
void CommLoraEnterSetByIntoLrMode(u08 *buf, u16 len);


void __ClrLoraResend(void);
u08 GetChByAddr(u08 addr);
void __LoraSendProData(u08 *buf, u16 len);//


#endif

