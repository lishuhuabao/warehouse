#include "includes.h"

/*
**
*/
dataSaveSt dataSaveS;
dataSaveSt preDataSaveS;
u08 firstPwrFlag;
updateParaSaveSt updateParaSaveS; //wzh

/*
**
*/
void BspDataRecordInit(void)
{
	u08 __refreshFlag = 0;

	u32 __id;
	u08 __buf[4];


	BspGetFirstProgFlag();
	
	__id = READ_REG(*((u32 *)ID_BASE_ADDR));
//	
//	//TRACE("id is %08x\r\n", __id);

//	
	if(0 == __id)
	{
		//__id = 0x34000001;
		__id = 0x24060001;
	}
	

	#if TRACE_ADDR_FIX_FOR_TEST_EN
		__id = 0x34000001;
	#endif

	memcpy(__buf, &__id, sizeof(__id));

//	
//	BspGetData(__DATA_SAVE_ADDR_START, (u08 *)&dataSaveS, sizeof(dataSaveS));
//	
	if(0x00 == firstPwrFlag || 0xFF == firstPwrFlag)
	{
		__refreshFlag    = 1;
		dataSaveS.addr = (((u16)__buf[2]) << 8) | (u16)__buf[3];
		dataSaveS.ch   = GetChByAddr((u16)(dataSaveS.addr >> 8));
		//BspSaveDataV2(__FIRST_PROG_FLAG_ADDR, 0, &firstPwrFlag, 1);
	}
	firstPwrFlag = 0x00;
	
	if(dataSaveS.waitSleepTime < 5)
	{
	//	dataSaveS.waitSleepTime = 30;  // wzh
		dataSaveS.waitSleepTime = 5;  // wzh
		__refreshFlag    = 1;
	}

	if(dataSaveS.txPwr < 1 || dataSaveS.txPwr > 20)
	{
		dataSaveS.txPwr = 20;//Lora发射功率，20最大
		__refreshFlag   = 1;
	}
//	
	if(__refreshFlag)
	{
		BspSaveData(__DATA_SAVE_ADDR_START, (u08 *)&dataSaveS, sizeof(dataSaveS));
		memcpy((u08 *)&preDataSaveS,  (u08 *)&dataSaveS, sizeof(dataSaveS));
	}
	//TRACE("dataSaveS.addr is %04x\r\n", dataSaveS.addr);
	//TRACE("dataSaveS.masterCh is %04x\r\n", dataSaveS.masterCh);
}

/*
**
*/
void BspDataRecordEvent(void)
{
	if(0 == OsStrCmp((u08 *)&preDataSaveS, (u08 *)&dataSaveS, sizeof(dataSaveS)))
	{
		return;
	}
	
	BspSaveData(__DATA_SAVE_ADDR_START, (u08 *)&dataSaveS, sizeof(dataSaveS));

	memcpy((void *)&preDataSaveS, &dataSaveS, sizeof(dataSaveS));
	//TRACE("addr is %04x\r\n", dataSaveS.addr);
}

/*
**
*/
void BspSaveData(u32 addr, u08 *dat, u32 len)
{
	u32 __relAddr;
	u32 __absAddr;
	u32 __totalLen;
	u32 __len;
	u08 __buf[PAGESIZE] = {0};
	u08 __con;
	
	__absAddr  = addr + __DATA_RECORD_START_ADDR;
	__relAddr  = addr % PAGESIZE;
	__totalLen = len;

	if(__relAddr + __totalLen <= PAGESIZE)
	{
		__len = __totalLen;
	}
	else
	{
		__len = PAGESIZE - __relAddr;
	}
	
	do{
		BspIntFlashRead(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf)); 
		memcpy(&__buf[__relAddr], dat, __len);
		BspIntFlashErasePages(__absAddr / PAGESIZE * PAGESIZE, (__absAddr / PAGESIZE + 1) * PAGESIZE);
		BspIntFlashWrite(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf));

		__totalLen -= __len;

		if(__totalLen)
		{
			__con = 1;
			
			__relAddr = 0;
			__absAddr += PAGESIZE;
			dat       += __len;
			if(__relAddr + __totalLen <= PAGESIZE)
			{
				__len = __totalLen;
			}
			else
			{
				__len = PAGESIZE - __relAddr;
			}	
		}
		else
		{
			__con = 0;
		}
	}while(__con);
}

