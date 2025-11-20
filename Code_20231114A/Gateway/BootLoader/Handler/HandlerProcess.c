#include "includes.h"

/*
**
*/
confLableAddrModEnum confLableAddrModE;
confLabAddrSt confLabAddrS;

void NetEnterConfLableAddrTimeoutEvent(void)
{
	static osDelaySt __osDelayS;

	//do every 1ms
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1000))
	{
		return;
	}

	if(confLabAddrS.timeout)
	{
		confLabAddrS.timeout--;
		if(0 == confLabAddrS.timeout)
		{
			TRACE("超时退出配置标签地址!\r\n");
			ExitSetLableAddrMode();
			//netSetStaE       = NET_SET_STA_WAIT;
			confLabAddrS.sta = COMM_PC_EXIT_CONF_LABLE_ADDR_MODE;
		}
	}
}

