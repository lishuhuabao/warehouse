#include "includes.h" 


/*
**
*/
//  在GT16S2W芯片中数据的地址
#define ASC0808D2HZ_ADDR    0x66c0 		//7*8 ascii code
#define ASC0812M2ZF_ADDR    0x66d40   		//6*12 ascii code
#define GBEX0816ZF_ADDR     0x3B7C0   	   		//8*16 ascii code

#define ZF1112B2ZF_ADDR    0x3cf80	   		//12*12 12点字符
#define HZ1112B2HZ_ADDR    (0x3cf80+376*24)	//12*12 12点汉字

#define CUTS1516ZF_ADDR  0x00  				//16*16 16点字符
#define JFLS1516HZ_ADDR  27072  			//16*16 16点汉字

//保留

#define ASCII0507ZF_ADDR        245696
#define ARIAL_16B0_ADDR         246464
#define ARIAL_12B0_ADDR         422720
#define SPAC1616_ADDR           425264
#define GB2311ToUnicode_addr    12032
#define UnicodeToGB2311_addr    425328

#define   TYPE_8  	0 //8  点字符
#define   TYPE_12  	1 //12 点汉字字符
#define   TYPE_16  	2 //16 点汉字字符

//------------------------------------------------------------

#define __ZK_GT_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define __ZK_GT_CS_PIN                    GPIO_PIN_0
#define __ZK_GT_CS_PORT                   GPIOB
#define __ZK_GT_CS_(x)                    HAL_GPIO_WritePin(__ZK_GT_CS_PORT, __ZK_GT_CS_PIN, (GPIO_PinState)(x))



/*
**
*/
SPI_HandleTypeDef zkGtSpi;
bspGtCharSt bspGtCharS;

const bspGtCharSt bspGtCharStab[] = {
	{BSP_GT_CHAR_TYPE_HZ_11X12,     BspGtGetHz11x12Image,    2,  11,  16},
	{BSP_GT_CHAR_TYPE_HZ_15X16,     BspGtGetHz15x16Image,    2,  15,  16},
	{BSP_GT_CHAR_TYPE_HZ_6X12,      BspGtGetHz6x12Image,     2,  6,   16},
	{BSP_GT_CHAR_TYPE_HZ_8X16,      BspGtGetHz8x16Image,     2,  8,   16},
	{BSP_GT_CHAR_TYPE_ASC_5X7,      BspGtGetAsc5x7Image,     1,  5,   8},
	{BSP_GT_CHAR_TYPE_ASC_7X8,      BspGtGetAsc7x8Image,     1,  7,   8},
	{BSP_GT_CHAR_TYPE_ASC_6X12,     BspGtGetAsc6x12Image,    1,  6,   16},
	{BSP_GT_CHAR_TYPE_ASC_8X16,     BspGtGetAsc8x16Image,    1,  8,   16},
	{BSP_GT_CHAR_TYPE_ASC_16X32,    BspGtGetAsc16x32Image,   1,  16,  32},
};


/*
**
*/
static void __ZkGtReadBytes(u32 addr, u08 *buf, u16 len);
static u08 __ZkGtReadWriteByte(u08 TxData);
static u32 __Gb2312Hz11x12GetAddr(u08 *code);
static u32 __Gb2312Hz15x16GetAddr(u08 *code);
static u32 __Gb2312Hz6x12GetAddr(u08 *code);
static u32 __Gb2312Hz8x16GetAddr(u08 *code);
static u32 __Gb2312Asc5x7GetAddr(u08 *code);
static u32 __Gb2312Asc7x8GetAddr(u08 *code);
static u32 __Gb2312Asc6x12GetAddr(u08 *code);
static u32 __Gb2312Asc8x16GetAddr(u08 *code);


/*
**
*/
static void SPI_Address(u08 AddH,u08 AddM,u08 AddL)
{
	__ZK_GT_CS_(0);
	__ZkGtReadWriteByte(0x0b);
	__ZkGtReadWriteByte(AddH);
	__ZkGtReadWriteByte(AddM);
	__ZkGtReadWriteByte(AddL);
	__ZkGtReadWriteByte(0xff);
}


