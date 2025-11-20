#include "includes.h"

/*
**
*/
#define __ENCRY_I2C_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define __ENCRY_I2C_SCL_PIN                    GPIO_PIN_0
#define __ENCRY_I2C_SCL_PORT                   GPIOH
#define __ENCRY_I2C_SCL_(x)                    HAL_GPIO_WritePin(__ENCRY_I2C_SCL_PORT, __ENCRY_I2C_SCL_PIN, (GPIO_PinState)(x)) 
                  
#define __ENCRY_I2C_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define __ENCRY_I2C_SDA_PIN                    GPIO_PIN_1
#define __ENCRY_I2C_SDA_PORT                   GPIOH
#define __ENCRY_I2C_SDA_(x)                    HAL_GPIO_WritePin(__ENCRY_I2C_SDA_PORT, __ENCRY_I2C_SDA_PIN, (GPIO_PinState)(x)) 
#define __ENCRY_I2C_SDA_READ                   HAL_GPIO_ReadPin(__ENCRY_I2C_SDA_PORT, __ENCRY_I2C_SDA_PIN) 

#define __ENCRY_PWR_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define __ENCRY_PWR_PIN                    GPIO_PIN_13
#define __ENCRY_PWR_PORT                   GPIOC
#define __ENCRY_PWR_(x)                    HAL_GPIO_WritePin(__ENCRY_PWR_PORT, __ENCRY_PWR_PIN, (GPIO_PinState)(x)) 
                
#define IIC_NOACK         1
#define IIC_ACK           0
#define HIGHT             1
#define LOW               0

#define IIC_DELAYTIME     1			 //50
#define ACKCHECKTIME      2000

#define IIC_ADDR	0x00			//对应ENCRY中地址


/*
**
*/

static u08 __GetProductId(void);


/*
**
*/
void BspEncryInit(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};
	
	__ENCRY_I2C_SCL_GPIO_CLK_ENABLE();  
	__ENCRY_I2C_SCL_(0);
	__gpioInitSt.Pin   = __ENCRY_I2C_SCL_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__ENCRY_I2C_SCL_PORT, &__gpioInitSt);

	__ENCRY_I2C_SDA_GPIO_CLK_ENABLE();
	__ENCRY_I2C_SDA_(0);
	__gpioInitSt.Pin   = __ENCRY_I2C_SDA_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__ENCRY_I2C_SDA_PORT, &__gpioInitSt);

	__ENCRY_PWR_GPIO_CLK_ENABLE(); 
	__ENCRY_PWR_(1);
	__gpioInitSt.Pin   = __ENCRY_PWR_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__ENCRY_PWR_PORT, &__gpioInitSt);
}

/*
**
*/
void SdaInputMode(void)
{
	BspPinModeSwitch(__ENCRY_I2C_SDA_PORT, __ENCRY_I2C_SDA_PIN, BSP_PIN_INPUT);
}

/*
**
*/
void SdaOutputMode(void)
{
	BspPinModeSwitch(__ENCRY_I2C_SDA_PORT, __ENCRY_I2C_SDA_PIN, BSP_PIN_OUTPUT);
}

/*
**
*/
void IIC_Delay(u16 times)
{
	u16 j;
	u16 k;
	
	
	for(j = 0; j < times; j++)
	{
		for(k = times; k > 0; k--)
		{
			;
		}
	}
}

/*
**
*/
void IIC_SetSclPin(void)
{
	__ENCRY_I2C_SCL_(1);
}

/*
**
*/
void IIC_ClrSclPin(void)
{
	__ENCRY_I2C_SCL_(0);
}

/*
**
*/
void IIC_SetSdaPin(void)
{
	__ENCRY_I2C_SDA_(1);
}

/*
**
*/
void IIC_ClrSdaPin(void)
{
	__ENCRY_I2C_SDA_(0);
}


//获取数据信号上的输入
bool IIC_GetSdaStus(void)
{
	return (bool)(__ENCRY_I2C_SDA_READ);
}

bool IIC_CheckACK(void)
{
	return IIC_GetSdaStus();
}


//时钟信号输出HLow
void IIC_SetScl(bool HLow)
{
	HLow?IIC_SetSclPin():IIC_ClrSclPin();
}

//数据信号输出HLow
void IIC_SetSda(bool HLow)
{
	HLow?IIC_SetSdaPin():IIC_ClrSdaPin();
}

void IIC_Start(void)
{
	SdaOutputMode();

	IIC_SetSda(HIGHT);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(LOW);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(LOW);
}


//--------------------------------------------------------- 
//函数介绍：i2c停止信号产生
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void IIC_Stop(void)
{
	SdaOutputMode();
	
	IIC_SetSda(LOW);
	IIC_SetScl(HIGHT);

	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	//IIC_SetScl(LOW);
}

//---------------------------------------------------------
//函数名: i2c发送一个字节
//参数说明：
//			ucVal : 字节数据
//返回值说明：
//			0 - 成功
//          1 - 失败
//---------------------------------------------------------
bool IIC_Send(u08 ucVal)
{
	bool Ack;
	u16 AckLoop;
	u08 i;

	SdaOutputMode();

	for(i=0;i<8;i++)
	{
		IIC_SetScl(LOW);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetSda((ucVal & 0x80)?1:0);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(HIGHT);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		ucVal<<= 1;
	}
	IIC_SetScl(LOW);
	SdaInputMode();

	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	Ack = IIC_NOACK;

	for(AckLoop=0;AckLoop<ACKCHECKTIME;AckLoop++) //260us
	{
		if(!IIC_GetSdaStus())
		{
			Ack = IIC_ACK;
			break;
		}
		IIC_Delay(IIC_DELAYTIME);
	}
	IIC_SetScl(LOW);
	return Ack;			//return success=0 / failure=1
}

