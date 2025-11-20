#ifndef		__BSP_DATA_RECORD_H_
#define		__BSP_DATA_RECORD_H_

/*
**
*/
//#define ID_BASE_ADDR 0x0801fff0
#define ID_BASE_ADDR 0x0801Fff0


/*
**
*/
typedef struct{
	u16 addr;
	u08 ch;
	u16 netAddr;
	u08 netCh;
	u16 waitSleepTime;
	s08 txPwr;
}dataSaveSt;

typedef struct{
	u32 updateFlag; 
	u32 updateNum; 
	u32 curAppAddress; 
	u32 nextAppAddress; 
}updateParaSaveSt; //wzh

extern dataSaveSt dataSaveS;	
extern dataSaveSt preDataSaveS;
extern u08 firstPwrFlag;
extern updateParaSaveSt updateParaSaveS; //wzh

/*
**
*/
void BspDataRecordInit(void);
void BspDataRecordEvent(void);
void BspSaveData(u32 addr, u08 *dat, u32 len);
void BspGetData(u32 addr, u08 *dat, u32 len);
void BspDateRecordTest(void);
void BspGetDataV2(u32 baseAddr, u32 offAddr, u08 *dat, u32 len);
void BspSaveDataV2(u32 baseAddr, u32 offAddr, u08 *dat, u32 len);
void BspGetFirstProgFlag(void);
void BspSaveFirstProgFlag(void);

#endif

