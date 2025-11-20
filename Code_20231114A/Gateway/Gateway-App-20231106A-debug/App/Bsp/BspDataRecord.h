#ifndef		__BSP_DATA_RECORD_H_
#define		__BSP_DATA_RECORD_H_

/*
**
*/
typedef struct{
	u16 netAddr;
	u08 netCh;
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


#endif

