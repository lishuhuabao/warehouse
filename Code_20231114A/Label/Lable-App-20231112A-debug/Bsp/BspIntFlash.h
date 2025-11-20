#ifndef		__BSP_INT_FLASH_H_
#define		__BSP_INT_FLASH_H_

#include "myConfig.h" //wzh
/*
**
*/
/*************************************************************************
@flash
	-addr start 0x0800 0000
	-addr end		0x0802 0000
		-size	             0x2 0000 //128kB

@bootloader
-addr start 0x0800 0000
-addr end  0X0800 3c00
-size	        0x0 3c00	   //15KB

@updateFlag;
-addr start 0x0800 3c00
-addr end  0X0800 4000
-size	        0x0 0400	   //1KB


@application
	-addr start 0x0800 4000
	-addr end  0X0801 1000
	-size               0x0 D000	   //52KB

@application backup
-addr start 0x0801 1000
-addr end  0X0801 E000
-size	        0x0 D000	   //52KB


// dataSaves   firstPwrFlag  __id
@Flash User:
	-addr start 0X0801 E000
	-addr end  0X0802  0000
	-size		    0x 2000	 //8K		 
***************************************************************************/
#define	__DATA_RECORD_START_ADDR   0x0801E000
#define	__DATA_RECORD_END_ADDR     0x08020000

#define __DATA_SAVE_ADDR_START       0
#define __DATA_SAVE_ADDR_SIZE        sizeof(dataSaveSt)
#define __DATA_SAVE_ADDR_END         (__DATA_SAVE_ADDR_START + __DATA_SAVE_ADDR_SIZE)

#define __FIRST_PROG_FLAG_ADDR    0x0801FFF4

#define ADDR_FLASH_PAGE_(x)     ((u32)0x08000000 + (u32)FLASH_PAGE_SIZE * (x)) 

#define FLASH_UPDATE_FLAG_START_ADDR  ADDR_FLASH_PAGE_(60) //15k
#define FLASH_UPDATE_FLAG_END_ADDR    ADDR_FLASH_PAGE_(64) //16k
#define FLASH_UPDATE_FLAG_SIZE        (FLASH_UPDATE_FLAG_END_ADDR - FLASH_UPDATE_FLAG_START_ADDR) // 1K

#define FLASH_APP_START_ADDR          ADDR_FLASH_PAGE_(64) //16k
#define FLASH_APP_END_ADDR            ADDR_FLASH_PAGE_(272) //68k
#define FLASH_APP_SIZE                (FLASH_APP_END_ADDR - FLASH_APP_START_ADDR) //52K

#define	FLASH_BACKUP_START_ADDR       ADDR_FLASH_PAGE_(272) //68K
#define	FLASH_BACKUP_END_ADDR         ADDR_FLASH_PAGE_(480) //120K
#define	FLASH_BACKUP_SIZE             (FLASH_BACKUP_END_ADDR - FLASH_BACKUP_START_ADDR) //52k

/*
**
*/

u32 BspIntFlashWrite(u32 addrress, u08 *buffer, u32 len);

u32 BspIntFlashRead(u32 address, u08 *buffer, u32 len);

void BspIntFlashTest(void);
HAL_StatusTypeDef BspIntFlashErasePages(u32 startAddr, u32 endAddr);


#endif

