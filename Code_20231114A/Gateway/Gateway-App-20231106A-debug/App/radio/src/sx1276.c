#include "includes.h"

/*
 * Local types definition
 */

/*!
 * Radio registers definition
 * 无线寄存器定义
 */
typedef struct
{
    RadioModems_t Modem;
    uint8_t       Addr;
    uint8_t       Value;
}RadioRegisters_t;

/*!
 * FSK bandwidth definition
 * FSK带宽定义
 */
typedef struct
{
    uint32_t bandwidth;
    uint8_t  RegValue;
}FskBandwidth_t;




__IO uint8_t  SX1276EventIrqFlag[EVENT_NUM] = {0};
/*
 * Private functions prototypes
 */

/*!
 * Performs the Rx chain calibration for LF and HF bands (对LF和HF波段进行RX链校准)
 * \remark Must be called just after the reset so all registers are at their
 *         default values （在重置后调用，所以所有寄存器都处于默认值。）
 */
static void RxChainCalibration( void );

/*!
 * \brief Resets the SX1276
 */
void SX1276Reset( void );

/*!
 * \brief Sets the SX1276 in transmission mode for the given time(设置SX1276发送超时时间)
 * \param [IN] timeout Transmission timeout [ms] [0: continuous, others timeout]
 * \param [IN] 发送超时时间                  [ms] [0: 持续发送, 超时时间]
 */
void SX1276SetTx( uint32_t timeout );

/*!
 * \brief Writes the buffer contents to the SX1276 FIFO
 *
 * \param [IN] buffer Buffer containing data to be put on the FIFO.
 * \param [IN] size Number of bytes to be written to the FIFO
 */
void SX1276WriteFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads the contents of the SX1276 FIFO
 *
 * \param [OUT] buffer Buffer where to copy the FIFO read data.
 * \param [IN] size Number of bytes to be read from the FIFO
 */
void SX1276ReadFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the SX1276 operating mode
 *
 * \param [IN] opMode New operating mode
 */
void SX1276SetOpMode( uint8_t opMode );

/*
 * SX1276 DIO IRQ callback functions prototype
 */

/*!
 * \brief DIO 0 IRQ callback
 */
void SX1276OnDio0Irq( void );


/*!
 * \brief Tx & Rx timeout timer callback
 */
static void __SX1276OnTimeoutIrq( void );



/*
 * Private global constants
 */

/*!
 * Radio hardware registers initialization
 *
 * \remark RADIO_INIT_REGISTERS_VALUE is defined in sx1276-board.h file
 */
const RadioRegisters_t RadioRegsInit[] = RADIO_INIT_REGISTERS_VALUE;

/*!
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET_LF                              -164
#define RSSI_OFFSET_HF                              -157

/*!
 * Precomputed FSK bandwidth registers values
 */
const FskBandwidth_t FskBandwidths[] =
{
    { 2600  , 0x17 },
    { 3100  , 0x0F },
    { 3900  , 0x07 },
    { 5200  , 0x16 },
    { 6300  , 0x0E },
    { 7800  , 0x06 },
    { 10400 , 0x15 },
    { 12500 , 0x0D },
    { 15600 , 0x05 },
    { 20800 , 0x14 },
    { 25000 , 0x0C },
    { 31300 , 0x04 },
    { 41700 , 0x13 },
    { 50000 , 0x0B },
    { 62500 , 0x03 },
    { 83333 , 0x12 },
    { 100000, 0x0A },
    { 125000, 0x02 },
    { 166700, 0x11 },
    { 200000, 0x09 },
    { 250000, 0x01 },
    { 300000, 0x00 }, // Invalid Bandwidth
};


/*!
 * Reception buffer
 * 收发缓存区
 */
uint8_t RxTxBuffer[RX_BUFFER_SIZE];

/*
 * Public global variables
 */

/*!
 * Radio hardware and global parameters
 */
SX1276_t SX1276;


/*!
 * Tx and Rx timers
 */

#define TX_TIMEOUT_TIMER       1
#define RX_TIMEOUT_TIMER       2
#define RX_TIMEOUT_SYNCWORD    3

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- )
    {
        *dst++ = *src++;
    }
}

/*
 * Radio driver functions implementation
 */
 /************************************************
函数名称 ： SX1276Init
功    能 ： SX1276初始化
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1276Init(void)
{
    uint8_t i;
    // Initialize driver timeout timers
    TimerRegister(TX_TIMEOUT_TIMER, 3000, true, __SX1276OnTimeoutIrq);
    TimerRegister(RX_TIMEOUT_TIMER, 3000, true, __SX1276OnTimeoutIrq);
    //TimerRegister(RX_TIMEOUT_SYNCWORD, 3000, true, __SX1276OnTimeoutIrq);

    SX1276Reset( );

    RxChainCalibration( );

    SX1276SetOpMode( RF_OPMODE_SLEEP );

    for( i = 0; i < sizeof( RadioRegsInit ) / sizeof( RadioRegisters_t ); i++ )
    {
        SX1276SetModem( RadioRegsInit[i].Modem );
        SX1276Write( RadioRegsInit[i].Addr, RadioRegsInit[i].Value );
    }
	SX1276SetModem( MODEM_LORA );

    SX1276.Settings.State = RF_IDLE;
}


 /************************************************
函数名称 ： SX1276GetStatus
功    能 ： 读取当前处于什么状态
参    数 ： 无
返 回 值 ： 返回当前状态
作    者 ： sun
*************************************************/

