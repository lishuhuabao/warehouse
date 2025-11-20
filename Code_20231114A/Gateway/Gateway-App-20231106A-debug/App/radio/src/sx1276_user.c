#include "includes.h"

/*
**
*/
#define RF_FREQUENCY                               435575000 // 定义工作频率
//#define RF_FREQUENCY                               445575000 // 定义工作频率
#define TX_OUTPUT_POWER                            (dataSaveS.txPwr)        // 定义发送功率

#define LORA_BANDWIDTH                             2         // [0: 125 kHz, 定义带宽
                                                             //  1: 250 kHz,
                                                             //  2: 500 kHz,

#define LORA_SPREADING_FACTOR                      7         // [SF7..SF12] 定义扩频因子
#define LORA_CODINGRATE                            1         // [1: 4/5,    定义编码率
                                                             //  2: 4/6,
                                                             //  3: 4/7,
                                                             //  4: 4/8]

//#define LORA_PREAMBLE_LENGTH                       6533         // Same for Tx and Rx  定义前导码长度
#define LORA_PREAMBLE_LENGTH               250     

#define LORA_SYMBOL_TIMEOUT                        8192      // Symbols   超时符号个数

#define LORA_FIX_LENGTH_PAYLOAD_ON                 false
#define LORA_IQ_INVERSION_ON                       false

/*
**
*/
static u32 __GetSignalBw(u32 bw);
static float __GetErrCode(u32 errCode);

/*
**
*/
void delay_ms(uint32_t cnt)
{
    volatile uint32_t i = cnt * 3190;
    while(i--);
}


void OnTxDone(void)
{
	//printf("tx done\r\n");
	// SX1276SetFreq(BspLoraGetFreqByCh(dataSaveS.netCh));
	//spTimerStart(STIMER_REC,1,1);//启动休眠任务 // wzh
	SX1276SetMaxPayloadLength(MODEM_LORA, 255);  // wzh
	SX1276SetRx(0); // wzh	
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr)
{
	#if 1
		for(uint8_t i = 0;i < size;i++)
		{
			printf("%x ",*(payload+i));
		}
		printf("size = %d\r\n",size);
	#endif
	
	if ( (dataSaveS.netAddr >> 8) == RxTxBuffer[0] && (dataSaveS.netAddr & 0xff) == RxTxBuffer[1] ) //wzh
	{
	   loraRecFlag = 1;
	
     //		printf("\r\nOnRxDone\r\n");
     //		spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	
	   // wzh
	   spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	   spTimerStart(STIMER_REC,10,1);//启动休眠任务
	}
	else
  {
		 spTimerStart(STIMER_REC,1,1);
  }
}

void OnTxTimeout(void)
{
    printf("tx timeout\r\n");
	spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	spTimerStart(STIMER_REC,10,1); //wzh
}

void OnRxTimeout(void)
{
    printf("rx timeout\r\n");
	spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	spTimerStart(STIMER_REC,10,1); //wzh
}

void OnRxError(void)
{
    printf("rx error\r\n");
	  spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	  spTimerStart(STIMER_REC,10,1); //wzh
}



volatile unsigned char  gCAD = 0;//CAD检测次数
//volatile unsigned char	gCnt = 0;
void OnCadDone(u08 channelActivityDetected)
{
     printf("cad done\r\n");
	 if(channelActivityDetected)
	 {
		//gCnt = 0;
		if( gCAD == 1 )//连续两次CAD检测到信号进入接收状态
		{
			gCAD = 0;
			SX1276SetRx(3000);//CAD检测到数据开启2s接收窗
			SX1276SetMaxPayloadLength( MODEM_LORA, 255 );
		}
		else
		{
			gCAD = gCAD + 1;
			SX1276StartCad();//再次CAD检测
		}
	 }
	 else
	 {
			gCAD = 0;
		  spTimerStart(STIMER_SLEEP,1,1);//启动休眠任务
	 }
	 
}


