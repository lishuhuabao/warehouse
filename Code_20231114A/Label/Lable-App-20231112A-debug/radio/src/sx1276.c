#include "includes.h"

/*
**
*/
#define RSSI_OFFSET_LF -164
#define RSSI_OFFSET_HF -157

#define TX_TIMEOUT_TIMER 1
#define RX_TIMEOUT_TIMER 2
#define CAD_TIMEOUT_TIMER 3
// #define RX_TIMEOUT_SYNCWORD    3

/*!
 * Radio registers definition
 * 无线寄存器定义
 */
typedef struct
{
	RadioModems_t modem;
	u08 addr;
	u08 val;
} radioRegSt;

__IO u08 SX1276EventIrqFlag[EVENT_NUM] = {0};

const radioRegSt RadioRegsInit[] = RADIO_INIT_REGISTERS_VALUE;
u08 RxTxBuffer[RX_BUFFER_SIZE];
sx1278St sx1278S;

/*!
 * Performs the Rx chain calibration for LF and HF bands (对LF和HF波段进行RX链校准)
 * \remark Must be called just after the reset so all registers are at their
 *         default values （在重置后调用，所以所有寄存器都处于默认值。）
 */
static void RxChainCalibration(void);

/*!
 * \brief Resets the sx1278S
 */
void SX1276Reset(void);

/*!
 * \brief Sets the sx1278S in transmission mode for the given time(设置SX1276发送超时时间)
 * \param [IN] timeout Transmission timeout [ms] [0: continuous, others timeout]
 * \param [IN] 发送超时时间                  [ms] [0: 持续发送, 超时时间]
 */
void SX1276SetTx(u32 timeout);

/*!
 * \brief Writes the buffer contents to the sx1278S FIFO
 *
 * \param [IN] buffer Buffer containing data to be put on the FIFO.
 * \param [IN] size Number of bytes to be written to the FIFO
 */
void SX1276WriteFifo(u08 *buffer, u08 size);

/*!
 * \brief Reads the contents of the sx1278S FIFO
 *
 * \param [OUT] buffer Buffer where to copy the FIFO read data.
 * \param [IN] size Number of bytes to be read from the FIFO
 */
void SX1276ReadFifo(u08 *buffer, u08 size);

/*!
 * \brief Sets the sx1278S operating mode
 *
 * \param [IN] opMode New operating mode
 */
// void SX1276SetOpMode( u08 opMode );

/*!
 * \brief Tx & Rx timeout timer callback
 */
void SX1276OnTimeoutIrq(void);

/*
**
*/
void SX1276Init(void)
{
	u08 i;

	// Initialize driver timeout timers
	TimerRegister(TX_TIMEOUT_TIMER, 5000, true, SX1276OnTimeoutIrq);
	TimerRegister(RX_TIMEOUT_TIMER, 5000, true, SX1276OnTimeoutIrq);
	TimerRegister(CAD_TIMEOUT_TIMER, 1000, true, SX1276OnTimeoutIrq);

	SX1276Reset();

	RxChainCalibration();

	SX1276SetOpMode(RF_OPMODE_SLEEP);

	for (i = 0; i < sizeof(RadioRegsInit) / sizeof(radioRegSt); i++)
	{
		SX1276SetModem(RadioRegsInit[i].modem);
		Sx1278Write(RadioRegsInit[i].addr, RadioRegsInit[i].val);
	}

	SX1276SetModem(MODEM_LORA);

	sx1278S.Settings.State = RF_IDLE;
}

