#include "includes.h"


/*
**
*/
dataSaveSt dataSaveS;
dataSaveSt preDataSaveS;
updateParaSaveSt updateParaSaveS; //wzh

/*
**
*/
void BspDataRecordInit(void)
{
	u08 refreshFlag = 0;
	
	BspGetData(__DATA_SAVE_ADDR_START, (u08 *)&dataSaveS, sizeof(dataSaveS));
	
	
	
	if(0xffff == dataSaveS.netAddr)
	{
		dataSaveS.netAddr = 0;
		refreshFlag       = 1;
	}

	if(dataSaveS.txPwr < 1 || dataSaveS.txPwr > 20)
	{
		dataSaveS.txPwr = 20;//10; //wzh
		refreshFlag     = 1;
	}

	if(refreshFlag)
	{
		BspSaveData(__DATA_SAVE_ADDR_START, (u08 *)&dataSaveS, sizeof(dataSaveS));
		memcpy((u08 *)&preDataSaveS,  (u08 *)&dataSaveS, sizeof(dataSaveS));
	}

	dataSaveS.netCh = GetChByAddr(dataSaveS.netAddr & 0xFF);
	TRACE("addr :ch is %04x:%d\r\n", dataSaveS.netAddr, dataSaveS.netCh);


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

	TRACE("SAVE net address!\r\n");
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

	TRACE("a1 is %08x\r\n", a1);
	TRACE("b1 is %08x\r\n", b1);

	BspGetData(0, (u08 *)&a1, sizeof(a1));
	BspGetData(4, (u08 *)&b1, sizeof(b1));

	
	TRACE("a1 is %08x\r\n", a1);
	TRACE("b1 is %08x\r\n", b1);
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
