#ifndef __HANDLER_PROCESS_H_
#define __HANDLER_PROCESS_H_
/*
**
*/

typedef enum{
	COMM_PC_EXIT_CONF_LABLE_ADDR_MODE = 0,
	COMM_PC_ENTER_CONF_LABLE_ADDR_MODE,
}confLableAddrModEnum;

typedef struct{
	confLableAddrModEnum sta;
	u32 timeout;
}confLabAddrSt;

void NetEnterConfLableAddrTimeoutEvent(void);



#endif

