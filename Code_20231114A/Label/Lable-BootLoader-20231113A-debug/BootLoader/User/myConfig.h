#ifndef __MY_CONFIG_H_
#define __MY_CONFIG_H_

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u08; 

typedef signed int   s32;
typedef signed short s16;
typedef signed char  s08; 


typedef const unsigned int uc32;  /*!< Read Only */
typedef const unsigned short uc16;  /*!< Read Only */
typedef const unsigned char uc8;   /*!< Read Only */

typedef volatile unsigned int  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile unsigned int vuc32;  /*!< Read Only */
typedef volatile unsigned short vuc16;  /*!< Read Only */
typedef volatile unsigned char vuc8;   /*!< Read Only */


#define BIT(x)  (1 << (x))


#endif


