
#include	"includes.h"
#include	<string.h>
#include	<stdio.h>

usrSt usr;
u08 loraSendFlag;
u16 prePackNo = 0;

#define __LED_GREEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __LED_GREEN_PIN                    GPIO_PIN_5
#define __LED_GREEN_PORT                   GPIOB
#define __LED_GREEN_(x)                    HAL_GPIO_WritePin(__LED_GREEN_PORT, __LED_GREEN_PIN, (GPIO_PinState)(x)) 
#define __LED_GREEN_TOGGLE()               HAL_GPIO_TogglePin(__LED_GREEN_PORT, __LED_GREEN_PIN) 
void UpdateDataProcess(u08 *dat, u16 len)
{
	u08 __crc;	
	u08 __progDat[PRO_DAT_PACK_LEN_128];
	static u32 __addr;	
	u16 i;
	static u16 packNo;	
	
	if(EOT == dat[0])
	{
     //TRACE("Xmode translate is over!\r\n");	
		__LED_GREEN_(1);
     loraSendFlag = true;		
     BspIapUartSendByte(ACK);		 
     __UpdateResHandler(UPDATE_COMP);
     return;
	}
	else if ((SOH_1K != dat[0] && SOH_128 != dat[0]) || (len != (PRO_DAT_PACK_LEN_128 + 4)))
	{ 
		 //TRACE("updatedata len is error!\r\n");
		 BspIapUartSendByte(NAK);
		__LED_GREEN_(1);
     return;
	}

	__crc = 0;		
	for(i = 3; i < (len-1); i++)
	{
	   __crc += dat[i];
	}
	
	if(__crc != dat[PRO_DAT_PACK_LEN_128 + 3])
	{
			//TRACE("crc is error!\r\n");
			//xmodeStepE = XMODE_NONE;
		__LED_GREEN_(1);
			BspIapUartSendByte(NAK);
	}
	else
	{
		  packNo = ((u16)dat[2] << 8) | ((u16)dat[1]);
		
		  if ( packNo != prePackNo)
			{
			    //TRACE("crc is ok!\r\n");			
			    memcpy(__progDat, &dat[3], sizeof(__progDat));
			    //__addr = updateParaSaveS.nextAppAddress + (dat[1] - 1) * PRO_DAT_PACK_LEN_128;
		      __addr = updateParaSaveS.nextAppAddress + (packNo - 1) * PRO_DAT_PACK_LEN_128;
		      BspIntFlashWrite(__addr , __progDat, (PRO_DAT_PACK_LEN_128));		 
		
		      //BspSaveDataV2(updateParaSaveS.nextAppAddress, __addr, &dat[3], (PRO_DAT_PACK_LEN_128));
			
			    //TRACE("Send Ack!\r\n");
			    BspIapUartSendByte(ACK);
				
				  prePackNo = packNo;
			}				
	}
	
}

/********************************************************************************
**Function:timeout handler;
**Mark	  : none
********************************************************************************/
void GetUpdatPacketDatTimeout(void)
{
	static osDelaySt __osDelayS;

	if(DELAY_UNREACH == SystemDelay(&__osDelayS, 5000))
	{
		return;
	}
	
	if(usr.waitTimes < IAP_GET_DAT_WAIT_TIME)
	{
		//if(XMODE_NONE == xmodeStepE)
		//{
		//	if(0 == usr.waitTimes % 3000)
		//	{
		//		BspIapUartSendByte(NAK);
		//	}
		//}
		usr.waitTimes++;
		return;
	}
	
	//if(sysPwrS.time >= SLEEP_WAIT_TIME - 1)
	//{
		__UpdateResHandler(UPDATE_ERR);
	//}	
	
}

/********************************************************************************
**Function:iapS result handler;
**updateStaEnum sta:only	@UPDATE_COMP  or @UPDATE_ERR,
**Mark	  : none
********************************************************************************/
void __UpdateResHandler(updateStaEnum sta)
{
	//u16 i;
	//u08 __fillBuf[PRO_DAT_PACK_LEN_128];
	
	if(UPDATE_ERR == sta)
	{
		//TRACE("iapS is error!\r\n");
		//BspIntFlashErasePages(FLASH_UPDATE_FLAG_START_ADDR, FLASH_UPDATE_FLAG_END_ADDR);
		//updateParaSaveS.updateFlag = 0;
		//BspIntFlashErasePages(FLASH_APP_START_ADDR, FLASH_APP_END_ADDR);
		//for(i = 0; i < FLASH_BACKUP_SIZE / sizeof(__fillBuf); i++)
		//{
			//TRACE("complete %d\r\n", i * 2);
		//	BspIntFlashRead(FLASH_BACKUP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
		//	BspIntFlashWrite(FLASH_APP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
		//}

		
		//notifyUpdateOk = 1;
		//__NotifyNetExitUpdate();
	}
	else if(UPDATE_COMP == sta)
	{		
		//BspIntFlashErasePages(FLASH_UPDATE_FLAG_START_ADDR, FLASH_UPDATE_FLAG_END_ADDR);
		//updateParaSaveS.updateFlag = 0;
		updateParaSaveS.updateNum += 1; 
    if ( updateParaSaveS.curAppAddress == FLASH_APP_START_ADDR )
		{
		   updateParaSaveS.curAppAddress = FLASH_BACKUP_START_ADDR;
	     updateParaSaveS.nextAppAddress = FLASH_APP_START_ADDR;
		}
		else
		{
			 updateParaSaveS.curAppAddress = FLASH_APP_START_ADDR;
		   updateParaSaveS.nextAppAddress = FLASH_BACKUP_START_ADDR;
		}
		//TRACE("iapS is complete!\r\n");
		
		//BspIntFlashErasePages(FLASH_APP_START_ADDR, FLASH_APP_END_ADDR);
		//__DmaInit();
		//for(i = 0; i < FLASH_BACKUP_SIZE / sizeof(__fillBuf); i++)
		//{
			//TRACE("complete %d\r\n", i * 2);
			//BspIntFlashRead(FLASH_BACKUP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
			//BspIntFlashWrite(FLASH_APP_START_ADDR + i * sizeof(__fillBuf), __fillBuf, sizeof(__fillBuf));
		//}

		//notifyUpdateOk = 1;
		//TRACE("iapS is complete!\r\n");
		//__NotifyNetExitUpdate();

		  //if (HAL_DMA_Start_IT(&DmaHandle, (u32)FLASH_BACKUP_START_ADDR, (u32)FLASH_APP_START_ADDR, 13312) != HAL_OK)
		  //{
		    /* Transfer Error */
		    //Error_Handler();
		  //}
	}
	updateParaSaveS.updateFlag = 0;
	BspSaveDataV2(FLASH_UPDATE_FLAG_START_ADDR, 0, (u08 *)&updateParaSaveS, sizeof(updateParaSaveS));
	//BspIntFlashWrite(FLASH_UPDATE_FLAG_START_ADDR, (u08 *)&updateParaSaveS, sizeof(updateParaSaveS)); //wzh
}