RadioState_t SX1276GetStatus( void )
{
    return SX1276.Settings.State;
}
 /************************************************
函数名称 ： SX1276SetFreq
功    能 ： 设置SX1276工作频率
参    数 ： uint32_t freq：工作频率
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetFreq( uint32_t freq )
{
    SX1276.Settings.freq = freq;
    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1276Write( REG_FRFMSB, ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
    SX1276Write( REG_FRFMID, ( uint8_t )( ( freq >> 8 ) & 0xFF ) );
    SX1276Write( REG_FRFLSB, ( uint8_t )( freq & 0xFF ) );
}





/*!
 * Performs the Rx chain calibration for LF and HF bands
 * \remark Must be called just after the reset so all registers are at their
 *         default values
 */
 
  /************************************************
函数名称 ：  RxChainCalibration
功    能 ： 为低频和高频带执行Rx链校准
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void RxChainCalibration( void )
{
    uint8_t regPaConfigInitVal;
    uint32_t initialFreq;

    // Save context
    regPaConfigInitVal = SX1276Read( REG_PACONFIG );
    initialFreq = ( double )( ( ( uint32_t )SX1276Read( REG_FRFMSB ) << 16 ) |
                              ( ( uint32_t )SX1276Read( REG_FRFMID ) << 8 ) |
                              ( ( uint32_t )SX1276Read( REG_FRFLSB ) ) ) * ( double )FREQ_STEP;

    // Cut the PA just in case, RFO output, power = -1 dBm
    SX1276Write( REG_PACONFIG, 0x00 );

    // Launch Rx chain calibration for LF band
    SX1276Write( REG_IMAGECAL, ( SX1276Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1276Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Sets a Frequency in HF band
    SX1276SetFreq( 868000000 );

    // Launch Rx chain calibration for HF band
    SX1276Write( REG_IMAGECAL, ( SX1276Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_MASK ) | RF_IMAGECAL_IMAGECAL_START );
    while( ( SX1276Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING ) == RF_IMAGECAL_IMAGECAL_RUNNING )
    {
    }

    // Restore context
    SX1276Write( REG_PACONFIG, regPaConfigInitVal );
    SX1276SetFreq( initialFreq );
}

/************************************************
函数名称 ：  SX1276SetRxConfig
功    能 ： 接收配置
参    数 ： uint32_t bandwidth
返 回 值 ： RadioModems_t modem,  :LoRa或者FSK
					 uint32_t bandwidth,   ：带宽
					 uint32_t datarate,    :LORa扩频因子 
					                         FSK : 600..300000 bits/s
 *                                 LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                         10: 1024, 11: 2048, 12: 4096  chips]
					 uint8_t coderate,      设置编码率 (只针对LoRa)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
					 uint32_t bandwidthAfc, 设置 AFC Bandwidth (指针对FSK )
 *                                FSK : >= 2600 and <= 250000 Hz
 *                                LoRa: N/A ( set to 0 )
					 uint16_t preambleLen,  设置前导码
 *                                FSK : Number of bytes
 *                                LoRa: Length in symbols (硬件增加了4个字符)
					 uint16_t symbTimeout,  设置接收超时
 *                                FSK : timeout number of bytes
 *                                LoRa: timeout in symbols
					 bool fixLen,           是否为固定长度的包 [0: 可变的, 1: 固定的]
					 uint8_t payloadLen,    设置负载长度，当fixlen为1是需要设置
					 bool crcOn,            是否使能 CRC [0: OFF, 1: ON]
					 bool freqHopOn,        调频开关
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: OFF, 1: ON]
					 uint8_t hopPeriod,     每跳之间的符号数
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: Number of symbols
					 bool iqInverted,       中断信号翻转(LoRa only)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: not inverted, 1: inverted]
					 bool rxContinuous )    //在连续模式下接收信号 0:单次接收  1:连续接收
作    者 ： sun
*************************************************/
void SX1276SetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{
    SX1276SetModem( modem );

	/* if( bandwidth > 2 ) */
	if( bandwidth > 3 )
	{
	    // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
	    while( 1 );
	}
	/* bandwidth += 7; */
	if(bandwidth == 3)
	{
	    bandwidth = 6;
	}
	else
	{
	    bandwidth += 7;
	}
	/* DEBUG("bandwidth=%d\r\n",bandwidth); */
	SX1276.Settings.LoRa.Bandwidth = bandwidth;
	SX1276.Settings.LoRa.Datarate = datarate;
	SX1276.Settings.LoRa.Coderate = coderate;
	SX1276.Settings.LoRa.PreambleLen = preambleLen;
	SX1276.Settings.LoRa.FixLen = fixLen;
	SX1276.Settings.LoRa.PayloadLen = payloadLen;
	SX1276.Settings.LoRa.CrcOn = crcOn;
	SX1276.Settings.LoRa.FreqHopOn = freqHopOn;
	SX1276.Settings.LoRa.HopPeriod = hopPeriod;
	SX1276.Settings.LoRa.IqInverted = iqInverted;
	SX1276.Settings.LoRa.RxContinuous = rxContinuous;

	if( datarate > 12 )
	{
	    datarate = 12;
	}
	else if( datarate < 6 )
	{
	    datarate = 6;
	}

	/* if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) || */
	/*     ( ( bandwidth == 8 ) && ( datarate == 12 ) ) ) */
	if( ( ( bandwidth == 7 || bandwidth == 6) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
	    ( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
	{
	    SX1276.Settings.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
	    SX1276.Settings.LoRa.LowDatarateOptimize = 0x00;
	}

	SX1276Write( REG_LR_MODEMCONFIG1,
	             ( SX1276Read( REG_LR_MODEMCONFIG1 ) &
	               RFLR_MODEMCONFIG1_BW_MASK &
	               RFLR_MODEMCONFIG1_CODINGRATE_MASK &
	               RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
	               ( bandwidth << 4 ) | ( coderate << 1 ) |
	               fixLen );

	SX1276Write( REG_LR_MODEMCONFIG2,
	             ( SX1276Read( REG_LR_MODEMCONFIG2 ) &
	               RFLR_MODEMCONFIG2_SF_MASK &
	               RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
	               RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) |
	               ( datarate << 4 ) | ( crcOn << 2 ) |
	               ( ( symbTimeout >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) );

	SX1276Write( REG_LR_MODEMCONFIG3,
	             ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
	               RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
	               ( SX1276.Settings.LoRa.LowDatarateOptimize << 3 ) );

	SX1276Write( REG_LR_SYMBTIMEOUTLSB, ( uint8_t )( symbTimeout & 0xFF ) );

	SX1276Write( REG_LR_PREAMBLEMSB, ( uint8_t )( ( preambleLen >> 8 ) & 0xFF ) );
	SX1276Write( REG_LR_PREAMBLELSB, ( uint8_t )( preambleLen & 0xFF ) );

	if( fixLen == 1 )
	{
	    SX1276Write( REG_LR_PAYLOADLENGTH, payloadLen );
	}

	if( SX1276.Settings.LoRa.FreqHopOn == true )
	{
	    SX1276Write( REG_LR_PLLHOP, ( SX1276Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
	    SX1276Write( REG_LR_HOPPERIOD, SX1276.Settings.LoRa.HopPeriod );
	}

	if( ( bandwidth == 9 ) && ( SX1276.Settings.freq > RF_MID_BAND_THRESH ) )
	{
	    // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
	    SX1276Write( REG_LR_TEST36, 0x02 );
	    SX1276Write( REG_LR_TEST3A, 0x64 );
	}
	else if( bandwidth == 9 )
	{
	    // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
	    SX1276Write( REG_LR_TEST36, 0x02 );
	    SX1276Write( REG_LR_TEST3A, 0x7F );
	}
	else
	{
	    // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
	    SX1276Write( REG_LR_TEST36, 0x03 );
	}

	if( datarate == 6 )
	{
	    SX1276Write( REG_LR_DETECTOPTIMIZE,
	                 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
	                   RFLR_DETECTIONOPTIMIZE_MASK ) |
	                   RFLR_DETECTIONOPTIMIZE_SF6 );
	    SX1276Write( REG_LR_DETECTIONTHRESHOLD,
	                 RFLR_DETECTIONTHRESH_SF6 );
	}
	else
	{
	    SX1276Write( REG_LR_DETECTOPTIMIZE,
	                 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
	                 RFLR_DETECTIONOPTIMIZE_MASK ) |
	                 RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
	    SX1276Write( REG_LR_DETECTIONTHRESHOLD,
	                 RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
	}
}



/************************************************
函数名称 ：  SX1276SetTxConfig
功    能 ： SX1276发送设置
参    数 ： 
          RadioModems_t modem,   //LoRa/FSK m模式
					int8_t power,         设置发送功率  dbm
					uint32_t fdev,        设置频率偏移(FSK only)
                                 FSK : [Hz]
                                 LoRa: 0
					uint32_t bandwidth,    设置带宽
					uint32_t datarate,      LORa扩频因子 
					                        FSK : 600..300000 bits/s
 *                                LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                         10: 1024, 11: 2048, 12: 4096  chips]
					uint8_t coderate,       设置编码率 (只针对LoRa)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
					uint16_t preambleLen,   设置前导码
 *                                FSK : Number of bytes
 *                                LoRa: Length in symbols (硬件增加了4个字符)
					bool fixLen,            是否为固定长度的包 [0: 可变的, 1: 固定的]
					bool crcOn,             是否使能 CRC [0: OFF, 1: ON]
					bool freqHopOn,         调频开关
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: OFF, 1: ON]
					uint8_t hopPeriod,      每跳之间的符号数
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: Number of symbols
					 bool iqInverted,        LoRa: Number of symbols
					 bool iqInverted,       中断信号翻转(LoRa only)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: not inverted, 1: inverted]
					uint32_t timeout        发送超时时间


返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    SX1276SetModem( modem );

    SX1276SetRfTxPower(power);

	/* if( bandwidth > 2 ) */
	if( bandwidth > 3 )
	{
		// Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
		while( 1 );
	}
	/* bandwidth += 7; */
	if(bandwidth == 3)
	{
		bandwidth = 6;
	}
	else
	{
		bandwidth += 7;
	}
	/* DEBUG("bandwidth=%d\r\n",bandwidth); */
	SX1276.Settings.LoRa.Bandwidth = bandwidth;
	SX1276.Settings.LoRa.Datarate = datarate;
	SX1276.Settings.LoRa.Coderate = coderate;
	SX1276.Settings.LoRa.PreambleLen = preambleLen;
	SX1276.Settings.LoRa.FixLen = fixLen;
	SX1276.Settings.LoRa.FreqHopOn = freqHopOn;
	SX1276.Settings.LoRa.HopPeriod = hopPeriod;
	SX1276.Settings.LoRa.CrcOn = crcOn;
	SX1276.Settings.LoRa.IqInverted = iqInverted;
	SX1276.Settings.LoRa.TxTimeout = timeout;

	if( datarate > 12 )
	{
		datarate = 12;
	}
	else if( datarate < 6 )
	{
		datarate = 6;
	}
	/* if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) || */
	/*	   ( ( bandwidth == 8 ) && ( datarate == 12 ) ) ) */
	if( ( ( bandwidth == 7 || bandwidth == 6) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
		( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
	{
		SX1276.Settings.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
		SX1276.Settings.LoRa.LowDatarateOptimize = 0x00;
	}

	if( SX1276.Settings.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_PLLHOP, ( SX1276Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
		SX1276Write( REG_LR_HOPPERIOD, SX1276.Settings.LoRa.HopPeriod );
	}

	SX1276Write( REG_LR_MODEMCONFIG1,
				 ( SX1276Read( REG_LR_MODEMCONFIG1 ) &
				   RFLR_MODEMCONFIG1_BW_MASK &
				   RFLR_MODEMCONFIG1_CODINGRATE_MASK &
				   RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
				   ( bandwidth << 4 ) | ( coderate << 1 ) |
				   fixLen );

	SX1276Write( REG_LR_MODEMCONFIG2,
				 ( SX1276Read( REG_LR_MODEMCONFIG2 ) &
				   RFLR_MODEMCONFIG2_SF_MASK &
				   RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) |
				   ( datarate << 4 ) | ( crcOn << 2 ) );

	SX1276Write( REG_LR_MODEMCONFIG3,
				 ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
				   RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
				   ( SX1276.Settings.LoRa.LowDatarateOptimize << 3 ) );

	SX1276Write( REG_LR_PREAMBLEMSB, ( preambleLen >> 8 ) & 0x00FF );
	SX1276Write( REG_LR_PREAMBLELSB, preambleLen & 0xFF );

	if( datarate == 6 )
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,
					 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
					   RFLR_DETECTIONOPTIMIZE_MASK ) |
					   RFLR_DETECTIONOPTIMIZE_SF6 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD,
					 RFLR_DETECTIONTHRESH_SF6 );
	}
	else
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,
					 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
					 RFLR_DETECTIONOPTIMIZE_MASK ) |
					 RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD,
					 RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
	}
}


/************************************************
函数名称 ：  SX1276GetTimeOnAir
功    能 ： 获取空中时间
参    数 ： RadioModems_t modem, LoRa/FSK 模式
            uint8_t pktLen    负载长度
返 回 值 ： 时间单位ms
作    者 ： sun
*************************************************/
uint32_t SX1276GetTimeOnAir( RadioModems_t modem, uint8_t pktLen )
{
    uint32_t airTime = 0;
	double bw = 0.0;
	// REMARK: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
	switch( SX1276.Settings.LoRa.Bandwidth )
	{
	//case 0: // 7.8 kHz
	//	  bw = 7800;
	//	  break;
	//case 1: // 10.4 kHz
	//	  bw = 10400;
	//	  break;
	//case 2: // 15.6 kHz
	//	  bw = 15600;
	//	  break;
	//case 3: // 20.8 kHz
	//	  bw = 20800;
	//	  break;
	//case 4: // 31.2 kHz
	//	  bw = 31200;
	//	  break;
	//case 5: // 41.4 kHz
	//	  bw = 41400;
	//	  break;
	//case 6: // 62.5 kHz
	//	  bw = 62500;
	//	  break;
	case 7: // 125 kHz
		bw = 125000;
		break;
	case 8: // 250 kHz
		bw = 250000;
		break;
	case 9: // 500 kHz
		bw = 500000;
		break;
	}

	// Symbol rate : time for one symbol (secs)
	double rs = bw / ( 1 << SX1276.Settings.LoRa.Datarate );
	double ts = 1 / rs;
	// time of preamble
	double tPreamble = ( SX1276.Settings.LoRa.PreambleLen + 4.25 ) * ts;
	// Symbol length of payload and time
	double tmp = ceil( ( 8 * pktLen - 4 * SX1276.Settings.LoRa.Datarate +
						 28 + 16 * SX1276.Settings.LoRa.CrcOn -
						 ( SX1276.Settings.LoRa.FixLen ? 20 : 0 ) ) /
						 ( double )( 4 * ( SX1276.Settings.LoRa.Datarate -
						 ( ( SX1276.Settings.LoRa.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) ) *
						 ( SX1276.Settings.LoRa.Coderate + 4 );
	double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
	double tPayload = nPayload * ts;
	// Time on air
	double tOnAir = tPreamble + tPayload;
	// return ms secs
	airTime = floor( tOnAir * 1000 + 0.999 );
	
    return airTime;
}



/************************************************
函数名称 ：  SX1276Send
功    能 ： 数据发送
参    数 ： uint8_t *buffer, 发送缓冲区
            uint8_t size     数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276Send( uint8_t *buffer, uint8_t size )
{
    uint32_t txTimeout = 0;
		u08 i;
		
		TRACE("__LoraSendProData:\r\n");
	
		for(i = 0; i < size; i++)
		{
			TRACE(" %02x", buffer[i]);
		}
	if( SX1276.Settings.LoRa.IqInverted == true )
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON );
	}
	else
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF );
	}

	SX1276.Settings.LoRaPacketHandler.Size = size;

	// Initializes the payload size
	SX1276Write( REG_LR_PAYLOADLENGTH, size );

	// Full buffer used for Tx
	SX1276Write( REG_LR_FIFOTXBASEADDR, 0 );
	SX1276Write( REG_LR_FIFOADDRPTR, 0 );

	// FIFO operations can not take place in Sleep mode
	if( ( SX1276Read( REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
	{
		SX1276SetStby( );
		delay( 1 );
	}
	// Write payload buffer
	SX1276WriteFifo( buffer, size );
	txTimeout = SX1276.Settings.LoRa.TxTimeout;
	
    SX1276SetTx( txTimeout );
}


/************************************************
函数名称 ： SX1276SetSleep
功    能 ： 设置SX1276休眠
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetSleep( void )
{
    TimerStop(RX_TIMEOUT_TIMER);
    TimerStop(TX_TIMEOUT_TIMER);
    SX1276SetOpMode( RF_OPMODE_SLEEP );
    SX1276.Settings.State = RF_IDLE;
}



/************************************************
函数名称 ： SX1276SetStby
功    能 ： 设置SX1276为待机模式
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetStby( void )
{
    TimerStop(RX_TIMEOUT_TIMER);
    TimerStop(TX_TIMEOUT_TIMER);
    SX1276SetOpMode( RF_OPMODE_STANDBY );
    SX1276.Settings.State = RF_IDLE;
}
/************************************************
函数名称 ： SX1276SetRx
功    能 ： 设置SX1276为接收模式
参    数 ： uint32_t timeout 接收超时时间 0:为持续接收  
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetRx( uint32_t timeout )
{
    bool rxContinuous = false;

	if( SX1276.Settings.LoRa.IqInverted == true )
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_ON | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON );
	}
	else
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF );
	}

	// ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
	if( SX1276.Settings.LoRa.Bandwidth < 9 )
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE, SX1276Read( REG_LR_DETECTOPTIMIZE ) & 0x7F );
		SX1276Write( REG_LR_TEST30, 0x00 );
		switch( SX1276.Settings.LoRa.Bandwidth )
		{
		case 0: // 7.8 kHz
			SX1276Write( REG_LR_TEST2F, 0x48 );
			SX1276SetFreq(SX1276.Settings.freq + 7810 );
			break;
		case 1: // 10.4 kHz
			SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetFreq(SX1276.Settings.freq + 10420 );
			break;
		case 2: // 15.6 kHz
			SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetFreq(SX1276.Settings.freq + 15620 );
			break;
		case 3: // 20.8 kHz
			SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetFreq(SX1276.Settings.freq + 20830 );
			break;
		case 4: // 31.2 kHz
			SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetFreq(SX1276.Settings.freq + 31250 );
			break;
		case 5: // 41.4 kHz
			SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetFreq(SX1276.Settings.freq + 41670 );
			break;
		case 6: // 62.5 kHz
			SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		case 7: // 125 kHz
			SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		case 8: // 250 kHz
			SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		}
	}
	else
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE, SX1276Read( REG_LR_DETECTOPTIMIZE ) | 0x80 );
	}

	rxContinuous = SX1276.Settings.LoRa.RxContinuous;

	if( SX1276.Settings.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
										  //RFLR_IRQFLAGS_RXDONE |
										  //RFLR_IRQFLAGS_PAYLOADCRCERROR |
										  RFLR_IRQFLAGS_VALIDHEADER |
										  RFLR_IRQFLAGS_TXDONE |
										  RFLR_IRQFLAGS_CADDONE |
										  //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
										  RFLR_IRQFLAGS_CADDETECTED );

		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK  ) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO2_00 );
	}
	else
	{
		SX1276Write( REG_LR_IRQFLAGSMASK, //RFLR_IRQFLAGS_RXTIMEOUT |
										  //RFLR_IRQFLAGS_RXDONE |
										  //RFLR_IRQFLAGS_PAYLOADCRCERROR |
										  RFLR_IRQFLAGS_VALIDHEADER |
										  RFLR_IRQFLAGS_TXDONE |
										  RFLR_IRQFLAGS_CADDONE |
										  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
										  RFLR_IRQFLAGS_CADDETECTED );

		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_00 );
	}
	SX1276Write( REG_LR_FIFORXBASEADDR, 0 );
	SX1276Write( REG_LR_FIFOADDRPTR, 0 );

    memset( RxTxBuffer, 0, ( size_t )RX_BUFFER_SIZE );

    SX1276.Settings.State = RF_RX_RUNNING;
    if(timeout != 0)
    {
        /* TimerSetValue( &RxTimeoutTimer, timeout ); */
        TimerChangePeriod(RX_TIMEOUT_TIMER, timeout);
        /* TimerStart( &RxTimeoutTimer ); */
        TimerStart(RX_TIMEOUT_TIMER);
    }

    if( rxContinuous == true )
    {
        SX1276SetOpMode(RFLR_OPMODE_RECEIVER);
    }
    else
    {
        SX1276SetOpMode(RFLR_OPMODE_RECEIVER_SINGLE);
    }
}


/************************************************
函数名称 ： SX1276SetTx
功    能 ： 设置SX1276为发送模式
参    数 ： uint32_t timeout 发送超时时间   
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetTx( uint32_t timeout )
{
    /* TimerSetValue( &TxTimeoutTimer, timeout ); */
    TimerChangePeriod(TX_TIMEOUT_TIMER, timeout);

	if( SX1276.Settings.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
										  RFLR_IRQFLAGS_RXDONE |
										  RFLR_IRQFLAGS_PAYLOADCRCERROR |
										  RFLR_IRQFLAGS_VALIDHEADER |
										  //RFLR_IRQFLAGS_TXDONE |
										  RFLR_IRQFLAGS_CADDONE |
										  //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
										  RFLR_IRQFLAGS_CADDETECTED );

		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK ) | RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO2_00 );
	}
	else
	{
		SX1276Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
										  RFLR_IRQFLAGS_RXDONE |
										  RFLR_IRQFLAGS_PAYLOADCRCERROR |
										  RFLR_IRQFLAGS_VALIDHEADER |
										  //RFLR_IRQFLAGS_TXDONE |
										  RFLR_IRQFLAGS_CADDONE |
										  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
										  RFLR_IRQFLAGS_CADDETECTED );

		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_01 );
	}

    SX1276.Settings.State = RF_TX_RUNNING;
	  TimerStop(RX_TIMEOUT_TIMER);  // wzh
    /* TimerStart( &TxTimeoutTimer ); */
    TimerStart(TX_TIMEOUT_TIMER);
    SX1276SetOpMode( RF_OPMODE_TRANSMITTER );
}

/************************************************
函数名称 ： SX1276StartCad
功    能 ： 启动CAD模式
参    数 ： 无  
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1276StartCad( void )
{
	SX1276Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
								RFLR_IRQFLAGS_RXDONE |
								RFLR_IRQFLAGS_PAYLOADCRCERROR |
								RFLR_IRQFLAGS_VALIDHEADER |
								RFLR_IRQFLAGS_TXDONE |
								//RFLR_IRQFLAGS_CADDONE |
								RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL // |
								//RFLR_IRQFLAGS_CADDETECTED
								);

	// DIO0=CADDone
	SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_10 );

	SX1276.Settings.State = RF_CAD;
	SX1276SetOpMode( RFLR_OPMODE_CAD );
}


/************************************************
函数名称 ：  SX1276SetTxContinuousWave
功    能 ： 设置SX1276为连续发送模式
参    数 ：  uint32_t freq,  发送的频率 
						int8_t power,    发送的功率
						uint16_t time    超时时间，单位为s
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    uint32_t timeout = ( uint32_t )( time * 1000 );

    SX1276SetFreq( freq );

    SX1276SetTxConfig( MODEM_FSK, power, 0, 0, 4800, 0, 5, false, false, 0, 0, 0, timeout );

    SX1276Write( REG_PACKETCONFIG2, ( SX1276Read( REG_PACKETCONFIG2 ) & RF_PACKETCONFIG2_DATAMODE_MASK ) );
    // Disable radio interrupts
    SX1276Write( REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_11 | RF_DIOMAPPING1_DIO1_11 );
    SX1276Write( REG_DIOMAPPING2, RF_DIOMAPPING2_DIO4_10 | RF_DIOMAPPING2_DIO5_10 );

    /* TimerSetValue( &TxTimeoutTimer, timeout ); */
    TimerChangePeriod(TX_TIMEOUT_TIMER, timeout);

    SX1276.Settings.State = RF_TX_RUNNING;
    /* TimerStart( &TxTimeoutTimer ); */
    TimerStart(TX_TIMEOUT_TIMER);
    SX1276SetOpMode( RF_OPMODE_TRANSMITTER );
}



