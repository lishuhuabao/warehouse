#include "includes.h"

#include <string.h>

bool TcpClosedFlag = false;
wifiCfgSt wifiCfgS;

const unsigned char ESP12F_ssid[] = "JMUWZH";//"ZB_IOT";										// 此处填写无线AP名称
const unsigned char ESP12F_password[] = "www.542821";//"ZB_IOT666";								// 此处填写无线AP密码
const unsigned char ESP12F_ip[] = "192.168.1.103";//"192.168.1.48";									// 此处填写服务器IP地址
const unsigned char ESP12F_port[] = "8234";//"6000";										// 此处填写服务器端口

void esp12f_send_byte(unsigned char c);
void esp12f_send_string(unsigned char* s);
bool esp12f_cmd_wait_ack(unsigned char *ack1, unsigned char *ack2, u32 timeout);

static void  __ESP12F_RESET_LOW(void); // 0
static void  __ESP12F_RESET_HIGH(void); // 1
static void  __ESP12F_RESET_DelayForReady(void);  // 2

static void  __ESP12F_AutoConn_DISABLE(void);
static void  __ESP12F_AutoConn_DISABLE_DelayForReady(void);	
	
static void  __ESP12F_STA_Mode_SET(void); 
static void  __ESP12F_STA_Mode_SET_DelayForReady(void); 
	
static void  __ESP12F_JoinAP(void);  
static void  __ESP12F_JoinAP_DelayForReady(void); 
	
static void  __ESP12F_SINGLE_ID_SET(void); 
static void  __ESP12F_SINGLE_ID_SET_DelayForReady(void); 

static void  __ESP12F_Link_Server(void); 
static void  __ESP12F_Link_Server_DelayForReady(void); 

static void  __ESP12F_UnvarnishSendMode_SET(void); 
static void  __ESP12F_UnvarnishSendMode_SET_DelayForReady(void); 

static void  __ESP12F_UnvarnishSend(void); 
static void  __ESP12F_UnvarnishSend_DelayForReady(void); 	

static void  __Wait_Branch_State(void);

static void  __ESP12F_ExitUnvarnishSend(void); 
static void  __ESP12F_ExitUnvarnishSend_DelayForReady(void); 
	
static void  __ESP12F_Get_LinkStatus(void); 
static void  __ESP12F_Get_LinkStatus_DelayForReady(void); 

/*--------------------------------------------
note: esp12f配置流程
--------------------------------------------*/
typedef enum{
	__STEP_RESET_LOW = 0, // 0
	__STEP_RESET_HIGH, // 1
	__STEP_RESET_DelayForReady, // 2
	
	__STEP_STA_Mode_SET,  // 5
  __STEP_STA_Mode_SET_DelayForReady, // 6	
	
	__STEP_JoinAP_1,   // 7
	__STEP_JoinAP_DelayForReady_1, // 8
	
	__STEP_AutoConn_DISABLE, // 3
	__STEP_AutoConn_DISABLE_DelayForReady, // 4
	
	__STEP_SINGLE_ID_SET,  // 9
  __STEP_SINGLE_ID_SET_DelayForReady, // 10

  __STEP_Link_Server_1, // 11
	__STEP_Link_Server_DelayForReady_1, // 12
	
	__STEP_UnvarnishSendMode_SET_1, // 13
	__STEP_UnvarnishSendMode_SET_DelayForReady_1, // 14
	
	__STEP_UnvarnishSend_1, // 15
	__STEP_UnvarnishSend_DelayForReady_1, // 16
	
	__STEP_Wait_Branch_State_1,  // 17
	
	__STEP_ExitUnvarnishSend, // 18
	__STEP_ExitUnvarnishSend_DelayForReady, // 19
	
	__STEP_Get_LinkStatus, // 20
	__STEP_Get_LinkStatus_DelayForReady, // 21
	
	__STEP_JoinAP_2,  // 22
	__STEP_JoinAP_DelayForReady_2, // 23
	
	__STEP_Link_Server_2, // 24
	__STEP_Link_Server_DelayForReady_2, // 25
	
	__STEP_UnvarnishSendMode_SET_2, // 26
	__STEP_UnvarnishSendMode_SET_DelayForReady_2, // 27
	
	__STEP_UnvarnishSend_2, // 28
	__STEP_UnvarnishSend_DelayForReady_2, // 29
	
	__STEP_Wait_Branch_State_2, // 30
	
}Esp12fConfigStepEnum;