/*
**
*/
void BspGetData(u32 addr, u08 *dat, u32 len)
{
	u32 __relAddr;
	u32 __absAddr;
	u32 __totalLen;
	u32 __len;
	u08 __buf[PAGESIZE] = {0};
	u08 __con;
	
	__absAddr  = addr + __DATA_RECORD_START_ADDR;
	__relAddr  = addr % PAGESIZE;
	__totalLen = len;

	if(__relAddr + __totalLen <= PAGESIZE)
	{
		__len = __totalLen;
	}
	else
	{
		__len = PAGESIZE - __relAddr;
	}
	
	do{
		BspIntFlashRead(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf)); 
		memcpy(dat, &__buf[__relAddr], __len);

		__totalLen -= __len;

		if(__totalLen)
		{
			__con = 1;
			
			__relAddr = 0;
			__absAddr += PAGESIZE;
			dat       += __len;
			if(__relAddr + __totalLen <= PAGESIZE)
			{
				__len = __totalLen;
			}
			else
			{
				__len = PAGESIZE - __relAddr;
			}	
		}
		else
		{
			__con = 0;
		}
	}while(__con);
}

/*
**
*/
void BspDateRecordTest(void)
{
	u32 a1 = 0x12345678;
	u32 b1 = 0x87654321;

	BspSaveData(0, (u08 *)&a1, sizeof(a1));
	BspSaveData(4, (u08 *)&b1, sizeof(b1));

	memset((u08 *)&a1, 0, sizeof(a1));
	memset((u08 *)&b1, 0, sizeof(b1));

	//TRACE("a1 is %08x\r\n", a1);
	//TRACE("b1 is %08x\r\n", b1);

	BspGetData(0, (u08 *)&a1, sizeof(a1));
	BspGetData(4, (u08 *)&b1, sizeof(b1));

	
	//TRACE("a1 is %08x\r\n", a1);
	//TRACE("b1 is %08x\r\n", b1);
}


void BspGetFirstProgFlag(void)
{
	BspGetDataV2(__FIRST_PROG_FLAG_ADDR, 0, &firstPwrFlag, 1);
	//TRACE("GET firstPwrFlag is %02x\r\n", firstPwrFlag);
}

/*
**
*/
void BspSaveFirstProgFlag(void)
{
	BspSaveDataV2(__FIRST_PROG_FLAG_ADDR, 0, &firstPwrFlag, 1);
	//TRACE("SAVE firstPwrFlag is %02x\r\n", firstPwrFlag);
}

/*
**
*/
void BspGetDataV2(u32 baseAddr, u32 offAddr, u08 *dat, u32 len)
{
	u32 __relAddr;
	u32 __absAddr;
	u32 __totalLen;
	u32 __len;
	u08 __buf[PAGESIZE] = {0};
	u08 __con;
	
	__absAddr  = offAddr + baseAddr;
	__relAddr  = offAddr % PAGESIZE;
	__totalLen = len;

	if(__relAddr + __totalLen <= PAGESIZE)
	{
		__len = __totalLen;
	}
	else
	{
		__len = PAGESIZE - __relAddr;
	}
	
	do{
		BspIntFlashRead(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf)); 
		memcpy(dat, &__buf[__relAddr], __len);

		__totalLen -= __len;

		if(__totalLen)
		{
			__con = 1;
			
			__relAddr = 0;
			__absAddr += PAGESIZE;
			dat 	  += __len;
			if(__relAddr + __totalLen <= PAGESIZE)
			{
				__len = __totalLen;
			}
			else
			{
				__len = PAGESIZE - __relAddr;
			}	
		}
		else
		{
			__con = 0;
		}
	}while(__con);
}


/*
**
*/
void BspSaveDataV2(u32 baseAddr, u32 offAddr, u08 *dat, u32 len)
{
	u32 __relAddr;
	u32 __absAddr;
	u32 __totalLen;
	u32 __len;
	u08 __buf[PAGESIZE] = {0};
	u08 __con;
	
	__absAddr  = offAddr + baseAddr;
	__relAddr  = offAddr % PAGESIZE;
	__totalLen = len;

	if(__relAddr + __totalLen <= PAGESIZE)
	{
		__len = __totalLen;
	}
	else
	{
		__len = PAGESIZE - __relAddr;
	}
	
	do{
		BspIntFlashRead(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf)); 
		memcpy(&__buf[__relAddr], dat, __len);
		BspIntFlashErasePages(__absAddr / PAGESIZE * PAGESIZE, (__absAddr / PAGESIZE + 1) * PAGESIZE);
		BspIntFlashWrite(__absAddr / PAGESIZE * PAGESIZE, __buf, sizeof(__buf));

		__totalLen -= __len;

		if(__totalLen)
		{
			__con = 1;
			
			__relAddr = 0;
			__absAddr += PAGESIZE;
			dat 	  += __len;
			if(__relAddr + __totalLen <= PAGESIZE)
			{
				__len = __totalLen;
			}
			else
			{
				__len = PAGESIZE - __relAddr;
			}	
		}
		else
		{
			__con = 0;
		}
	}while(__con);
}