/*
**
*/
static void __ZkGtReadBytes(u32 addr, u08 *buf, u16 len)
{
	u08 i;
	
	SPI_Address((u08)(addr >> 16), (u08)(addr >> 8), (u08)(addr));
	for(i = 0; i < len; i++)
	{
		buf[i] = __ZkGtReadWriteByte(0xff);
	}
	__ZK_GT_CS_(1);
}

static u32 __Gb2312Hz11x12GetAddr(u08 *code)
{
	u32 __baseAdd = 0x3cf80; 
	u32 __addr;

	if(code[0] >= 0xA1 && code[0] <= 0Xa3 && code[1] >= 0xA1)
	{
		__addr =( (code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 24+ __baseAdd;
	}
	else if(code[0] == 0Xa9 && code[1] >= 0xA1)
	{
		__addr = (282 + (code[1] - 0xA1)) * 24 + __baseAdd;
	}
	else if(code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		__addr = ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 376) * 24 + __baseAdd;	
	}

	return __addr;
}


/*
**
*/
static u32 __Gb2312Hz15x16GetAddr(u08 *code)
{
	u32 __baseAdd = 0;
	u32 __addr;
	
	if(code[0] >= 0xA4 && code[0] <= 0Xa8 && code[1] >= 0xA1)
	{
		__addr = __baseAdd;
	}
	else if(code[0] >= 0xA1 && code[0] <= 0Xa9 && code[1] >= 0xA1)
	{
		__addr =((code[0] - 0xA1) * 94 + (code[1] - 0xA1)) * 32 + __baseAdd;
	}
	else if(code[0] >= 0xB0 && code[0] <= 0xF7 && code[1] >= 0xA1)
	{
		__addr = ((code[0] - 0xB0) * 94 + (code[1] - 0xA1) + 846) * 32 + __baseAdd; 
	}

	return __addr;
}

/*
**
*/
static u32 __Gb2312Hz6x12GetAddr(u08 *code)
{
	u32 __baseAdd = 0x66d40;
	u16 __code;
	u32 __addr;

	OsBufIntoVar16(&__code, code);
	
	if ((__code >= 0xAAA1) && (__code <= 0xAAFE )) 
	{
		__addr = (__code - 0xAAA1) * 12 + __baseAdd;
	}
	else if((__code >= 0xABA1) && (__code <= 0xABC0 ))
	{
		__addr = (__code - 0xABA1 + 94) * 12 + __baseAdd;
	}

	return __addr; 
}

/*
**
*/
static u32 __Gb2312Hz8x16GetAddr(u08 *code)
{
	u32 __baseAdd = 0x3b7c0;
	u16 __code;
	u32 __addr;
	
	OsBufIntoVar16(&__code, code);

	if((__code>= 0xAAA1) && (__code<=0xAAFE ))
	{
		__addr = (__code - 0xAAA1 ) * 16 + __baseAdd;
	}
	else if((__code>= 0xABA1) && (__code<=0xABC0 ))
	{
		__addr = (__code - 0xABA1 + 94) * 16+__baseAdd;
	}

	return __addr;
}

/*
**
*/
static u32 __Gb2312Asc5x7GetAddr(u08 *code)
{
	u32 __baseAdd = 0x3bfc0;
	u08 __code;
	u32 __addr;
	
	__code = code[0];

	if((__code >= 0x20) && (__code <= 0x7E))
	{
		__addr = (__code - 0x20 ) * 8 + __baseAdd;
	}
	
	return __addr;
}

/*
**
*/
static u32 __Gb2312Asc7x8GetAddr(u08 *code)
{
	u32 __baseAdd = 0x66c0;
	u08 __code;
	u32 __addr;
	
	__code = code[0];


	if((__code >= 0x20) && (__code <= 0x7E))
	{
		__addr = (__code - 0x20 ) * 8+__baseAdd;
	}
	return __addr;
}

/*
**
*/
static u32 __Gb2312Asc6x12GetAddr(u08 *code)
{
	u32 __baseAdd = 0x66d40;
	u08 __code;
	u32 __addr;
	
	__code = code[0];

	if((__code >= 0x20) && (__code <= 0x7E))
	{
		__addr = (__code - 0x20 ) * 12 + __baseAdd;
	}
	return __addr;
}

/*
**
*/
static u32 __Gb2312Asc8x16GetAddr(u08 *code)
{
	u32 __baseAdd = 0x3b7c0;
	u08 __code;
	u32 __addr;
	
	__code = code[0];

	if((__code >= 0x20) && (__code <= 0x7E))
	{
		__addr = (__code - 0x20 ) * 16 + __baseAdd;
	}
	return __addr;
}

/*
**
*/
void BspGtGetHz11x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
	u08 __image[24];
	u16 __buf16[12];
	u16 __buf16Rota[12];
//	u16 __rotaVal;

	//if(BSP_GT_CHAR_YANG_WEN == yangYin)
	//{
	//	__rotaVal = 0xc003;
	//}
	//else
	//{
	//	__rotaVal = 0;
	//}
	//TRACE("get Hz11 *12!\r\n");

	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Hz11x12GetAddr(code + 2 * k);

		__ZkGtReadBytes(__addr, __image, 24);

		//for(i = 0; i < 24; i++)
		//{
		//	__image[i] ^=  yangYin;
		//}

		OsBuf8IntoBuf16(__image, __buf16, 12);

		//for(i = 0; i < 12; i++)
		//{
		//	__buf16[i] = OsVar16MsbIntoLsb(__buf16[i]);
		//}
		
		//for(i = 0; i < 11; i++)
		//{
		//	__buf16Rota[i] = __rotaVal;
		//	for(j = 0; j < 12; j++)
		//	{
		//		if(__buf16[11 - j] & BIT(i))
		//		{
		//			__buf16Rota[i] |= BIT(j + 2);
		//		}
		//	}
		//}
		
		//OsBuf16IntoBuf8(__buf16Rota, &image[k * 22], 22);
		
		for(i = 0; i < 11; i++)
		{
			__buf16Rota[i] = 0x0000;
			for(j = 0; j < 12; j++)
			{
				if(__buf16[11 - j] & BIT(15-i))
				{
					__buf16Rota[i] |= BIT(j + 2);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 22 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
			image[k * 22 + 2 * i + 1] = (u08)(__buf16Rota[i]);
		}
		
	}
}



/*
**
*/
void BspGtGetHz15x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
	u16 __buf16[16];
	u16 __buf16Rota[16];
	u08 __image[32];
//	u16 __rotaVal;

	//if(BSP_GT_CHAR_YANG_WEN == yangYin  //wzh
	//{
	//	__rotaVal = 0x8000;
	//}
	//else
	//{
	//	__rotaVal = 0;
	//}

	for(k  = 0; k < len; k++)
	{
		__addr = __Gb2312Hz15x16GetAddr(code + 2 * k);

		__ZkGtReadBytes(__addr, __image, 32);

		//for(i = 0; i < 32; i++)  //wzh
		//{
		//	__image[i] ^=  yangYin;
		//}

		OsBuf8IntoBuf16(__image, __buf16, 16);

	//	for(i = 0; i < 16; i++)  //wzh
	//	{
	//		__buf16[i] = OsVar16MsbIntoLsb(__buf16[i]);
	//	}
		
	//	for(i = 0; i < 15; i++)  //wzh
	//	{
	//		__buf16Rota[i] = __rotaVal;	
	//		for(j = 0; j < 16; j++)
	//		{
	//			if(__buf16[15 - j] & BIT(i))			
	//			{
	//				__buf16Rota[i] |= BIT(j);
	//			}
	//		}
	//	}
	//OsBuf16IntoBuf8(__buf16Rota, &image[k * 30], 30);
	
	for(i = 0; i < 15; i++)  //wzh
		{
			__buf16Rota[i] = 0x0000;
			
			for(j = 0; j < 16; j++)
			{
				if(__buf16[15 - j] & BIT(15-i)) 
				{
					__buf16Rota[i] |= BIT(j);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 30 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
		  image[k * 30 + 2 * i + 1] = (u08)(__buf16Rota[i]);
		}
		    		
	}
}


/*
** 6*16
*/
void BspGtGetHz6x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[12];
	u16 __buf16Rota[6];
	u08 __image[12];
//	u16 __rotaVal;

	//if(BSP_GT_CHAR_YANG_WEN == yangYin)
	//{
	//	__rotaVal = 0xc003;
	//}
	//else
	//{
	//	__rotaVal = 0;
	//}
	
	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Hz6x12GetAddr(code + 2 * k);

		__ZkGtReadBytes(__addr, __image, 12);
		
		//for(i = 0; i < 12; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]	= __image[i];
		//	__buf8[i]	= OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 6; i++)
		//{
		//	__buf16Rota[i] = __rotaVal;
			
		//	for(j = 0; j < 12; j++)
		//	{
		//		if(__buf8[11 - j] & BIT(i))
		//		{
		//			__buf16Rota[i] |= BIT(j + 2);
		//		}
		//	}
		//}
		//OsBuf16IntoBuf8(__buf16Rota, &image[k * 12], 12);
		
		for(i = 0; i < 6; i++)
		{
			__buf16Rota[i] = 0x0000;
			
			for(j = 0; j < 12; j++)
			{
				if(__image[11 - j] & BIT(7-i))
				{
					__buf16Rota[i] |= BIT(j + 2);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 12 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
		  image[k * 12 + 2 * i + 1] = (u08)(__buf16Rota[i]);
		}		
		
	}
}


/*
** 8*16
*/
void BspGtGetHz8x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[16];
	u16 __buf16Rota[8];
	u08 __image[16];

	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Hz8x16GetAddr(code + 2 * k);

		__ZkGtReadBytes(__addr, __image, 16);
		
		//for(i = 0; i < 16; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]	= __image[i];
		//	__buf8[i]	= OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 8; i++)
		//{
		//	__buf16Rota[i] = 0x0000;
			
		//	for(j = 0; j < 16; j++)
		//	{
		//		if(__buf8[15 - j] & BIT(i))
		//		{
		//			__buf16Rota[i] |= BIT(j);
		//		}
		//	}
		//}
		//OsBuf16IntoBuf8(__buf16Rota, &image[k * 16], 16);
		
		for(i = 0; i < 8; i++)
		{
			__buf16Rota[i] = 0x0000;
			
			for(j = 0; j < 16; j++)
			{
				if(__image[15 - j] & BIT(7-i))
				{
					__buf16Rota[i] |= BIT(j);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 16 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
		  image[k * 16 + 2 * i + 1] = (u08)(__buf16Rota[i]);
		}		
		
	}
}


/*
**5*8
*/
void BspGtGetAsc5x7Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[8];
	u16 __buf8Rota[8];
	u08 __image[8];
	
	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Asc5x7GetAddr(code + 1 * k);

		__ZkGtReadBytes(__addr, __image, 7);  //wzh
		//__ZkGtReadBytes(__addr, __image, 8);


		//for(i = 0; i < 8; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]   = __image[i];
		//	__buf8[i]   = OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 5; i++)
		//{
		//	__buf8Rota[i] = 0x0000;
		//	
		//	for(j = 0; j < 8; j++)
		//	{
		//		if(__buf8[7 - j] & BIT(i))
		//		{
		//			__buf8Rota[i] |= BIT(j);
		//		}
		//	}
		//}
		
		for(i = 0; i < 5; i++)
		{
			__buf8Rota[i] = 0x0000;
			
			for(j = 0; j < 7; j++)
			{
				if(__image[6 - j] & BIT(7-i))
				{
					__buf8Rota[i] |= BIT(j);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				image[i + k * 5] = ~__buf8Rota[i];
			}
			else
			{
			  image[i + k * 5] = __buf8Rota[i];
			}
		}

		//for(i = 0; i < 5; i++)
		//{
		//	image[i + k * 5] = __buf8Rota[i];
		//}
	}
}

/*
**7 * 8
*/
void BspGtGetAsc7x8Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[8];
	u16 __buf8Rota[8];
	u08 __image[8];
	
	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Asc7x8GetAddr(code + 1 * k);

		__ZkGtReadBytes(__addr, __image, 8);


		//for(i = 0; i < 8; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]	= __image[i];
		//	__buf8[i]	= OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 7; i++)
		//{
		//	__buf8Rota[i] = 0x0000;
		//	
		//	for(j = 0; j < 8; j++)
		//	{
		//		if(__buf8[7 - j] & BIT(i))
		//		{
		//			__buf8Rota[i] |= BIT(j);
		//		}
		//	}
		//}

		//for(i = 0; i < 7; i++)
		//{
		//	image[i + k * 7] = __buf8Rota[i];
		//}
		
		for(i = 0; i < 7; i++)
		{
			__buf8Rota[i] = 0x0000;
			
			for(j = 0; j < 8; j++)
			{
				if(__image[7 - j] & BIT(7-i))
				{
					__buf8Rota[i] |= BIT(j);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				image[i + k * 7] = ~__buf8Rota[i];
			}
			else
			{
			  image[i + k * 7] = __buf8Rota[i];
			}
		}
	}
}

