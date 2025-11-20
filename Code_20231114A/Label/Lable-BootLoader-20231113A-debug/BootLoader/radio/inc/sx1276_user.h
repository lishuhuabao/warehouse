#ifndef __SX1276_USER_H__
#define __SX1276_USER_H__

#include "myConfig.h"

/*
**
*/
#define SYSTEM_WAIT_SLEEP_TIME 1



/*
**
*/
void SX1276AppInit(void);
void OnTxTimeout(void);
void OnTxDone(void);
void OnRxDone( u08 *payload, u16 size, s16 _rssi, s08 _snr);
void OnRxTimeout(void);
void OnRxError(void);
void OnCadDone(u08 channelActivityDetected);
u32 Sx1276LoraGetAirDr(void);
u32 Sx1276GetRsAsUs(void);
u32 Sx1276PreambleKeepTimeMs(void);
u32 BspLoraGetFreqByCh(u08 ch);

#endif

