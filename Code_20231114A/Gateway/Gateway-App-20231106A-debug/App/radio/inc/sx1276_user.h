#ifndef __SX1276_USER_H_
#define __SX1276_USER_H_

#include "myConfig.h"

/*
**
*/
void OnTxDone(void);
void OnRxDone(u08 *payload, u16 size, s16 _rssi, s08 _snr);
void OnTxTimeout(void);
void OnRxTimeout(void);
void OnRxError(void);
void OnCadDone(u08 channelActivityDetected);

void SX1276AppInit(void);
u32 Sx1276LoraGetAirDr(void);
u32 Sx1276GetRsAsUs(void);
u32 Sx1276PreambleKeepTimeMs(void);
#endif