static void (* Esp12fConfigFlow[])(void) = {
	__ESP12F_RESET_LOW, // 0
	__ESP12F_RESET_HIGH, // 1
	__ESP12F_RESET_DelayForReady, // 2
	
	__ESP12F_STA_Mode_SET,  // 5
  __ESP12F_STA_Mode_SET_DelayForReady, // 6	
	
	__ESP12F_JoinAP,   // 7
	__ESP12F_JoinAP_DelayForReady, // 8
	
	__ESP12F_AutoConn_DISABLE, // 3
	__ESP12F_AutoConn_DISABLE_DelayForReady, // 4
	
	__ESP12F_SINGLE_ID_SET,  // 9
  __ESP12F_SINGLE_ID_SET_DelayForReady, // 10

  __ESP12F_Link_Server, // 11
	__ESP12F_Link_Server_DelayForReady, // 12
	
	__ESP12F_UnvarnishSendMode_SET, // 13
	__ESP12F_UnvarnishSendMode_SET_DelayForReady, // 14
	
	__ESP12F_UnvarnishSend, // 15
	__ESP12F_UnvarnishSend_DelayForReady, // 16
	
	__Wait_Branch_State,  // 17
	
	__ESP12F_ExitUnvarnishSend, // 18
	__ESP12F_ExitUnvarnishSend_DelayForReady, // 19
	
	__ESP12F_Get_LinkStatus, // 20
	__ESP12F_Get_LinkStatus_DelayForReady, // 21
	
	__ESP12F_JoinAP,  // 22
	__ESP12F_JoinAP_DelayForReady, // 23
	
	__ESP12F_Link_Server, // 24
	__ESP12F_Link_Server_DelayForReady, // 25
	
	__ESP12F_UnvarnishSendMode_SET, // 26
	__ESP12F_UnvarnishSendMode_SET_DelayForReady, // 27
	
	__ESP12F_UnvarnishSend, // 28
	__ESP12F_UnvarnishSend_DelayForReady, // 29
	
	__Wait_Branch_State, // 30
	
};

/*--------------------------------------------
note: 向esp12f发送字符
--------------------------------------------*/
void esp12f_send_byte(unsigned char c)
{
	HAL_UART_Transmit(&bspWifiUartHandler, &c, 1, 1000);
}

/*--------------------------------------------
note: 向esp12f发送字符串
--------------------------------------------*/
void esp12f_send_string(unsigned char* s)
{
	while(*s)
	{
		HAL_UART_Transmit(&bspWifiUartHandler, s++, 1, 1000);
	}
}