//此函数添加到主函数初始化中，一定要加在SPI和引脚初始化之后
void SX1276AppInit(void)
{
    SX1276Init();
	
	TRACE("Lora netch:%d", dataSaveS.netCh);
	BspLoraChSet(dataSaveS.netCh);
    SX1276SetModem( MODEM_LORA );
    SX1276SetSyncWord(0x56);
	                      
    SX1276SetTxConfig(MODEM_LORA,      /*LoRa模式*/  
                      TX_OUTPUT_POWER, /*发送功率20*/
                      0,               /*频偏0(FSK)*/
                      LORA_BANDWIDTH,  /*带宽500*/									
                      LORA_SPREADING_FACTOR, /*LoRa扩频因子*/	 
                      LORA_CODINGRATE,       /*LoRa编码率*/ 								
                      LORA_PREAMBLE_LENGTH, /*LoRa前导码*/
                      LORA_FIX_LENGTH_PAYLOAD_ON, /*LoRa是否固定发送长度*/
                      true,/*CRC校验*/             
                      0,   /*LoRa跳频开关*/              
                      0,   /*LoRa跳频符号数*/      
                      LORA_IQ_INVERSION_ON, /*LoRa中断信号翻转*/
                      9000);               /*发送超时时间*/
	
											 				
    SX1276SetRxConfig(MODEM_LORA, /*LoRa模式*/ 
    				  LORA_BANDWIDTH, /*带宽500*/
    				  LORA_SPREADING_FACTOR,/*LoRa扩频因子*/							  
                      LORA_CODINGRATE,/*LoRa编码率*/ 
                      0, /*(FSK)*/
                      LORA_PREAMBLE_LENGTH,/*LoRa前导码*/				 	
                      LORA_SYMBOL_TIMEOUT,/*LoRa接收符号超时*/ 
                      LORA_FIX_LENGTH_PAYLOAD_ON,/*LoRa是否固定接收长度*/
											       
                      0,/*设置负载长度*/               
                      true,/*CRC校验*/      
                      0,/*LoRa跳频开关*/      
                      0, /*LoRa跳频符号数*/     
                      LORA_IQ_INVERSION_ON, /*LoRa中断信号翻转*/  
                      1);/*连续接收*/
//		printf("\r\nRF %d Hz,POWER = %d,BW = %d,SF = %d,CR = %f,Preamble = %d,symbol_timeout = %d\r\n",
//		            SX1276.Settings.freq, TX_OUTPUT_POWER, 
//		            __GetSignalBw(SX1276.Settings.LoRa.Bandwidth),
//		            LORA_SPREADING_FACTOR,
//		            __GetErrCode(SX1276.Settings.LoRa.Coderate),
//		            LORA_PREAMBLE_LENGTH,
//		            LORA_SYMBOL_TIMEOUT);

//		TRACE("Lora 空中速率:%d\r\n", Sx1276LoraGetAirDr());
//		TRACE("Lora Rs is %dus\r\n", Sx1276GetRsAsUs());
//		TRACE("前导码传输时长:%dMs\r\n", Sx1276PreambleKeepTimeMs());
}

/*
**
*/
u32 Sx1276LoraGetAirDr(void)
{
	u32 __dr;

	__dr = (u32)((float)SX1276.Settings.LoRa.Datarate * 
	       ((float)__GetSignalBw(SX1276.Settings.LoRa.Bandwidth) / (float)pow(2, SX1276.Settings.LoRa.Datarate)) *
	       __GetErrCode(SX1276.Settings.LoRa.Coderate));

	return __dr;
}

/*
**
**SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
**5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
*/
static u32 __GetSignalBw(u32 bw)
{
	switch(bw)
	{
		case 0:
			return 7800;
			
		case 1:
			return 10400;

		case 2:
			return 15600;

		case 3:
			return 20800;

		case 4:
			return 31200;

		case 5:
			return 41600;

		case 6:
			return 62500;

		case 7:
			return 125000;

		case 8:
			return 250000;

		case 9:
			return 500000;

		default:
			break;
	}

	return 0;
}

/*
**ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
*/
static float __GetErrCode(u32 errCode)
{
	switch(errCode)
	{
		case 1:
			return (float)((float)4 / (float)5);

		case 2:
		    return (float)((float)4 / (float)6);

		case 3:
	    	return (float)((float)4 / (float)7);

		case 4:
	    	return (float)((float)4 / (float)8);

	    default:
	    	break;
	}

	return (float)0;
}

/*
**
*/
u32 Sx1276GetRsAsUs(void)
{
	return (u32)((float)1000000 / ((float)__GetSignalBw(SX1276.Settings.LoRa.Bandwidth) / (float)pow(2, SX1276.Settings.LoRa.Datarate)));
}

/*
**
*/
u32 Sx1276PreambleKeepTimeMs(void)
{
	return SX1276.Settings.LoRa.PreambleLen * Sx1276GetRsAsUs() / 1000; 
}



