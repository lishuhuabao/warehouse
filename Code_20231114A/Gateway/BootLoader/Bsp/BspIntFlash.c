#include "includes.h"


/*
**
*/
/*************************************************************************
@flash
	-addr start 0x0800 0000
	-addr end		0x0804 0000
		-size	             0x4 0000 //256kB

@bootloader 
	-addr start 0x0800 0000
	-addr end		0x0800 4000
	-size			        0x 4000		//16kB

@application
	-addr start 0x0800 4000
	-addr end		0X0803 8000
	-size			      0x3 4000	   //208KB


@Flash User:
	-addr start 0X0803 8000
	-addr end  0X0804  0000
	-size		    0x 8000	 //32K		 
***************************************************************************/
#define	__INT_FLASH_RECODE_START_ADDR   0x08038000
#define	__INT_FLASH_RECODE_END_ADDR     0x08040000

/********************************************************************************
**Function:write data into inside flash;
**Input Para:int addrress--falsh address of data write;
		      u08*buffer --data  buffer;
		      int len--length of data;
**Mark    : "int len" Must be an integer multiple of 4
********************************************************************************/
u32 BspIntFlashWrite(u32 addrress, u08*buffer, u32 len)
{
	u32 i;
	
	if(len % sizeof(int) != 0)
	{
		return 1;
	}

	HAL_FLASH_Unlock(); 

	for(i = 0; i < len; i += 4)
	{
		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addrress, *(int *)buffer) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return 1;
		}
		addrress += 4;
		buffer += 4;
	}
	HAL_FLASH_Lock();
	return 0;
}

/********************************************************************************
**Function:read data from inside flash;
**Input Para:int addrress--falsh address of data write;
		      u08*buffer --data  buffer;
		      int len--length of data;
**Mark    : "int len" Must be an integer multiple of 4
********************************************************************************/
u32 BspIntFlashRead(u32 address, u08 *buffer, u32 len)
{
	u32 i;
	
	if(len % sizeof(int) != 0)
	{
		return 1;
	}

	for(i = 0; i < len; i += 4)
	{
		*(int *)buffer = *(int *)address;
		buffer += sizeof(int);
		address += sizeof(int);
	}
	return 0;
}

//
HAL_StatusTypeDef BspIntFlashErasePages(u32 startAddr, u32 endAddr)
{
	u32 SectorError = 0;
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef __sta = HAL_ERROR;

	if(startAddr % PAGESIZE)
	{
		TRACE("address is err! \r\n");
		return __sta;
	}
	
	HAL_FLASH_Unlock();
	FlashEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES; //erase type:sector erase;
	FlashEraseInit.PageAddress = startAddr;	//sector erased ;
	FlashEraseInit.NbPages     = (endAddr - startAddr) / PAGESIZE; //only erase one sector 
	__sta = HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError);
	if(HAL_OK != __sta)
	{
		TRACE("erase sector failed!\r\n");
	}
	HAL_FLASH_Lock(); 

	return __sta;
}

/********************************************************************************
**Function:test write data into flash,read from flash and erase flash funciton;
**Input Para:int addrress--falsh address of data write;
		      u08*buffer --data  buffer;
		      int len--length of data;
**Mark    : none
********************************************************************************/
void BspIntFlashTest(void)
{
}

