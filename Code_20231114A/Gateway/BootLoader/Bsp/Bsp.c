#include "includes.h"

/*
**
*/
void BspInit(void)
{
	BspDebugInit();
	//BspDataRecordInit();
	//BspLoraInit();
    BspWifiInit();
	BspPcInit();
	BspLedInit(); 
}

