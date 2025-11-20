#ifndef __USR_OS_H_
#define __USR_OS_H_

//#include "configs.h"

/*
**
*/
#define SYS_TICK_MIN_MS  1

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))


typedef enum{
	DELAY_REACH = 0,
	DELAY_UNREACH
}delayReachEnum;

typedef enum{
	PARA_NO_RESET = 0,
	PARA_RESET
}paraResetEnum;

/*
**
*/
typedef struct{
	u16 ticks;  //wzh  u08 ticks;
	u16 delays;
}osDelaySt;


/*
**
*/

delayReachEnum SystemDelay(osDelaySt *osDelayS, u16 ms);
void OsMemCpy(u08 *desBuf, u08 *sourceBuf, u16 len);
void OsInit(void);
void OsTimeBaseEvent(void);
void OsDeInit(void);
int OsStrloc(char *str1,char *str2);
char OsStrCmp(u08 *str1, u08 *str2, u16 len);
u08 isSubset(u08 arr1[],  u08 m, u08 arr2[], u08 n);  
void OsVar16IntoBuf(u16 var, u08 *buf);
void OsVar32IntoBuf(u32 var, u08 *buf);
char OsStrStr(u08 *str1, u16 len1, u08 *str2, u16 len2);
void OsBufIntoVar16(u16 *var, u08 *buf);
void OsBufIntoVar32(u32 *var, u08 *buf);
void OsBuf8IntoBuf16(u08 *buf8, u16 *buf16, u08 len);
void OsBuf16IntoBuf8(u16 *buf16, u08 *buf8, u08 len);
u16 OsVar16MsbIntoLsb(u16 var);
u08 OsVar8MsbIntoLsb(u08 var);



#endif