/************************************************
函数名称 ：  SX1276ReadRssi
功    能 ： 读取当前模式下的信号强度
参    数 ：  RadioModems_t modem  工作模式
返 回 值 ： 信号强度
作    者 ： sun
*************************************************/
int16_t SX1276ReadRssi( RadioModems_t modem )
{
    int16_t rssi = 0;

    switch( modem )
    {
	    case MODEM_FSK:
	        rssi = -( SX1276Read( REG_RSSIVALUE ) >> 1 );
	        break;
	    case MODEM_LORA:
	        if( SX1276.Settings.freq > RF_MID_BAND_THRESH )
	        {
	            rssi = RSSI_OFFSET_HF + SX1276Read( REG_LR_RSSIVALUE );
	        }
	        else
	        {
	            rssi = RSSI_OFFSET_LF + SX1276Read( REG_LR_RSSIVALUE );
	        }
	        break;
	    default:
	        rssi = -1;
	        break;
    }
    return rssi;
}


/************************************************
函数名称 ：  SX1276Reset
功    能 ： SX1276重启
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276Reset( void )
{
    SX1276SetReset();
}



/************************************************
函数名称 ： SX1276SetOpMode
功    能 ： SX1276接收发送切换
参    数 ： 1：发送 0接收
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetOpMode( uint8_t opMode )
{
    if( opMode == RF_OPMODE_SLEEP )
    {
        SX1276SetAntSwLowPower( true );
    }
    else
    {
        SX1276SetAntSwLowPower( false );
        SX1276SetAntSw( opMode );
    }
    SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RF_OPMODE_MASK ) | opMode );
}


/************************************************
函数名称 ： SX1276SetModem
功    能 ： 设置sx1276工作模式
参    数 ： LoRa  /FSK
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1276SetModem( RadioModems_t modem )
{
    if( ( SX1276Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_ON ) != 0 )
    {
        SX1276.Settings.Modem = MODEM_LORA;
    }
    else
    {
        SX1276.Settings.Modem = MODEM_FSK;
    }

    if( SX1276.Settings.Modem == modem )
    {
        return;
    }

    SX1276.Settings.Modem = modem;
    switch( SX1276.Settings.Modem )
    {
    default:
    case MODEM_FSK:
        SX1276SetSleep( );
        SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF );

        SX1276Write( REG_DIOMAPPING1, 0x00 );
        SX1276Write( REG_DIOMAPPING2, 0x30 ); // DIO5=ModeReady
        break;
    case MODEM_LORA:
        SX1276SetSleep( );
        SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON );

        SX1276Write( REG_DIOMAPPING1, 0x00 );
        SX1276Write( REG_DIOMAPPING2, 0x00 );
        break;
    }
}




/************************************************
函数名称 ： SX1276Write
功    能 ： SPI写
参    数 ： uint8_t addr, 地址
            uint8_t data  数据
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}


/************************************************
函数名称 ： SX1276Read
功    能 ： SPI读
参    数 ： uint8_t addr, 地址
返 回 值 ： 数据
作    者 ： sun
*************************************************/
uint8_t SX1276Read( uint8_t addr )
{
    uint8_t data;
    SX1276ReadBuffer( addr, &data, 1 );
    return data;
}


