#ifndef __BSP_ENCRY_H_
#define __BSP_ENCRY_H_

/*
**
*/
#define __SEND_PACK_PARA_LEN  22
	
#define __GET_FW_VER       0xE0
#define __GET_PRODUCT_ID   0xE1
#define __SET_SPD_PARA     0xE2

#define __EN_CPY           1



/*
**
*/
void BspEncryInit(void);
void ProductIdCheck(void);
void BspEncryVerCheck(void);
void BspEncryWritePacket(u08 *buf, u16 len);
void BspEncryReadPacket(u08 *buf, u16 len);
u08 __CheckCodeCrc(u08 *buf, u08 len);
void BspEncrySleep(void);


#endif