/*
**
*/ 
bool esp12f_cmd_wait_ack(unsigned char *ack1, unsigned char *ack2, u32 timeout)
{
  static osDelaySt __osDelayS;
	
	if(NULL == ack1 && NULL == ack2)     //不需要接收数据
  {
    wifiCfgS.step++;
		wifiCfgS.cnt  = 0;
		return true;
  }
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return false;
	}
	
	wifiCfgS.cnt++;
	
	//if( (200 == bspWifiUartS.rxTime ) && ( bspWifiUartS.rxTop > 0) )
	if(wifiCfgS.cnt >= timeout)	
	{
	   bspWifiUartS.rxBuf[bspWifiUartS.rxTop] = '\0';
		
		 if (  ((NULL != ack1) && (NULL != ack2) && ( (strstr((const char*)bspWifiUartS.rxBuf, (const char*)ack1)) || (strstr((const char*)bspWifiUartS.rxBuf, (const char*)ack2)) ) ) 
	      || ((NULL != ack1) && (NULL == ack2) && (strstr((const char*)bspWifiUartS.rxBuf, (const char*)ack1)) )
	      || ((NULL == ack1) && (NULL != ack2) && (strstr((const char*)bspWifiUartS.rxBuf, (const char*)ack2)) ) )
	   {		
			 wifiCfgS.step++;
			 wifiCfgS.cnt  = 0;
			 return true;
		 }
		 else
		 {
		   wifiCfgS.step -= 1;                //超时退后一步//重新开始
		   wifiCfgS.cnt  = 0;			 
		 }
	}
		
	//if(wifiCfgS.cnt >= timeout)	
	//{			
	//	wifiCfgS.step -= 1;                //超时退后一步//重新开始
	//	wifiCfgS.cnt  = 0;
	//}	
	
	return false;
}

/*--------------------------------------------
note: 配置esp12f
--------------------------------------------*/
void esp12f_config()
{
	static u08 pre_step;
	if ( (false  == wifiCfgS.flag) || ( true == TcpClosedFlag) ) 
	{
		Esp12fConfigFlow[wifiCfgS.step]();
		if (pre_step != wifiCfgS.step)
		{
		  TRACE("STEP = %d\r\n",wifiCfgS.step);
			pre_step = wifiCfgS.step;
		}
		
	}	
}

/*
**
*/
static void  __ESP12F_RESET_LOW(void) // 0
{
	BSP_WIFI_RST_(0);
  wifiCfgS.step++;
}

/*
**
*/
static void  __ESP12F_RESET_HIGH(void) // 1
{
  static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	wifiCfgS.cnt++;
	if(wifiCfgS.cnt >= 500)
	{		
		//TRACE("step is %d\r\n", wifiCfgS.step);
		BSP_WIFI_RST_(1);
		
		wifiCfgS.step++;
		wifiCfgS.cnt  = 0;
	}

}

/*
**
*/
static void  __ESP12F_RESET_DelayForReady(void)  // 2
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	wifiCfgS.cnt++;
	if(wifiCfgS.cnt >= 1000)
	{		
		//TRACE("step is %d\r\n", wifiCfgS.step);
		wifiCfgS.step++;
		wifiCfgS.cnt  = 0;
	}
}

/*
**
*/
static void  __ESP12F_AutoConn_DISABLE(void)
{
	bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;
  esp12f_send_string((unsigned char*)"AT+CWAUTOCONN=0\r\n");  // 上电不自动连接wifi
	wifiCfgS.step++;
}
	
/*
**
*/
static void  __ESP12F_AutoConn_DISABLE_DelayForReady(void)
{
  esp12f_cmd_wait_ack((unsigned char *)"OK", NULL, 500);	
}

/*
**
*/
static void  __ESP12F_STA_Mode_SET(void)
{
	bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;
	esp12f_send_string((unsigned char*)"AT+CWMODE=1\r\n");  // 设置为station模式
	wifiCfgS.step++;
}	

/*
**
*/
static void  __ESP12F_STA_Mode_SET_DelayForReady(void)
{
	esp12f_cmd_wait_ack((unsigned char *)"OK",(unsigned char *)"no change", 2500);
}	