/************************************************
函数名称 ： SX1276WriteBuffer
功    能 ： SPI连续写多个地址
参    数 ： uint8_t addr, 地址
						uint8_t *buffer, 待写入缓冲区
						uint8_t size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    SpiSetNss(0);
    SpiTransfer( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SpiTransfer( buffer[i] );
    }
    SpiSetNss(1);
}
/************************************************
函数名称 ： SX1276ReadBuffer
功    能 ： SPI连续读多个地址
参    数 ： uint8_t addr, 地址
						uint8_t *buffer, 读出缓冲区
						uint8_t size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    SpiSetNss(0);
    SpiTransfer( addr & 0x7F );
    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiTransfer( 0 );
    }
    SpiSetNss(1);
}
/************************************************
函数名称 ： SX1276WriteFifo
功    能 ： 写入SX1276发送缓冲区
参    数 ： 
						uint8_t *buffer, 写入缓冲区
						uint8_t size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}
/************************************************
函数名称 ： SX1276ReadFifo
功    能 ： 从SX1276接收缓冲区读取数据
参    数 ： 
						uint8_t *buffer, 读出缓冲区
						uint8_t size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}


/************************************************
函数名称 ： SX1276SetMaxPayloadLength
功    能 ： 设置SX1276最大负载长度
参    数 ：  RadioModems_t modem 工作模式
            uint8_t max 最大数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    SX1276SetModem( modem );
	SX1276Write( REG_LR_PAYLOADMAXLENGTH, max );
}
/************************************************
函数名称 ： SX1276SetSyncWord
功    能 ： 设置SX1276  SYNC_WORD 同步字
参    数 ： uint8_t SyncWord 同步字
           
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetSyncWord(uint8_t SyncWord)
{
	SX1276Write( REG_LR_SYNCWORD, SyncWord );

}

/************************************************
函数名称 ： SX1276SetPublicNetwork
功    能 ： 设置SX1276  SYNC_WORD 同步字
参    数 ：  enable 公有还是私有
           
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetPublicNetwork( bool enable )
{
    SX1276SetModem( MODEM_LORA );
    SX1276.Settings.LoRa.PublicNetwork = enable;
    if( enable == true )
    {
        // Change LoRa modem SyncWord
        SX1276Write( REG_LR_SYNCWORD, LORA_MAC_PUBLIC_SYNCWORD );
    }
    else
    {
        // Change LoRa modem SyncWord
        SX1276Write( REG_LR_SYNCWORD, LORA_MAC_PRIVATE_SYNCWORD );
    }
}








///超时或者IO口事件处理
void SX1276OnTimeoutEvent( void )
{
 	switch(SX1276.Settings.State)
	{
		case RF_RX_RUNNING:
		    OnRxTimeout();
		    
		    break;
		case RF_TX_RUNNING:
		    // Tx timeout shouldn't happen.
		    // But it has been observed that when it happens it is a result of a corrupted SPI transfer
		    // it depends on the platform design.
		    //
		    // The workaround is to put the radio in a known state. Thus, we re-initialize it.

		    // BEGIN WORKAROUND

		    // Reset the radio
		    SX1276Reset( );

		    // Calibrate Rx chain
		    RxChainCalibration( );

		    // Initialize radio default values
		    SX1276SetOpMode( RF_OPMODE_SLEEP );

		    for( uint8_t i = 0; i < sizeof( RadioRegsInit ) / sizeof( RadioRegisters_t ); i++ )
		    {
		        SX1276SetModem( RadioRegsInit[i].Modem );
		        SX1276Write( RadioRegsInit[i].Addr, RadioRegsInit[i].Value );
		    }
		    SX1276SetModem(MODEM_LORA);

		    // Restore previous network type setting.
		    SX1276SetPublicNetwork( SX1276.Settings.LoRa.PublicNetwork );
		    // END WORKAROUND

		    SX1276.Settings.State = RF_IDLE;
		    OnTxTimeout();
		    break;
		    
		default:
		    break;
	}
}


static void __SX1276OnTimeoutIrq( void )
{
	printf("SX1276EventIrqFlag[EVENT_TIME_IRQ] = 1\r\n");
	SX1276EventIrqFlag[EVENT_TIME_IRQ] = 1;
}


void SX1276OnDio0Event(void)
{
    volatile uint8_t irqFlags = 0;
	int8_t snr = 0;
	int16_t rssi;
	
    switch(SX1276.Settings.State)
    {
        case RF_RX_RUNNING:
            //TimerStop( &RxTimeoutTimer );
            // RxDone interrupt
			// Clear Irq
			SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE );

			irqFlags = SX1276Read( REG_LR_IRQFLAGS );
			if( ( irqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
			{
			    // Clear Irq
			    SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);

			    if( SX1276.Settings.LoRa.RxContinuous == false )
			    {
			        SX1276.Settings.State = RF_IDLE;
			    }
			    /* TimerStop( &RxTimeoutTimer ); */
			    TimerStop(RX_TIMEOUT_TIMER);
			    
			    printf("void SX1276OnDio0Event(void) if\r\n");
			    OnRxError();
			    printf("void SX1276OnDio0Event(void) end\r\n");
			    
			    break;
			}

			SX1276.Settings.LoRaPacketHandler.SnrValue = SX1276Read( REG_LR_PKTSNRVALUE );
			if( SX1276.Settings.LoRaPacketHandler.SnrValue & 0x80 ) // The SNR sign bit is 1
			{
			    // Invert and divide by 4
			    snr = ( ( ~SX1276.Settings.LoRaPacketHandler.SnrValue + 1 ) & 0xFF ) >> 2;
			    snr = -snr;
			}
			else
			{
			    // Divide by 4
			    snr = ( SX1276.Settings.LoRaPacketHandler.SnrValue & 0xFF ) >> 2;
			}

			rssi = SX1276Read( REG_LR_PKTRSSIVALUE);
			if( snr < 0 )
			{
			    if( SX1276.Settings.freq > RF_MID_BAND_THRESH )
			    {
			        SX1276.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + ( rssi >> 4 ) +
			                                                      snr;
			    }
			    else
			    {
			        SX1276.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 ) +
			                                                      snr;
			    }
			}
			else
			{
			    if( SX1276.Settings.freq > RF_MID_BAND_THRESH )
			    {
			        SX1276.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + ( rssi >> 4 );
			    }
			    else
			    {
			        SX1276.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 );
			    }
			}

			SX1276.Settings.LoRaPacketHandler.Size = SX1276Read( REG_LR_RXNBBYTES );
			SX1276Write( REG_LR_FIFOADDRPTR, SX1276Read( REG_LR_FIFORXCURRENTADDR ) );
			SX1276ReadFifo( RxTxBuffer, SX1276.Settings.LoRaPacketHandler.Size );

			if( SX1276.Settings.LoRa.RxContinuous == false )
			{
			    SX1276.Settings.State = RF_IDLE;
			}
			/* TimerStop( &RxTimeoutTimer ); */
			TimerStop(RX_TIMEOUT_TIMER);

			OnRxDone( RxTxBuffer, SX1276.Settings.LoRaPacketHandler.Size, SX1276.Settings.LoRaPacketHandler.RssiValue, SX1276.Settings.LoRaPacketHandler.SnrValue );
            break;
            
        case RF_TX_RUNNING:
            /* TimerStop( &TxTimeoutTimer ); */
            TimerStop(TX_TIMEOUT_TIMER);
            // TxDone interrupt
			// Clear Irq
			SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );
			// Intentional fall through
			SX1276.Settings.State = RF_IDLE;
			OnTxDone();
            break;
            
		case RF_CAD:
			if( ( SX1276Read( REG_LR_IRQFLAGS ) & RFLR_IRQFLAGS_CADDETECTED ) == RFLR_IRQFLAGS_CADDETECTED )
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED | RFLR_IRQFLAGS_CADDONE );
				OnCadDone(true);
			}
			else
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE );
				OnCadDone(false);
			}
			break;
		
        default:
            break;
    }
}

