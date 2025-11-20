#include "includes.h"

/*
**
*/

/*
**
*/
static u16 sysTick;  //wzh  u08 sysTick;


/*
**
*/

//delay 1 system tick, 1ms  
delayReachEnum SystemDelay(osDelaySt *osDelayS, u16 ms)
{
	//wzh
	if (osDelayS->ticks  == 0)
	{
		osDelayS->ticks  = sysTick;		
		return DELAY_UNREACH;
	}
	
	if  (sysTick < osDelayS->ticks)
	{
		osDelayS->delays = 65536 - (osDelayS->ticks - sysTick);
	}
	else
	{
		osDelayS->delays = sysTick - osDelayS->ticks;
	}
		
	if((osDelayS->delays) >= ms)
	{
		osDelayS->ticks  = sysTick;		
		return DELAY_REACH;
	}
	else
	{
		return DELAY_UNREACH;
	}
	
	//if((osDelayS->delays) >= ms)
	//{
	//	osDelayS->ticks  = sysTick;
	//	osDelayS->delays = 0;
	//	return DELAY_REACH;
	//}
	
	//if(osDelayS->ticks == sysTick)
	//{
	//	return DELAY_UNREACH;
	//}
	//osDelayS->ticks = sysTick;

	//if(osDelayS->delays < ms)
	//{
	//	osDelayS->delays++;
	//	return DELAY_UNREACH;
	//}
	//osDelayS->delays = 0;
	//return DELAY_REACH;
}



void OsMemCpy(u08 *desBuf, u08 *sourceBuf, u16 len)
{
	u16 i;

	for(i = 0; i < len; i++)
	{
		*desBuf++ = *sourceBuf++;
	}
}


void OsInit(void)
{

	sysTick = 0;
}

void OsDeInit(void)
{
	sysTick = 0;
}

/*
**
*/
void OsTimeBaseEvent(void)
{
	sysTick++;
}

/*
**
*/
int OsStrloc(char *str1,char *str2)
{
	int la = strlen(str2);
	int i,j;
	int lb = strlen(str1);
	for(i = 0; i < lb; i++)
	{
		for(j = 0; j < la && i + j < lb && str2[j] == str1[i + j]; j++)
		{
			;
		}
		if(j == la)
		{
			return i;
		}
	}
	return -1;
}

/*
**
*/
char OsStrCmp(u08 *str1, u08 *str2, u16 len)
{
	u08 i;

	for(i = 0; i < len; i++)
	{
		if(str1[i] != str2[i])
		{
			return (i + 1);
		}
	}

	return 0;
}

/*
**
*/
#if  0
char OsStrStr(u08 *str1, u08 len1, u08 *str2, u08 len2)
{
	u08 i;  //str1
	u08 j = 0;  //str2

	for(i = 0; i < len1; i++)
	{
		if(str2[0] == str1[i])
		{
			break;
		}
	}

	if(i >= len1)
	{
		return 1;
	}

	for(j = 0; j < len2; j++)
	{
		if(str1[i + j] != str2[j])
		{
			return 1;
		}
	}

	return 0;
}
#endif

#if 1
u08 isSubset(u08 arr1[],  u08 m, u08 arr2[], u08 n)     
{   
	u08 i = 0;         
	u08 j = 0;         
	for (i = 0; i < n; i++)         
	{             
		for (j = 0; j < m; j++)
		{
			if(arr2[i] == arr1[j])                     
			{
				break; 
			}
	         
			if (j == m) 
			{
				return false;
			}
		}
	}                         
	return true; 
} 

#endif

/*
**
*/
void OsVar16IntoBuf(u16 var, u08 *buf)
{
	buf[0] = (u08)(var >> 8);
	buf[1] = (u08)(var);
}

/*
**
*/
void OsVar32IntoBuf(u32 var, u08 *buf)
{
	buf[0] = (u08)(var >> 24);
	buf[1] = (u08)(var >> 16);
	buf[2] = (u08)(var >> 8);
	buf[3] = (u08)(var);
}


/*
**
*/
char OsStrStr(u08 *str1, u16 len1, u08 *str2, u16 len2)
{
	u16 i;  //str1

	if(len1 < len2)
	{
		return 1;
	}

	for(i = 0; i < len2; i++)
	{
		if(str1[i] != str2[i])
		{
			return 1;
		}
	}
	return 0;
}

/*
**
*/
void OsBufIntoVar16(u16 *var, u08 *buf)
{
	*var = ((u16)buf[0] << 8) | ((u16)buf[1]);
}

/*
**
*/
void OsBufIntoVar32(u32 *var, u08 *buf)
{
	*var = ((u32)buf[0] << 24) | 
	       ((u32)buf[1] << 16) |
	       ((u32)buf[2] << 8) |
	       ((u32)buf[3]);
}