/*
**6 * 16
*/
void BspGtGetAsc6x12Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[12];
	u16 __buf16Rota[6];
	u08 __image[12];
//	u16 __rotaVal;

	//if(BSP_GT_CHAR_YANG_WEN == yangYin)
	//{
	//	__rotaVal = 0xc003;
	//}
	//else
	//{
	//	__rotaVal = 0;
	//}	
	
	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Asc6x12GetAddr(code + 1 * k);

		__ZkGtReadBytes(__addr, __image, 12);
		
		//for(i = 0; i < 12; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]	= __image[i];
		//	__buf8[i]	= OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 6; i++)
		//{
		//	__buf16Rota[i] = __rotaVal;
			
		//	for(j = 0; j < 12; j++)
		//	{
		//		if(__buf8[11 - j] & BIT(i))
		//		{
		//			__buf16Rota[i] |= BIT(j + 2);
		//		}
		//	}
		//}
		
		//OsBuf16IntoBuf8(__buf16Rota, &image[k * 12], 12);
		
		for(i = 0; i < 6; i++)
		{
			__buf16Rota[i] = 0x0000;
			
			for(j = 0; j < 12; j++)
			{
				if(__image[11 - j] & BIT(7-i))
				{
					__buf16Rota[i] |= BIT(j + 2);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 12 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
		  image[k * 12 + 2 * i + 1] = (u08)(__buf16Rota[i]);			
		}
		
	}
}