void SX1276OnDio0Irq( void )
{

#ifdef  EVENT_IN_IRQ
 SX1276OnDio0Event();
#else
	SX1276EventIrqFlag[EVENT_DIO0_IRQ] = 1;;
#endif

}


// 获取版本信息
uint8_t SX1276GetVersion(void)
{
    return SX1276Read( REG_LR_VERSION );
}
//获取当前工作频率
uint32_t SX1276LoRaGetRFFrequency( void )
{
    uint32_t RFFrequency;
    uint8_t freq[3];
    SX1276ReadBuffer( REG_LR_FRFMSB, freq, 3 );
    RFFrequency = ( ( uint32_t )freq[0] << 16 ) | ( ( uint32_t )freq[1] << 8 ) | ( ( uint32_t )freq[2]  );
    RFFrequency = ( uint32_t )( ( double )RFFrequency * ( double )FREQ_STEP );
    return RFFrequency;
}

uint32_t SX1276LoRaGetErrorRFFrequency( void )
{
    uint32_t errorFreq;
    uint8_t freq[3];
    SX1276ReadBuffer( REG_LR_FEIMSB, freq, 3 );
    errorFreq = ( ( uint32_t )freq[0] << 16 ) | ( ( uint32_t )freq[1] << 8 ) | ( ( uint32_t )freq[2]  );
    return errorFreq;
}