/************************************************
函数名称 ： SX1276SetFreq
功    能 ： 设置SX1276工作频率
参    数 ： u32 freq：工作频率
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetFreq(u32 freq)
{
	sx1278S.Settings.freq = freq;
	freq = (u32)((double)freq / (double)FREQ_STEP);
	Sx1278Write(REG_FRFMSB, (u08)((freq >> 16) & 0xFF));
	Sx1278Write(REG_FRFMID, (u08)((freq >> 8) & 0xFF));
	Sx1278Write(REG_FRFLSB, (u08)(freq & 0xFF));
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
static void RxChainCalibration(void)
{
	u08 regPaConfigInitVal;
	u32 initialFreq;

	// Save context
	regPaConfigInitVal = Sx1278Read(REG_PACONFIG);
	initialFreq = (double)(((u32)Sx1278Read(REG_FRFMSB) << 16) |
						   ((u32)Sx1278Read(REG_FRFMID) << 8) |
						   ((u32)Sx1278Read(REG_FRFLSB))) *
				  (double)FREQ_STEP;

	// Cut the PA just in case, RFO output, power = -1 dBm
	Sx1278Write(REG_PACONFIG, 0x00);

	// Launch Rx chain calibration for LF band
	Sx1278Write(REG_IMAGECAL, (Sx1278Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) | RF_IMAGECAL_IMAGECAL_START);
	while ((Sx1278Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) == RF_IMAGECAL_IMAGECAL_RUNNING)
	{
	}

	// Sets a Frequency in HF band
	SX1276SetFreq(868000000);

	// Launch Rx chain calibration for HF band
	Sx1278Write(REG_IMAGECAL, (Sx1278Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) | RF_IMAGECAL_IMAGECAL_START);
	while ((Sx1278Read(REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) == RF_IMAGECAL_IMAGECAL_RUNNING)
	{
	}

	// Restore context
	Sx1278Write(REG_PACONFIG, regPaConfigInitVal);
	// SX1276SetFreq(initialFreq);
	BspLoraChSet(dataSaveS.ch);
}

/************************************************
函数名称 ：  SX1276SetRxConfig
功    能 ： 接收配置
参    数 ： u32 bandwidth
返 回 值 ： RadioModems_t modem,  :LoRa或者FSK
					 u32 bandwidth,   ：带宽
					 u32 datarate,    :LORa扩频因子
											 FSK : 600..300000 bits/s
 *                                 LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                         10: 1024, 11: 2048, 12: 4096  chips]
					 u08 coderate,      设置编码率 (只针对LoRa)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
					 u32 bandwidthAfc, 设置 AFC Bandwidth (指针对FSK )
 *                                FSK : >= 2600 and <= 250000 Hz
 *                                LoRa: N/A ( set to 0 )
					 uint16_t preambleLen,  设置前导码
 *                                FSK : Number of bytes
 *                                LoRa: Length in symbols (硬件增加了4个字符)
					 uint16_t symbTimeout,  设置接收超时
 *                                FSK : timeout number of bytes
 *                                LoRa: timeout in symbols
					 bool fixLen,           是否为固定长度的包 [0: 可变的, 1: 固定的]
					 u08 payloadLen,    设置负载长度，当fixlen为1是需要设置
					 bool crcOn,            是否使能 CRC [0: OFF, 1: ON]
					 bool freqHopOn,        调频开关
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: OFF, 1: ON]
					 u08 hopPeriod,     每跳之间的符号数
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: Number of symbols
					 bool iqInverted,       中断信号翻转(LoRa only)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: not inverted, 1: inverted]
					 bool rxContinuous )    //在连续模式下接收信号 0:单次接收  1:连续接收
作    者 ： sun
*************************************************/
void SX1276SetRxConfig(RadioModems_t modem, u32 bandwidth,
					   u32 datarate, u08 coderate,
					   u32 bandwidthAfc, uint16_t preambleLen,
					   uint16_t symbTimeout, bool fixLen,
					   u08 payloadLen,
					   bool crcOn, bool freqHopOn, u08 hopPeriod,
					   bool iqInverted, bool rxContinuous)
{
	SX1276SetModem(modem);

	/* if( bandwidth > 2 ) */
	if (bandwidth > 3)
	{
		// Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
		while (1)
			;
	}
	/* bandwidth += 7; */
	if (bandwidth == 3)
	{
		bandwidth = 6;
	}
	else
	{
		bandwidth += 7;
	}
	/* DEBUG("bandwidth=%d\r\n",bandwidth); */
	sx1278S.Settings.LoRa.Bandwidth = bandwidth;
	sx1278S.Settings.LoRa.Datarate = datarate;
	sx1278S.Settings.LoRa.Coderate = coderate;
	sx1278S.Settings.LoRa.PreambleLen = preambleLen;
	sx1278S.Settings.LoRa.FixLen = fixLen;
	sx1278S.Settings.LoRa.PayloadLen = payloadLen;
	sx1278S.Settings.LoRa.CrcOn = crcOn;
	sx1278S.Settings.LoRa.FreqHopOn = freqHopOn;
	sx1278S.Settings.LoRa.HopPeriod = hopPeriod;
	sx1278S.Settings.LoRa.IqInverted = iqInverted;
	sx1278S.Settings.LoRa.RxContinuous = rxContinuous;

	if (datarate > 12)
	{
		datarate = 12;
	}
	else if (datarate < 6)
	{
		datarate = 6;
	}

	/* if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) || */
	/*	   ( ( bandwidth == 8 ) && ( datarate == 12 ) ) ) */
	if (((bandwidth == 7 || bandwidth == 6) && ((datarate == 11) || (datarate == 12))) ||
		((bandwidth == 8) && (datarate == 12)))
	{
		sx1278S.Settings.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
		sx1278S.Settings.LoRa.LowDatarateOptimize = 0x00;
	}

	Sx1278Write(REG_LR_MODEMCONFIG1,
				(Sx1278Read(REG_LR_MODEMCONFIG1) &
				 RFLR_MODEMCONFIG1_BW_MASK &
				 RFLR_MODEMCONFIG1_CODINGRATE_MASK &
				 RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) |
					(bandwidth << 4) | (coderate << 1) |
					fixLen);

	Sx1278Write(REG_LR_MODEMCONFIG2,
				(Sx1278Read(REG_LR_MODEMCONFIG2) &
				 RFLR_MODEMCONFIG2_SF_MASK &
				 RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
				 RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK) |
					(datarate << 4) | (crcOn << 2) |
					((symbTimeout >> 8) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK));

	Sx1278Write(REG_LR_MODEMCONFIG3,
				(Sx1278Read(REG_LR_MODEMCONFIG3) &
				 RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
					(sx1278S.Settings.LoRa.LowDatarateOptimize << 3));

	Sx1278Write(REG_LR_SYMBTIMEOUTLSB, (u08)(symbTimeout & 0xFF));

	Sx1278Write(REG_LR_PREAMBLEMSB, (u08)((preambleLen >> 8) & 0xFF));
	Sx1278Write(REG_LR_PREAMBLELSB, (u08)(preambleLen & 0xFF));

	if (fixLen == 1)
	{
		Sx1278Write(REG_LR_PAYLOADLENGTH, payloadLen);
	}

	if (sx1278S.Settings.LoRa.FreqHopOn == true)
	{
		Sx1278Write(REG_LR_PLLHOP, (Sx1278Read(REG_LR_PLLHOP) & RFLR_PLLHOP_FASTHOP_MASK) | RFLR_PLLHOP_FASTHOP_ON);
		Sx1278Write(REG_LR_HOPPERIOD, sx1278S.Settings.LoRa.HopPeriod);
	}

	if ((bandwidth == 9) && (sx1278S.Settings.freq > RF_MID_BAND_THRESH))
	{
		// ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
		Sx1278Write(REG_LR_TEST36, 0x02);
		Sx1278Write(REG_LR_TEST3A, 0x64);
	}
	else if (bandwidth == 9)
	{
		// ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
		Sx1278Write(REG_LR_TEST36, 0x02);
		Sx1278Write(REG_LR_TEST3A, 0x7F);
	}
	else
	{
		// ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
		Sx1278Write(REG_LR_TEST36, 0x03);
	}

	if (datarate == 6)
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE,
					(Sx1278Read(REG_LR_DETECTOPTIMIZE) &
					 RFLR_DETECTIONOPTIMIZE_MASK) |
						RFLR_DETECTIONOPTIMIZE_SF6);
		Sx1278Write(REG_LR_DETECTIONTHRESHOLD,
					RFLR_DETECTIONTHRESH_SF6);
	}
	else
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE,
					(Sx1278Read(REG_LR_DETECTOPTIMIZE) &
					 RFLR_DETECTIONOPTIMIZE_MASK) |
						RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12);
		Sx1278Write(REG_LR_DETECTIONTHRESHOLD,
					RFLR_DETECTIONTHRESH_SF7_TO_SF12);
	}
}