/*
**
*/
static void  __ESP12F_JoinAP(void)
{
	char cCmd [120];
	bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;	
	sprintf(cCmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", ESP12F_ssid, ESP12F_password);   // 连接AP
	esp12f_send_string((unsigned char*)cCmd);
	wifiCfgS.step++;
}

/*
**
*/
static void  __ESP12F_JoinAP_DelayForReady(void)
{
  esp12f_cmd_wait_ack((unsigned char *)"OK",NULL, 10000);
}	 

/*
**
*/
static void  __ESP12F_SINGLE_ID_SET(void)
{
	bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;		
	esp12f_send_string((unsigned char*)"AT+CIPMUX=0\r\n"); // 单一连接
	wifiCfgS.step++;
}	

/*
**
*/
static void  __ESP12F_SINGLE_ID_SET_DelayForReady(void)
{
	esp12f_cmd_wait_ack((unsigned char *)"OK",NULL, 500);
}

/*
**
*/
static void  __ESP12F_Link_Server(void)
{
  u08 ESP12F_id = 0;
  char cCmd [120];
	memset(bspWifiUartS.rxBuf, 0, sizeof(bspWifiUartS.rxBuf));
	bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;	
  //sprintf(cCmd, "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n",ESP12F_id,"TCP", (char*)ESP12F_ip, (char*)ESP12F_port); //连接服务器
  sprintf(cCmd, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n","TCP", (char*)ESP12F_ip, (char*)ESP12F_port);
  esp12f_send_string((unsigned char*)cCmd);
	wifiCfgS.step++;
}


/*
**
*/
static void  __ESP12F_Link_Server_DelayForReady(void)
{
  esp12f_cmd_wait_ack((unsigned char *)"OK",(unsigned char *)"ALREAY CONNECT", 4000);
}

/*
**
*/
static void  __ESP12F_UnvarnishSendMode_SET(void)
{
  bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;	
  esp12f_send_string((unsigned char*)"AT+CIPMODE=1\r\n");   //开启透传模式
	wifiCfgS.step++;
}

/*
**
*/
static void  __ESP12F_UnvarnishSendMode_SET_DelayForReady(void)
{
	esp12f_cmd_wait_ack((unsigned char *)"OK",NULL, 500);
}

/*
**
*/
static void  __ESP12F_UnvarnishSend(void)
{
  bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;	
  esp12f_send_string((unsigned char*)"AT+CIPSEND\r\n");   //开始透传
	wifiCfgS.step++;
}

/*
**
*/
static void  __ESP12F_UnvarnishSend_DelayForReady(void)
{
	esp12f_cmd_wait_ack((unsigned char *)">",NULL, 1000);
}

/*
**
*/
static void  __Wait_Branch_State(void)
{
	wifiCfgS.flag = true;	
  if ( true == TcpClosedFlag)
	{
	   wifiCfgS.step = __STEP_ExitUnvarnishSend; // 退出透传模式
		 wifiCfgS.flag = false;
	}
}

/*
**
*/
static void  __ESP12F_ExitUnvarnishSend(void)
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	wifiCfgS.cnt++;
	if(wifiCfgS.cnt >= 1000)
	{		
		esp12f_send_string((unsigned char*)"+++");   //退出透传模式
		wifiCfgS.step++;
		wifiCfgS.cnt  = 0;
	}	
}	

/*
**
*/
static void  __ESP12F_ExitUnvarnishSend_DelayForReady(void)
{
	static osDelaySt __osDelayS;
	
	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 1))
	{
		return;
	}

	wifiCfgS.cnt++;
	if(wifiCfgS.cnt >= 500)
	{		
		wifiCfgS.step++;
		wifiCfgS.cnt  = 0;
	}
}

/*
**
*/
static void  __ESP12F_Get_LinkStatus(void)
{
  bspWifiUartS.rxTop = 0;
	bspWifiUartS.rxTime = 0;	
  esp12f_send_string((unsigned char*)"AT+CIPSTATUS\r\n");   //获取连接状态
	wifiCfgS.step++;
}

/*
**
*/
static void  __ESP12F_Get_LinkStatus_DelayForReady(void)
{
  if ( esp12f_cmd_wait_ack((unsigned char *)"OK",NULL, 500) )
	{
	  if ( strstr((const char*)bspWifiUartS.rxBuf, "STATUS:4\r\n" ) )
		{
		  wifiCfgS.step = __STEP_JoinAP_2;  // 重连AP
		}
		else
		{
		  wifiCfgS.step = __STEP_UnvarnishSendMode_SET_2;  // 开启透传模式
		}
	}
	
}