/*
** 8* 16
*/
void BspGtGetAsc8x16Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u32 __addr;
	u08 i;
	u08 j;
	u16 k;
//	u16 __buf8[16];
	u16 __buf16Rota[8];
	u08 __image[16];
	
	for(k = 0; k < len; k++)
	{
		__addr = __Gb2312Asc8x16GetAddr(code + 1 * k);

		__ZkGtReadBytes(__addr, __image, 16);
		
		//for(i = 0; i < 16; i++)
		//{
		//	__image[i] ^= yangYin;
		//	__buf8[i]	= __image[i];
		//	__buf8[i]	= OsVar8MsbIntoLsb(__buf8[i]);
		//}
		
		//for(i = 0; i < 8; i++)
		//{
		//	__buf16Rota[i] = 0x0000;
			
		//	for(j = 0; j < 16; j++)
		//	{
		//		if(__buf8[15 - j] & BIT(i))
		//		{
		//			__buf16Rota[i] |= BIT(j);
		//		}
		//	}
		//}
		
		//OsBuf16IntoBuf8(__buf16Rota, &image[k * 16], 16);
		
		for(i = 0; i < 8; i++)
		{
			__buf16Rota[i] = 0x0000;
			
			for(j = 0; j < 16; j++)
			{
				if(__image[15 - j] & BIT(7-i))
				{
					__buf16Rota[i] |= BIT(j);
				}
			}
			if(BSP_GT_CHAR_YANG_WEN == yangYin)
			{
				__buf16Rota[i] = ~__buf16Rota[i];
			}
			
			image[k * 16 + 2 * i] = (u08)(__buf16Rota[i]>> 8);
		  image[k * 16 + 2 * i + 1] = (u08)(__buf16Rota[i]);	
		}
		
	}
}

