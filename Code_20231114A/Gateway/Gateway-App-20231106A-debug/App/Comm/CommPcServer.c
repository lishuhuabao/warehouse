#include "includes.h"

static void __ResendPc_ErrHandler(void);
static void __ResendServer_ErrHandler(void);

/*
**
*/
void CommPcRecDatHandler(void)
{
	static u16 __packLen = 0;
	static u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	static u16 __cnt;
	static osDelaySt __osDelayS;
	static u16 __ccs = 0;

	#if TRACE_REC_PC_DAT_AS_HEX_EN
		u16 i;
	#endif

	while(bspPcUartS.rxTop != bspPcUartS.rxBottom)
	{
		bspPcUartS.busyflag = true;
		bspPcUartS.rxTime = 0;	
		
		__buf[__cnt] = bspPcUartS.rxBuf[bspPcUartS.rxBottom];

		if(COMM_LORA_STEP_UPDATA == commLoraCtrlS.step)
		{
			__cnt++;
		}
		else
		{
			if(0 == __cnt) //header1
			{
				if(__PC_SERVER_FRAME_START1 == __buf[__cnt])
				{
					__cnt++;
				}
			} 
			else if(1 == __cnt) //header2
			{
				if(__PC_SERVER_FRAME_START2 == __buf[__cnt])
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
				__packLen = ((u16)__buf[__cnt]) << 8;
				__cnt++;
			}
			else if(3 == __cnt) //data lenth LSB
			{
				__packLen |=  (u16)__buf[__cnt];

				if(__packLen < __FROM_PC_SERVER_PACKET_MAX_NUM)
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
			else if(__cnt == 5 + __packLen) //ccs LSB
			{
				__ccs = ((u16)__buf[__cnt]) << 8;
				__cnt++;
			}
			else if(__cnt == 6 + __packLen) //ccs MSB
			{
				__ccs |= (u16)__buf[__cnt];

				#if TRACE_REC_PC_DAT_AS_HEX_EN
					TRACE("PC Receive Data As Hex is :\r\n");
					for(i = 0; i < __cnt - 1; i++)
					{
						TRACE(" %02x", __buf[i]);
					}
					TRACE("\r\n");

				TRACE("ccs is %04x\r\n", __ccs);
				#endif
				
				if(__ccs == __CrcCheck1021(__buf, __packLen + 5))
				{
					__DecodeDataFromPcSer(&__buf[4], __packLen);
				}
				else 
				{
					TRACE("ccs is err in 592!\r\n");
				}
				__cnt = 0;
			}
		}
		
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
		
		if(COMM_LORA_STEP_UPDATA == commLoraCtrlS.step)
		{
			//6A 01 00 03 70 00 34 01 D1 72
			if((11   == __cnt &&
			   0x6A == __buf[0] &&
			   0x01 == __buf[1] &&
			   0x00 == __buf[2] &&
			   0x04 == __buf[3] &&
			   0x70 == __buf[4] &&
			   0x00 == __buf[8])) //||
			   //(1 == __cnt && 0x04 == __buf[0]))
			   
			{
				// EnterUpdataLable(); wzh
				commLoraCtrlS.step = COMM_LORA_STEP_PC_COMM; //wzh
				ExitUpdateLable();//wzh
			}
			else
			{
				CommLoraSendUpdata(__buf, __cnt);
				if((1 == __cnt && 0x04 == __buf[0]))
				{
					exitUpdata = 1;
				}
			}
		}
		memset(__buf, 0, sizeof(__buf));
		__cnt = 0;
	}
}

/*
**
*/
void CommPcReSendEvent(void)
{
	static osDelaySt __osDelayS;

	//do every 1ms
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}
	
	if(netToPcPackListStab[0].cnt) 
	{
		if(0 == netToPcPackListStab[0].cnt % netToPcPackListStab[0].timeInterval &&
			netToPcPackListStab[0].cnt <= netToPcPackListStab[0].enCnt)
		{
			toPcSerReSendHandlerStab[netToPcPackListStab[0].sendType].sendPackt(netToPcPackListStab[0].buf, netToPcPackListStab[0].len);
		}
		
		netToPcPackListStab[0].cnt--;

		if(0 == netToPcPackListStab[0].cnt)
		{
			TRACE("SEND is not receve is \r\n");
			memset(&netToPcPackListStab, 0, sizeof(netToPcPackListStab));
			__ResendPc_ErrHandler();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
**
*/
void CommServerRecDatHandler(void)
{
	static u16 __packLen = 0;
	static u08 __buf[__FROM_PC_SERVER_PACKET_MAX_NUM] = {0};
	static u16 __cnt;
	static osDelaySt __osDelayS;
	static u16 __ccs = 0;
	
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
		}
		else if (COMM_LORA_STEP_UPDATA == commLoraCtrlS.step)
		{
			__cnt++;
		}
		else
		{
			if(0 == __cnt) //header1
			{
				if(__PC_SERVER_FRAME_START1 == __buf[__cnt])
				{
					__cnt++;
				}
			} 
			else if(1 == __cnt) //header2
			{
				if(__PC_SERVER_FRAME_START2 == __buf[__cnt])
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
				__packLen = ((u16)__buf[__cnt]) << 8;
				__cnt++;
			}
			else if(3 == __cnt) //data lenth LSB
			{
				__packLen |=  (u16)__buf[__cnt];

				if(__packLen < __FROM_PC_SERVER_PACKET_MAX_NUM)
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
			else if(__cnt == 5 + __packLen) //ccs LSB
			{
				__ccs = ((u16)__buf[__cnt]) << 8;
				__cnt++;
			}
			else if(__cnt == 6 + __packLen) //ccs MSB
			{
				__ccs |= (u16)__buf[__cnt];

				#if TRACE_REC_PC_DAT_AS_HEX_EN
					TRACE("PC Receive Data As Hex is :\r\n");
					for(i = 0; i < __cnt - 1; i++)
					{
						TRACE(" %02x", __buf[i]);
					}
					TRACE("\r\n");

				TRACE("ccs is %04x\r\n", __ccs);
				#endif
				
				if(__ccs == __CrcCheck1021(__buf, __packLen + 5))
				{
					  if (true == bspPcUartS.busyflag)
            {
	              __SendAckStaToPcSer((CmdEnum)__buf[4], __ACK_STA_BUSY, SendToWifi);		        
	          }
				    else
					  {
						    __DecodeDataFromPcSer(&__buf[4], __packLen);
				    }					
				}
				else 
				{
					TRACE("ccs is err in 592!\r\n");
				}
				__cnt = 0;
			}
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
	
	if( (20 == bspWifiUartS.rxTime) && __cnt_err && (0 == __cnt) )  // 接收的数据有问题，看是否是断开连接了
	{
	  TcpClosedFlag = strstr((const char*)__buf_err, "CLOSED\r\n" ) ? 1 : 0;
		memset(__buf_err, 0, sizeof(__buf_err));
		__cnt_err = 0;
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
		
		if(COMM_LORA_STEP_UPDATA == commLoraCtrlS.step)
		{
			//6A 01 00 03 70 00 34 01 D1 72
			if((11   == __cnt &&
			   0x6A == __buf[0] &&
			   0x01 == __buf[1] &&
			   0x00 == __buf[2] &&
			   0x04 == __buf[3] &&
			   0x70 == __buf[4] &&
			   0x00 == __buf[8])) //||
			   //(1 == __cnt && 0x04 == __buf[0]))
			   
			{
				// EnterUpdataLable(); wzh
				commLoraCtrlS.step = COMM_LORA_STEP_PC_COMM; //wzh
				ExitUpdateLable();//wzh
			}
			else
			{
				CommLoraSendUpdata(__buf, __cnt);
				if((1 == __cnt && 0x04 == __buf[0]))
				{
					exitUpdata = 1;
				}
			}
		}
		memset(__buf, 0, sizeof(__buf));
		__cnt = 0;
	}
}

/*
**
*/
void CommServerReSendEvent(void)
{
	static osDelaySt __osDelayS;

	//do every 1ms
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}
	
	if(netToServerPackListStab[0].cnt) 
	{
		if(0 == netToServerPackListStab[0].cnt % netToServerPackListStab[0].timeInterval &&
			netToServerPackListStab[0].cnt <= netToServerPackListStab[0].enCnt)
		{
			toPcSerReSendHandlerStab[netToServerPackListStab[0].sendType].sendPackt(netToServerPackListStab[0].buf, netToServerPackListStab[0].len);
		}
		
		netToServerPackListStab[0].cnt--;

		if(0 == netToServerPackListStab[0].cnt)
		{
			TRACE("SEND is not receve is \r\n");
			memset(&netToServerPackListStab, 0, sizeof(netToServerPackListStab));
			__ResendServer_ErrHandler();
		}
	}
}

/*
**
*/
static void __ResendPc_ErrHandler()
{

}

/*
**
*/
static void __ResendServer_ErrHandler()
{

}
