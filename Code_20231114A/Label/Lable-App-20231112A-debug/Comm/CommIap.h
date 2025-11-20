#ifndef		__COMM_IAP_H_
#define		__COMM_IAP_H_

/*
**
*/
#define		APP_UPDATE_OTP			1

#define IAP_GET_DAT_MAX_REPEAT_TIMES   5
#define IAP_GET_DAT_WAIT_TIME          3// 3S
#define PRO_DAT_PACK_LEN_128           128

#define SOH_128               0x01
#define SOH_1K                0x02
#define EOT                   0X04
#define ACK                   0X06
#define NAK                   0X15
//#define CAN                   0X18

/*
**
*/
typedef enum{
	XMODE_NONE = 0,
	XMODE_READY,
	XMODE_START,
	XMODE_END
}xmodeStepEnum;


typedef enum{
	UPDATE_COMP = 0,
	UPDATE_BANK_OK,
	UPDATE_BANK_ERR,
	UPDATE_ERR,
}updateStaEnum;


typedef enum{
	UPDATE_NONE = 0,
	UPDATING,
	UPDAT_ERR_EXIT,
	UPDATE_OVER,
}updateStepEnum;

typedef struct{
	u32 waitTimes;
}usrSt;

extern usrSt usr;
extern u08 loraSendFlag;
extern u16 prePackNo;

void UpdateDataProcess(u08 *dat, u16 len);
void GetUpdatPacketDatTimeout(void);
void __UpdateResHandler(updateStaEnum sta);

#endif