/*
**16*32
*/
void BspGtGetAsc16x32Image(u08 *code, u16 len, u08 *image, bspGtCharYangYinWenTypeEnum yangYin)
{
	u16 k;
	u16 i;

	for(k = 0; k < len; k++)
	{
		//memcpy(&image[k  * 64], asc16X32[code[k] - 0x20], 64);

		//if(yangYin == BSP_GT_CHAR_YIN_WEN)
		//{
		//	for(i = 0; i < 64; i++)
		//	{
		//		image[k  * 64 + i] ^= 0xFF;
		//	}
		//}
		
		if(yangYin == BSP_GT_CHAR_YIN_WEN)
		{
		   for(i = 0; i < 64; i++)
		  	{
				  image[k  * 64 + i] = ~ asc16X32[code[k] - 0x20][i];
			  }
		}
		else
    { 
			 for(i = 0; i < 64; i++)
			  {
				  image[k  * 64 + i] =  asc16X32[code[k] - 0x20][i];
			  }
		}
		
	}
}

/*
**
*/
void BspGt21l16S2wInit(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};

	__ZK_GT_CS_GPIO_CLK_ENABLE();	 
	
	__gpioInitSt.Pin   = __ZK_GT_CS_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(__ZK_GT_CS_PORT, &__gpioInitSt);
	__ZK_GT_CS_(1);

	BSP_ZK_GT_PWR_CTRL_GPIO_CLK_ENABLE();
	
	__gpioInitSt.Pin   = BSP_ZK_GT_PWR_CTRL_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_ZK_GT_PWR_CTRL_PORT, &__gpioInitSt);
	BSP_ZK_GT_PWR_CTRL_(0); 

	zkGtSpi.Instance               = BSP_ZK_GT_SPI;
	zkGtSpi.Init.Mode              = SPI_MODE_MASTER;
	zkGtSpi.Init.Direction         = SPI_DIRECTION_2LINES;
	zkGtSpi.Init.DataSize          = SPI_DATASIZE_8BIT;
	zkGtSpi.Init.CLKPolarity       = SPI_POLARITY_LOW;
	zkGtSpi.Init.CLKPhase          = SPI_PHASE_1EDGE;
	zkGtSpi.Init.NSS               = SPI_NSS_SOFT;
	zkGtSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; //wzhSPI_BAUDRATEPRESCALER_2;//
	zkGtSpi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	zkGtSpi.Init.TIMode            = SPI_TIMODE_DISABLE;
	zkGtSpi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	zkGtSpi.Init.CRCPolynomial     = 10;
	if(HAL_SPI_Init(&zkGtSpi) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_SPI_ENABLE(&zkGtSpi);                    //使能SPI
}

