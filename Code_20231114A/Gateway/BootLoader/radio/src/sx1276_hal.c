/*
   / _____)             _              | |
   ( (____  _____ ____ _| |_ _____  ____| |__
   \____ \| ___ |    (_   _) ___ |/ ___)  _ \
   _____) ) ____| | | || |_| ____( (___| | | |
   (______/|_____)_|_|_| \__)_____)\____)_| |_|
   (C)2013 Semtech

Description: Generic SX1276 driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Wael Guibene
*/

#include "sx1276_hal.h"

//控制发射接收引脚 高收低发
void HalWriteRxTx(uint8_t level)
{

}



void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t millis(void)
{
    return HAL_GetTick();
}