/************************************************
函数名称 ：  SX1276SetTxConfig
功    能 ： SX1276发送设置
参    数 ：
		  RadioModems_t modem,   //LoRa/FSK m模式
					int8_t power,         设置发送功率  dbm
					u32 fdev,        设置频率偏移(FSK only)
								 FSK : [Hz]
								 LoRa: 0
					u32 bandwidth,    设置带宽
					u32 datarate,      LORa扩频因子
											FSK : 600..300000 bits/s
 *                                LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                         10: 1024, 11: 2048, 12: 4096  chips]
					u08 coderate,       设置编码率 (只针对LoRa)
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
					u08 hopPeriod,      每跳之间的符号数
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: Number of symbols
					 bool iqInverted,        LoRa: Number of symbols
					 bool iqInverted,       中断信号翻转(LoRa only)
 *                                FSK : N/A ( set to 0 )
 *                                LoRa: [0: not inverted, 1: inverted]
					u32 timeout        发送超时时间


返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetTxConfig(RadioModems_t modem, int8_t power, u32 fdev,
					   u32 bandwidth, u32 datarate,
					   u08 coderate, uint16_t preambleLen,
					   bool fixLen, bool crcOn, bool freqHopOn,
					   u08 hopPeriod, bool iqInverted, u32 timeout)
{
	SX1276SetModem(modem);

	Sx1278SetRfTxPower(power);

	/* if( bandwidth > 2 ) */
	if (bandwidth > 3)
	{
		// Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
		while (1)
			;
	}
	/* bandwidth += 7; */
	if (bandwidth == 3)
	{
		bandwidth = 6;
	}
	else
	{
		bandwidth += 7;
	}
	/* DEBUG("bandwidth=%d\r\n",bandwidth); */
	sx1278S.Settings.LoRa.Bandwidth = bandwidth;
	sx1278S.Settings.LoRa.Datarate = datarate;
	sx1278S.Settings.LoRa.Coderate = coderate;
	sx1278S.Settings.LoRa.PreambleLen = preambleLen;
	sx1278S.Settings.LoRa.FixLen = fixLen;
	sx1278S.Settings.LoRa.FreqHopOn = freqHopOn;
	sx1278S.Settings.LoRa.HopPeriod = hopPeriod;
	sx1278S.Settings.LoRa.CrcOn = crcOn;
	sx1278S.Settings.LoRa.IqInverted = iqInverted;
	sx1278S.Settings.LoRa.TxTimeout = timeout;

	if (datarate > 12)
	{
		datarate = 12;
	}
	else if (datarate < 6)
	{
		datarate = 6;
	}
	/* if( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) || */
	/*	   ( ( bandwidth == 8 ) && ( datarate == 12 ) ) ) */
	if (((bandwidth == 7 || bandwidth == 6) && ((datarate == 11) || (datarate == 12))) ||
		((bandwidth == 8) && (datarate == 12)))
	{
		sx1278S.Settings.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
		sx1278S.Settings.LoRa.LowDatarateOptimize = 0x00;
	}

	if (sx1278S.Settings.LoRa.FreqHopOn == true)
	{
		Sx1278Write(REG_LR_PLLHOP, (Sx1278Read(REG_LR_PLLHOP) & RFLR_PLLHOP_FASTHOP_MASK) | RFLR_PLLHOP_FASTHOP_ON);
		Sx1278Write(REG_LR_HOPPERIOD, sx1278S.Settings.LoRa.HopPeriod);
	}

	Sx1278Write(REG_LR_MODEMCONFIG1,
				(Sx1278Read(REG_LR_MODEMCONFIG1) &
				 RFLR_MODEMCONFIG1_BW_MASK &
				 RFLR_MODEMCONFIG1_CODINGRATE_MASK &
				 RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) |
					(bandwidth << 4) | (coderate << 1) |
					fixLen);

	Sx1278Write(REG_LR_MODEMCONFIG2,
				(Sx1278Read(REG_LR_MODEMCONFIG2) &
				 RFLR_MODEMCONFIG2_SF_MASK &
				 RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) |
					(datarate << 4) | (crcOn << 2));

	Sx1278Write(REG_LR_MODEMCONFIG3,
				(Sx1278Read(REG_LR_MODEMCONFIG3) &
				 RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
					(sx1278S.Settings.LoRa.LowDatarateOptimize << 3));

	Sx1278Write(REG_LR_PREAMBLEMSB, (preambleLen >> 8) & 0x00FF);
	Sx1278Write(REG_LR_PREAMBLELSB, preambleLen & 0xFF);

	if (datarate == 6)
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE,
					(Sx1278Read(REG_LR_DETECTOPTIMIZE) &
					 RFLR_DETECTIONOPTIMIZE_MASK) |
						RFLR_DETECTIONOPTIMIZE_SF6);
		Sx1278Write(REG_LR_DETECTIONTHRESHOLD,
					RFLR_DETECTIONTHRESH_SF6);
	}
	else
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE,
					(Sx1278Read(REG_LR_DETECTOPTIMIZE) &
					 RFLR_DETECTIONOPTIMIZE_MASK) |
						RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12);
		Sx1278Write(REG_LR_DETECTIONTHRESHOLD,
					RFLR_DETECTIONTHRESH_SF7_TO_SF12);
	}
}