//---------------------------------------------------------
//函数名: i2c读取一个字节
//参数说明：
//			bACK - 读取一个字节完成后, 设置SDA状态值
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
u08 IIC_Read(bool bACK)
{
	u08 Data;
	u08 i;

	SdaInputMode();

	Data = 0;
	for(i=0;i<8;i++)
	{
		Data <<= 1;
		IIC_SetScl(LOW);
		IIC_Delay(IIC_DELAYTIME);
		//IIC_SetSda(HIGHT);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(HIGHT);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		if(IIC_GetSdaStus())
			Data |= 1;
		else
			Data &= 0xfe;
	}

	IIC_SetScl(LOW);
	SdaOutputMode();

	IIC_Delay(IIC_DELAYTIME);
    IIC_SetSda(bACK);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(LOW);
	return Data;

}

//---------------------------------------------------------
//函数名: 读出1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要读取数据缓冲区指针
//			len	 - 待读取长度
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
bool IIC_ReadWithAddr(u08 addr, u08 *buf, u08 len)
{
  	u08 i;
	i = 10;
    while (i) {
		IIC_Start();
        if (IIC_Send(addr | 0x1) == IIC_ACK) break;
        if (--i == 0) return IIC_NOACK;
    }
	for(i = 0; i < len -1 ; i++)
	{
		buf[i] = IIC_Read(IIC_ACK);
	}

	buf[i] = IIC_Read(IIC_NOACK);

	IIC_Stop();

	return IIC_ACK;
}

//---------------------------------------------------------
//函数名: 写入1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要写入数据缓冲区指针
//			len	 - 待写入长度
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
bool IIC_WriteWithAddr(u08 addr, u08 *buf, u08 len)
{
  	u08 i;

	i = 10;
    while (i) {
		IIC_Start();
        if (IIC_Send(addr  & 0xFE) == IIC_ACK) break;
        if (--i == 0) return IIC_NOACK;
    }

	for(i = 0; i < len; i++)
	{
		if(IIC_Send(buf[i]))
			return IIC_NOACK;
	}

	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	//IIC_Delay(20*IIC_DELAYTIME);
//	DelayNms(40);

	IIC_Stop();

	return IIC_ACK;

}

static u08 __GetProductId(void)
{
	u08 __buf[32] = {0};
	u08 __prodId[16] = {0};
	
	__buf[0] = __GET_PRODUCT_ID;	       
	__buf[1] = __CheckCodeCrc(__buf, 1);
	
	IIC_WriteWithAddr(IIC_ADDR, __buf, 2);
	IIC_ReadWithAddr(IIC_ADDR, __buf, 10);
	

	if(__buf[9] == __CheckCodeCrc(__buf, 9))
	{
		memcpy(__prodId, __buf, 9);
	}

	if(0 == memcmp(__prodId, "YJZNZA005", strlen("YJZNZA005")))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/*
**
*/
void ProductIdCheck(void)
{
	#if __EN_CPY
	    u08 ret;
		
		do{
			ret = __GetProductId();
		}while(ret);

		TRACE("Product ID is Right!\r\n");
	#endif
}


/*
**
*/
static u08 __GetProductVer(void)
{
	u08 __buf[32] = {0};
	u08 __verId[16] = {0};
	
	__buf[0] = __GET_FW_VER;	       
	__buf[1] = __CheckCodeCrc(__buf, 1);
	
	IIC_WriteWithAddr(IIC_ADDR, __buf, 2);
	IIC_ReadWithAddr(IIC_ADDR, __buf, 5);
	

	if(__buf[4] == __CheckCodeCrc(__buf, 4))
	{
		memcpy(__verId, __buf, 4);
	}

	if(0 == memcmp(__verId, "V1.0", strlen("V1.0")))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/*
**
*/
u08 __CheckCodeCrc(u08 *buf, u08 len)
{
	u08 __crc;
	u08 i;

	__crc = buf[0];

	for(i = 1; i < len; i++)
	{
		__crc ^= buf[i];
	}
	
	return (__crc) ;
}


/*
**
*/
void BspEncryWritePacket(u08 *buf, u16 len)
{
	IIC_WriteWithAddr(IIC_ADDR, buf, len);
}

/*
**
*/
void BspEncryReadPacket(u08 *buf, u16 len)
{
	IIC_ReadWithAddr(IIC_ADDR, buf, len);
}

/*
**
*/
void BspEncryVerCheck(void)
{
	u08 ret;
	
	do{
		ret = __GetProductVer();
		//BspIwdgFeed();
	}while(ret);

	//TRACE("Version is Right!\r\n");
}

/*
**
*/
void BspEncrySleep(void)
{
	__ENCRY_PWR_(1);
	__ENCRY_I2C_SCL_(0);
	__ENCRY_I2C_SDA_(0);
}