//此函数添加到主任务管理中即while循环中
void SX1276Task(void)
{
	#ifdef  EVENT_IN_IRQ
	  return;
	#else
	if(SX1276EventIrqFlag[EVENT_DIO0_IRQ] )
	{
		SX1276EventIrqFlag[EVENT_DIO0_IRQ]  = 0;
		//TRACE("SX1276OnDio0Event!\r\n");
		SX1276OnDio0Event();
	}
	if(SX1276EventIrqFlag[EVENT_DIO1_IRQ] )
	{
		SX1276EventIrqFlag[EVENT_DIO1_IRQ]  = 0;
	}
	if(SX1276EventIrqFlag[EVENT_DIO2_IRQ] )
	{
		SX1276EventIrqFlag[EVENT_DIO2_IRQ]  = 0;
	}
	if(SX1276EventIrqFlag[EVENT_DIO3_IRQ] )
	{
		SX1276EventIrqFlag[EVENT_DIO3_IRQ]  = 0;
	}
	if(SX1276EventIrqFlag[EVENT_TIME_IRQ])
	{
		SX1276EventIrqFlag[EVENT_TIME_IRQ]  = 0;
		SX1276OnTimeoutEvent();
		printf("SX1276EventIrqFlag[EVENT_TIME_IRQ]\r\n");
	}
	
	#endif
}