/*
**
*/
static u08 __ZkGtReadWriteByte(u08 TxData)
{
	u08 Rxdata;
	HAL_SPI_TransmitReceive(&zkGtSpi,&TxData,&Rxdata,1, 1000); 	  
	return Rxdata;						//返回收到的数据		
}

/*
**
*/
void BspGt21l16S2wSleep(void)
{
	GPIO_InitTypeDef __gpioInitSt = {0};

	__ZK_GT_CS_(0);
	BSP_ZK_GT_PWR_CTRL_(1); 
	HAL_SPI_DeInit(&zkGtSpi);

	BSP_ZK_GT_SCK_(0); 
	
	__gpioInitSt.Pin   = BSP_ZK_GT_SCK_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_ZK_GT_SCK_PORT, &__gpioInitSt);

	BSP_ZK_GT_MISO_(0); 

	__gpioInitSt.Pin   = BSP_ZK_GT_MISO_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_ZK_GT_MISO_PORT, &__gpioInitSt);

	BSP_ZK_GT_MOSI_(0); 

	__gpioInitSt.Pin   = BSP_ZK_GT_MOSI_PIN;
	__gpioInitSt.Mode  = GPIO_MODE_OUTPUT_PP;
	__gpioInitSt.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BSP_ZK_GT_MOSI_PORT, &__gpioInitSt);
}

/*
**
*/
void BspGt21l16S2wAwake(void)
{
	BspGt21l16S2wInit();
	//BSP_ZK_GT_PWR_CTRL_(0); 
}


