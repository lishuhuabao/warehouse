#include "includes.h"

/*
**
*/
void CommPcRecDatHandler(void)
{	
	static u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	static u16 __cnt;
	static osDelaySt __osDelayS;

	#if TRACE_REC_PC_DAT_AS_HEX_EN
		u16 i;
	#endif

	while(bspPcUartS.rxTop != bspPcUartS.rxBottom)
	{
		bspPcUartS.busyflag = true;
		bspPcUartS.rxTime = 0;	
		
		__buf[__cnt] = bspPcUartS.rxBuf[bspPcUartS.rxBottom];

		__cnt++;
		
		
		if(__cnt >= sizeof(__buf))
		{
			__cnt = 0;
		}
		
		bspPcUartS.rxBottom++;
		if(bspPcUartS.rxBottom >= sizeof(bspPcUartS.rxBuf))
		{
			bspPcUartS.rxBottom = 0;
		}
	}

	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	if(bspPcUartS.rxTime < 0xffff)
	{
		bspPcUartS.rxTime++;
	}
	
	if(0xffff == bspPcUartS.rxTime)
	{
		bspPcUartS.busyflag = false;
	}
		
	
	if(20 == bspPcUartS.rxTime && __cnt)
	{
		#if TRACE_REC_PC_DAT_AS_HEX_EN
			TRACE("PC Receive Data When Update As Hex is :\r\n");
			for(i = 0; i < __cnt; i++)
			{
				TRACE(" %02x", __buf[i]);
			}
			TRACE("\r\n");
		#endif
		
		UpdateDataProcess(__buf, __cnt);
		usr.waitTimes = 0;	
		memset(__buf, 0, sizeof(__buf));
		__cnt = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
**
*/
void CommServerRecDatHandler(void)
{	
	static u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	static u16 __cnt;
	static osDelaySt __osDelayS;
	
	static u08 __buf_err[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	static u16 __cnt_err;

	#if TRACE_REC_PC_DAT_AS_HEX_EN
		u16 i;
	#endif
  
	while(bspWifiUartS.rxTop != bspWifiUartS.rxBottom)
	{
		bspWifiUartS.rxTime = 0;
		
		__buf_err[__cnt_err] = bspWifiUartS.rxBuf[bspWifiUartS.rxBottom];
		
		if(__cnt_err >= sizeof(__buf_err))
		{
			 __cnt_err = 0;
		}
		else
		{
		   __cnt_err++;
		}
		
		
		__buf[__cnt] = bspWifiUartS.rxBuf[bspWifiUartS.rxBottom];
		
    if (false == wifiCfgS.flag ) // wifi未配置成功的情况下
		{
			__cnt = 0;
			usr.waitTimes = 0;
		}
		else
		{
			__cnt++;
		}
			
		
		if(__cnt >= sizeof(__buf))
		{
			__cnt = 0;
		}
		
		bspWifiUartS.rxBottom++;
		if(bspWifiUartS.rxBottom >= sizeof(bspWifiUartS.rxBuf))
		{
			bspWifiUartS.rxBottom = 0;
		}
	}

	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	if(bspWifiUartS.rxTime < 0xffff)
	{
		bspWifiUartS.rxTime++;
	}
	
	if( (20 == bspWifiUartS.rxTime) && __cnt_err &&  __cnt )  // 接收的数据有问题，看是否是断开连接了
	{
	  TcpClosedFlag = strstr((const char*)__buf_err, "CLOSED\r\n" ) ? 1 : 0;
		memset(__buf_err, 0, sizeof(__buf_err));
		__cnt_err = 0;
		if (TcpClosedFlag == 1)
		{
			return;
		}
	}
	
	
	if(20 == bspWifiUartS.rxTime && __cnt)
	{
		#if TRACE_REC_PC_DAT_AS_HEX_EN
			TRACE("PC Receive Data When Update As Hex is :\r\n");
			for(i = 0; i < __cnt; i++)
			{
				TRACE(" %02x", __buf[i]);
			}
			TRACE("\r\n");
		#endif
		
	  UpdateDataProcess(__buf, __cnt);
		usr.waitTimes = 0;	
		memset(__buf, 0, sizeof(__buf));
		__cnt = 0;
	}
}

/*
**398+ch Mhz
**430-440Mhz
**ch:32--42
**net:32-35
**lable:net + 3
**32, 35, 38,  41
*/
u08 GetChByAddr(u08 addr)
{
	return(addr % 128);
}