/************************************************
函数名称 ：  SX1276Send
功    能 ： 数据发送
参    数 ： u08 *buffer, 发送缓冲区
			u08 size     数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
u32 adxse = 0;
u32 ii;
void SX1276Send(u08 *buffer, u08 size)
{
	u32 txTimeout = 0;
	adxse = BspLoraGetFreqByCh(dataSaveS.ch);
	SX1276SetFreq(BspLoraGetFreqByCh(dataSaveS.ch));
	TRACE("ch: %d\t频率 is %d\r\n", dataSaveS.ch, sx1278S.Settings.freq);

	if (sx1278S.Settings.LoRa.IqInverted == true)
	{
		ii = 1;
		Sx1278Write(REG_LR_INVERTIQ, ((Sx1278Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON));
		Sx1278Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON);
	}
	else
	{
		Sx1278Write(REG_LR_INVERTIQ, ((Sx1278Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
		Sx1278Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
	}

	sx1278S.Settings.LoRaPacketHandler.Size = size;

	// Initializes the payload size
	Sx1278Write(REG_LR_PAYLOADLENGTH, size);

	// Full buffer used for Tx
	Sx1278Write(REG_LR_FIFOTXBASEADDR, 0);
	Sx1278Write(REG_LR_FIFOADDRPTR, 0);

	// FIFO operations can not take place in Sleep mode
	if ((Sx1278Read(REG_OPMODE) & ~RF_OPMODE_MASK) == RF_OPMODE_SLEEP)
	{
		SX1276SetStby();
		delay(1);
		TRACE("%d\r\n", __LINE__);
	}
	// Write payload buffer
	SX1276WriteFifo(buffer, size);
	txTimeout = sx1278S.Settings.LoRa.TxTimeout;

	SX1276SetTx(txTimeout);
}

/************************************************
函数名称 ： SX1276SetSleep
功    能 ： 设置SX1276休眠
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetSleep(void)
{
	TimerStop(RX_TIMEOUT_TIMER);
	TimerStop(TX_TIMEOUT_TIMER);
	SX1276SetOpMode(RF_OPMODE_SLEEP); // RF_OPMODE_STANDBY   RF_OPMODE_SLEEP
	sx1278S.Settings.State = RF_IDLE;
}

/************************************************
函数名称 ： SX1276SetStby
功    能 ： 设置SX1276为待机模式
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetStby(void)
{
	TimerStop(RX_TIMEOUT_TIMER);
	TimerStop(TX_TIMEOUT_TIMER);
	SX1276SetOpMode(RF_OPMODE_STANDBY);
	sx1278S.Settings.State = RF_IDLE;
}
/************************************************
函数名称 ： SX1276SetRx
功    能 ： 设置SX1276为接收模式
参    数 ： u32 timeout 接收超时时间 0:为持续接收
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetRx(u32 timeout)
{
	bool rxContinuous = false;

	Sx1278Write(REG_LR_PREAMBLEMSB, (sx1278S.Settings.LoRa.PreambleLen >> 8) & 0x00FF); // wzh
	Sx1278Write(REG_LR_PREAMBLELSB, sx1278S.Settings.LoRa.PreambleLen & 0xFF);			// wzh

	if (sx1278S.Settings.LoRa.IqInverted == true)
	{
		Sx1278Write(REG_LR_INVERTIQ, ((Sx1278Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_ON | RFLR_INVERTIQ_TX_OFF));
		Sx1278Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON);
	}
	else
	{
		Sx1278Write(REG_LR_INVERTIQ, ((Sx1278Read(REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
		Sx1278Write(REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
	}

	// ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
	if (sx1278S.Settings.LoRa.Bandwidth < 9)
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE, Sx1278Read(REG_LR_DETECTOPTIMIZE) & 0x7F);
		Sx1278Write(REG_LR_TEST30, 0x00);
		switch (sx1278S.Settings.LoRa.Bandwidth)
		{
		case 0: // 7.8 kHz
			Sx1278Write(REG_LR_TEST2F, 0x48);
			SX1276SetFreq(sx1278S.Settings.freq + 7810);
			break;
		case 1: // 10.4 kHz
			Sx1278Write(REG_LR_TEST2F, 0x44);
			SX1276SetFreq(sx1278S.Settings.freq + 10420);
			break;
		case 2: // 15.6 kHz
			Sx1278Write(REG_LR_TEST2F, 0x44);
			SX1276SetFreq(sx1278S.Settings.freq + 15620);
			break;
		case 3: // 20.8 kHz
			Sx1278Write(REG_LR_TEST2F, 0x44);
			SX1276SetFreq(sx1278S.Settings.freq + 20830);
			break;
		case 4: // 31.2 kHz
			Sx1278Write(REG_LR_TEST2F, 0x44);
			SX1276SetFreq(sx1278S.Settings.freq + 31250);
			break;
		case 5: // 41.4 kHz
			Sx1278Write(REG_LR_TEST2F, 0x44);
			SX1276SetFreq(sx1278S.Settings.freq + 41670);
			break;
		case 6: // 62.5 kHz
			Sx1278Write(REG_LR_TEST2F, 0x40);
			break;
		case 7: // 125 kHz
			Sx1278Write(REG_LR_TEST2F, 0x40);
			break;
		case 8: // 250 kHz
			Sx1278Write(REG_LR_TEST2F, 0x40);
			break;
		}
	}
	else
	{
		Sx1278Write(REG_LR_DETECTOPTIMIZE, Sx1278Read(REG_LR_DETECTOPTIMIZE) | 0x80);
	}

	rxContinuous = sx1278S.Settings.LoRa.RxContinuous;

	if (sx1278S.Settings.LoRa.FreqHopOn == true)
	{
		Sx1278Write(REG_LR_IRQFLAGSMASK, // RFLR_IRQFLAGS_RXTIMEOUT |
										 // RFLR_IRQFLAGS_RXDONE |
										 // RFLR_IRQFLAGS_PAYLOADCRCERROR |
										 // RFLR_IRQFLAGS_VALIDHEADER | //wzh
					RFLR_IRQFLAGS_TXDONE |
						RFLR_IRQFLAGS_CADDONE |
						// RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
						RFLR_IRQFLAGS_CADDETECTED);

		Sx1278Write(REG_DIOMAPPING1, (Sx1278Read(REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO2_00);
	}
	else
	{
		Sx1278Write(REG_LR_IRQFLAGSMASK, // RFLR_IRQFLAGS_RXTIMEOUT |
										 // RFLR_IRQFLAGS_RXDONE |
										 // RFLR_IRQFLAGS_PAYLOADCRCERROR |
										 // RFLR_IRQFLAGS_VALIDHEADER | //wzh
					RFLR_IRQFLAGS_TXDONE |
						RFLR_IRQFLAGS_CADDONE |
						RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
						RFLR_IRQFLAGS_CADDETECTED);

		// Sx1278Write( REG_DIOMAPPING1, ( Sx1278Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_00 ); // wzh
		Sx1278Write(REG_DIOMAPPING1, (Sx1278Read(REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO3_MASK) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO3_01);
	}
	Sx1278Write(REG_LR_FIFORXBASEADDR, 0);
	Sx1278Write(REG_LR_FIFOADDRPTR, 0);

	memset(RxTxBuffer, 0, (size_t)RX_BUFFER_SIZE);

	sx1278S.Settings.State = RF_RX_RUNNING;
	if (timeout != 0)
	{
		TimerChangePeriod(RX_TIMEOUT_TIMER, timeout);
		Sx1276TimerStart(RX_TIMEOUT_TIMER);
	}

	if (rxContinuous == true)
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
参    数 ： u32 timeout 发送超时时间
返 回 值 ： 无
作    者 ： sun
*************************************************/
u32 iii;
void SX1276SetTx(u32 timeout)
{
	/* TimerSetValue( &TxTimeoutTimer, timeout ); */
	TimerChangePeriod(TX_TIMEOUT_TIMER, timeout);

	Sx1278Write(REG_LR_PREAMBLEMSB, (6U >> 8) & 0x00FF); // wzh
	Sx1278Write(REG_LR_PREAMBLELSB, 6U & 0xFF);			 // wzh

	if (sx1278S.Settings.LoRa.FreqHopOn == true)
	{
		Sx1278Write(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
											 RFLR_IRQFLAGS_RXDONE |
											 RFLR_IRQFLAGS_PAYLOADCRCERROR |
											 RFLR_IRQFLAGS_VALIDHEADER |
											 // RFLR_IRQFLAGS_TXDONE |
											 RFLR_IRQFLAGS_CADDONE |
											 // RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											 RFLR_IRQFLAGS_CADDETECTED);

		Sx1278Write(REG_DIOMAPPING1, (Sx1278Read(REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK) | RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO2_00);
	}
	else
	{
		Sx1278Write(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
											 RFLR_IRQFLAGS_RXDONE |
											 RFLR_IRQFLAGS_PAYLOADCRCERROR |
											 RFLR_IRQFLAGS_VALIDHEADER |
											 // RFLR_IRQFLAGS_TXDONE |
											 RFLR_IRQFLAGS_CADDONE |
											 RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											 RFLR_IRQFLAGS_CADDETECTED);

		Sx1278Write(REG_DIOMAPPING1, (Sx1278Read(REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_01);
	}

	sx1278S.Settings.State = RF_TX_RUNNING;
	TimerStop(RX_TIMEOUT_TIMER);  // wzh
	TimerStop(CAD_TIMEOUT_TIMER); // wzh
	Sx1276TimerStart(TX_TIMEOUT_TIMER);
	SX1276SetOpMode(RF_OPMODE_TRANSMITTER);
	iii = 1;
	//	TimerStop(1);  // wzh
	//	TaskStart(TASK_START_CAD);
}

/************************************************
函数名称 ： SX1276StartCad
功    能 ： 启动CAD模式
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1276StartCad(void)
{
	Sx1278Write(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
										 RFLR_IRQFLAGS_RXDONE |
										 RFLR_IRQFLAGS_PAYLOADCRCERROR |
										 RFLR_IRQFLAGS_VALIDHEADER |
										 RFLR_IRQFLAGS_TXDONE |
										 // RFLR_IRQFLAGS_CADDONE |
										 RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL // |
				// RFLR_IRQFLAGS_CADDETECTED
	);

	// DIO0=CADDone
	// Sx1278Write( REG_DIOMAPPING1, ( Sx1278Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_10 );  //wzh
	Sx1278Write(REG_DIOMAPPING1, (Sx1278Read(REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO3_MASK) | RFLR_DIOMAPPING1_DIO0_10 | RFLR_DIOMAPPING1_DIO3_01);

	sx1278S.Settings.State = RF_CAD;

	Sx1276TimerStart(CAD_TIMEOUT_TIMER);
	SX1276SetOpMode(RFLR_OPMODE_CAD);
}

/************************************************
函数名称 ：  SX1276Reset
功    能 ： SX1276重启
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276Reset(void)
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
void SX1276SetOpMode(u08 opMode)
{
	if (opMode == RF_OPMODE_SLEEP)
	{
		SX1276SetAntSwLowPower(true);
	}
	else
	{
		SX1276SetAntSwLowPower(false);
		SX1276SetAntSw(opMode);
	}
	Sx1278Write(REG_OPMODE, (Sx1278Read(REG_OPMODE) & RF_OPMODE_MASK) | opMode);
}

void SX1276SetSleepWrapper(const char *func, int line)
{
	// TRACE("[%s][%d],from [%s] [%s]", __func__, __LINE__, func, line);
	SX1276SetSleep();
}

/************************************************
函数名称 ： SX1276SetModem
功    能 ： 设置sx1276工作模式
参    数 ： LoRa  /FSK
返 回 值 ： 无
作    者 ： sun
*************************************************/

void SX1276SetModem(RadioModems_t modem)
{
	if ((Sx1278Read(REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_ON) != 0)
	{
		sx1278S.Settings.modem = MODEM_LORA;
	}
	else
	{
		sx1278S.Settings.modem = MODEM_FSK;
	}

	if (sx1278S.Settings.modem == modem)
	{
		return;
	}

	sx1278S.Settings.modem = modem;
	switch (sx1278S.Settings.modem)
	{
	default:
	case MODEM_FSK:
		//   SX1276SetSleep( );//没有检测到有效数据Radio休眠
		SX1276SetSleepWrapper(__func__, __LINE__);
		Sx1278Write(REG_OPMODE, (Sx1278Read(REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_OFF);

		Sx1278Write(REG_DIOMAPPING1, 0x00);
		Sx1278Write(REG_DIOMAPPING2, 0x30); // DIO5=ModeReady
		break;
	case MODEM_LORA:
		//   SX1276SetSleep( );//没有检测到有效数据Radio休眠
		SX1276SetSleepWrapper(__func__, __LINE__);
		Sx1278Write(REG_OPMODE, (Sx1278Read(REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON);

		Sx1278Write(REG_DIOMAPPING1, 0x00);
		Sx1278Write(REG_DIOMAPPING2, 0x00);
		break;
	}
}

/************************************************
函数名称 ： Sx1278Write
功    能 ： SPI写
参    数 ： u08 addr, 地址
			u08 data  数据
返 回 值 ： 无
作    者 ： sun
*************************************************/
void Sx1278Write(u08 addr, u08 data)
{
	SX1276WriteBuffer(addr, &data, 1);
}

/************************************************
函数名称 ： Sx1278Read
功    能 ： SPI读
参    数 ： u08 addr, 地址
返 回 值 ： 数据
作    者 ： sun
*************************************************/
u08 Sx1278Read(u08 addr)
{
	u08 data;
	SX1276ReadBuffer(addr, &data, 1);
	return data;
}

/************************************************
函数名称 ： SX1276WriteBuffer
功    能 ： SPI连续写多个地址
参    数 ： u08 addr, 地址
						u08 *buffer, 待写入缓冲区
						u08 size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276WriteBuffer(u08 addr, u08 *buffer, u08 size)
{
	u08 i;

	BSP_LORA_CS_(0);
	SpiTransfer(addr | 0x80);
	for (i = 0; i < size; i++)
	{
		SpiTransfer(buffer[i]);
	}
	BSP_LORA_CS_(1);
}
/************************************************
函数名称 ： SX1276ReadBuffer
功    能 ： SPI连续读多个地址
参    数 ： u08 addr, 地址
						u08 *buffer, 读出缓冲区
						u08 size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276ReadBuffer(u08 addr, u08 *buffer, u08 size)
{
	u08 i;

	BSP_LORA_CS_(0);
	SpiTransfer(addr & 0x7F);
	for (i = 0; i < size; i++)
	{
		buffer[i] = SpiTransfer(0);
	}
	BSP_LORA_CS_(1);
}
/************************************************
函数名称 ： SX1276WriteFifo
功    能 ： 写入SX1276发送缓冲区
参    数 ：
						u08 *buffer, 写入缓冲区
						u08 size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276WriteFifo(u08 *buffer, u08 size)
{
	SX1276WriteBuffer(0, buffer, size);
}
/************************************************
函数名称 ： SX1276ReadFifo
功    能 ： 从SX1276接收缓冲区读取数据
参    数 ：
						u08 *buffer, 读出缓冲区
						u08 size     数据长度

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276ReadFifo(u08 *buffer, u08 size)
{
	SX1276ReadBuffer(0, buffer, size);
}

/************************************************
函数名称 ： SX1276SetMaxPayloadLength
功    能 ： 设置SX1276最大负载长度
参    数 ：  RadioModems_t modem 工作模式
			u08 max 最大数据长度
返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetMaxPayloadLength(RadioModems_t modem, u08 max)
{
	SX1276SetModem(modem);
	Sx1278Write(REG_LR_PAYLOADMAXLENGTH, max);
}
/************************************************
函数名称 ： SX1276SetSyncWord
功    能 ： 设置SX1276  SYNC_WORD 同步字
参    数 ： u08 SyncWord 同步字

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetSyncWord(u08 SyncWord)
{
	Sx1278Write(REG_LR_SYNCWORD, SyncWord);
}

/************************************************
函数名称 ： SX1276SetPublicNetwork
功    能 ： 设置SX1276  SYNC_WORD 同步字
参    数 ：  enable 公有还是私有

返 回 值 ： 无
作    者 ： sun
*************************************************/
void SX1276SetPublicNetwork(bool enable)
{
	SX1276SetModem(MODEM_LORA);
	sx1278S.Settings.LoRa.PublicNetwork = enable;
	if (enable == true)
	{
		// Change LoRa modem SyncWord
		Sx1278Write(REG_LR_SYNCWORD, LORA_MAC_PUBLIC_SYNCWORD);
	}
	else
	{
		// Change LoRa modem SyncWord
		Sx1278Write(REG_LR_SYNCWORD, LORA_MAC_PRIVATE_SYNCWORD);
	}
}

/// 超时或者IO口事件处理
void SX1276OnTimeoutEvent(void)
{
	switch (sx1278S.Settings.State)
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
		SX1276Reset();

		// Calibrate Rx chain
		RxChainCalibration();

		// Initialize radio default values
		SX1276SetOpMode(RF_OPMODE_SLEEP);

		for (u08 i = 0; i < sizeof(RadioRegsInit) / sizeof(radioRegSt); i++)
		{
			SX1276SetModem(RadioRegsInit[i].modem);
			Sx1278Write(RadioRegsInit[i].addr, RadioRegsInit[i].val);
		}
		SX1276SetModem(MODEM_LORA);

		// Restore previous network type setting.
		SX1276SetPublicNetwork(sx1278S.Settings.LoRa.PublicNetwork);
		// END WORKAROUND

		sx1278S.Settings.State = RF_IDLE;
		OnTxTimeout();

		break;

	case RF_CAD:
		TRACE("3\r\n");
		// Reset the radio
		SX1276Reset();

		// Calibrate Rx chain
		RxChainCalibration();

		// Initialize radio default values
		SX1276SetOpMode(RF_OPMODE_SLEEP);

		for (u08 i = 0; i < sizeof(RadioRegsInit) / sizeof(radioRegSt); i++)
		{
			SX1276SetModem(RadioRegsInit[i].modem);
			Sx1278Write(RadioRegsInit[i].addr, RadioRegsInit[i].val);
		}
		SX1276SetModem(MODEM_LORA);

		// Restore previous network type setting.
		SX1276SetPublicNetwork(sx1278S.Settings.LoRa.PublicNetwork);
		// END WORKAROUND

		sx1278S.Settings.State = RF_IDLE;

		OnCadDone(false);

		break;

	default:
		break;
	}
}

void SX1276OnTimeoutIrq(void)
{
	// printf("SX1276EventIrqFlag[EVENT_TIME_IRQ] = 1\r\n");
	SX1276EventIrqFlag[EVENT_TIME_IRQ] = 1;
}

// wzh
void SX1276OnDio3Event(void)
{
	volatile u08 irqFlags = 0;
	//	  u08 tt = 0;
	switch (sx1278S.Settings.State)
	{
	case RF_RX_RUNNING:
		// Clear Irq
		Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_VALIDHEADER);
		irqFlags = Sx1278Read(REG_LR_IRQFLAGS);
		if ((irqFlags & RFLR_IRQFLAGS_RXDONE_MASK) == RFLR_IRQFLAGS_RXDONE)
		{
			SX1276EventIrqFlag[EVENT_DIO3_IRQ] = 0;
			break;
		}
		if (Sx1278Read(REG_LR_FIFORXBYTEADDR) >= 4)
		{
			// irqFlags = Sx1278Read( REG_LR_FIFORXBYTEADDR );
			SX1276EventIrqFlag[EVENT_DIO3_IRQ] = 0;
			SX1276ReadFifo(RxTxBuffer, 4);
			if (((dataSaveS.addr >> 8) != RxTxBuffer[0] || (dataSaveS.addr & 0xff) != RxTxBuffer[1]) && (0xFF != RxTxBuffer[0] || 0xFF != RxTxBuffer[1]))
			{
				TimerStop(RX_TIMEOUT_TIMER);
				//   SX1276SetSleep( );//没有检测到有效数据Radio休眠
				SX1276SetSleepWrapper(__func__, __LINE__);
				EnterLowerPwrWithWup(500);
			}
			else if (((COMM_LORA_STEP_UPDATA == commLoraCtrlS.step) && (0xFF != RxTxBuffer[2])) || ((COMM_LORA_STEP_PC_COMM == commLoraCtrlS.step) && (0xFF == RxTxBuffer[2])))
			{
				TimerStop(RX_TIMEOUT_TIMER);
				//   SX1276SetSleep( );//没有检测到有效数据Radio休眠
				SX1276SetSleepWrapper(__func__, __LINE__);
				EnterLowerPwrWithWup(500);
			}
		}
		break;

	default:
		break;
	}
}
int fxs;
void SX1276OnDio0Event(void)
{
	volatile u08 irqFlags = 0;
	int8_t snr = 0;
	int16_t rssi;
	// printf("state:%d\r\n", sx1278S.Settings.State);
	switch (sx1278S.Settings.State)
	{
	case RF_RX_RUNNING:
		// TimerStop( &RxTimeoutTimer );
		//  Clear Irq
		// irqFlags = Sx1278Read( REG_LR_IRQFLAGS ); // wzh
		Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);
		irqFlags = Sx1278Read(REG_LR_IRQFLAGS);
		if ((irqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK) == RFLR_IRQFLAGS_PAYLOADCRCERROR)
		{
			// Clear Irq
			Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);

			if (sx1278S.Settings.LoRa.RxContinuous == false)
			{
				sx1278S.Settings.State = RF_IDLE;
			}
			/* TimerStop( &RxTimeoutTimer ); */
			TimerStop(RX_TIMEOUT_TIMER);

			// printf("void SX1276OnDio0Event(void) if\r\n");
			OnRxError();
			// printf("void SX1276OnDio0Event(void) end\r\n");

			break;
		}
		fxs = Sx1278Read(REG_LR_PKTSNRVALUE);
		sx1278S.Settings.LoRaPacketHandler.SnrValue = Sx1278Read(REG_LR_PKTSNRVALUE);
		if (sx1278S.Settings.LoRaPacketHandler.SnrValue & 0x80) // The SNR sign bit is 1
		{
			// Invert and divide by 4
			snr = ((~sx1278S.Settings.LoRaPacketHandler.SnrValue + 1) & 0xFF) >> 2;
			fxs = snr;
			snr = -snr;
			fxs = snr;
		}
		else
		{
			// Divide by 4
			snr = (sx1278S.Settings.LoRaPacketHandler.SnrValue & 0xFF) >> 2;
			fxs = snr;
		}

		rssi = Sx1278Read(REG_LR_PKTRSSIVALUE);

		if (snr < 0)
		{
			if (sx1278S.Settings.freq > RF_MID_BAND_THRESH)
			{
				sx1278S.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + (rssi >> 4) +
															   snr;
				fxs = sx1278S.Settings.LoRaPacketHandler.RssiValue;
			}
			else
			{
				sx1278S.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + (rssi >> 4) +
															   snr;
				fxs = sx1278S.Settings.LoRaPacketHandler.RssiValue;
			}
		}
		else
		{
			if (sx1278S.Settings.freq > RF_MID_BAND_THRESH)
			{
				sx1278S.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + (rssi >> 4);
				fxs = sx1278S.Settings.LoRaPacketHandler.RssiValue;
			}
			else
			{
				sx1278S.Settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + (rssi >> 4);
				fxs = sx1278S.Settings.LoRaPacketHandler.RssiValue;
			}
		}

		sx1278S.Settings.LoRaPacketHandler.Size = Sx1278Read(REG_LR_RXNBBYTES);
		Sx1278Write(REG_LR_FIFOADDRPTR, Sx1278Read(REG_LR_FIFORXCURRENTADDR));
		SX1276ReadFifo(RxTxBuffer, sx1278S.Settings.LoRaPacketHandler.Size);

		if (sx1278S.Settings.LoRa.RxContinuous == false)
		{
			sx1278S.Settings.State = RF_IDLE;
		}
		/* TimerStop( &RxTimeoutTimer ); */
		TimerStop(RX_TIMEOUT_TIMER);

		OnRxDone(RxTxBuffer, sx1278S.Settings.LoRaPacketHandler.Size, sx1278S.Settings.LoRaPacketHandler.RssiValue, sx1278S.Settings.LoRaPacketHandler.SnrValue);
		break;

	case RF_TX_RUNNING:
		/* TimerStop( &TxTimeoutTimer ); */
		TimerStop(TX_TIMEOUT_TIMER);
		Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
		sx1278S.Settings.State = RF_IDLE;
		OnTxDone();
		break;

	case RF_CAD:
		sx1278S.Settings.State = RF_IDLE;
		TimerStop(CAD_TIMEOUT_TIMER); // wzh
		if ((Sx1278Read(REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_CADDETECTED) == RFLR_IRQFLAGS_CADDETECTED)
		{
			// Clear Irq
			Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED | RFLR_IRQFLAGS_CADDONE);
			OnCadDone(true);
		}
		else
		{
			// Clear Irq
			Sx1278Write(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE);
			OnCadDone(false);
		}
		break;

	default:
		break;
	}
}

// 此函数添加到主任务管理中即while循环中
void SX1276Task(void)
{
	// wzh
	if (SX1276EventIrqFlag[EVENT_DIO3_IRQ])
	{
		// printf("SX1276EventIrqFlag[EVENT_DIO3_IRQ]\r\n");
		SX1276EventIrqFlag[EVENT_DIO3_IRQ]  = 0;
		// SX1276OnDio3Event();
	}

	if (SX1276EventIrqFlag[EVENT_DIO0_IRQ])
	{
		SX1276EventIrqFlag[EVENT_DIO0_IRQ] = 0;
		SX1276OnDio0Event();
	}

	if (SX1276EventIrqFlag[EVENT_TIME_IRQ])
	{
		SX1276EventIrqFlag[EVENT_TIME_IRQ] = 0;
		SX1276OnTimeoutEvent();
		// printf("SX1276EventIrqFlag[EVENT_TIME_IRQ]\r\n");
	}
}
