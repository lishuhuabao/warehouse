#ifndef __INCLUDES_H_
#define __INCLUDES_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>

#include "stm32l1xx_hal.h"
#include "myConfig.h"
#include "main.h"

#include "Bsp.h"
//#include "BspAdc.h"
//#include "BspBat.h"
#include "BspBuz.h"
#include "BspDebug.h"
#include "BspDataRecord.h"
//#include "BspEncry.h"
#include "BspIntFlash.h"
#include "BspGt21L16S2W.h"
#include "BspKey.h"
#include "BspIdwg.h"
#include "BspLed.h"
#include "BspLcdWf29.h"
#include "BspLcdWf29Font.h"
#include "BspLora.h"
#include "BspPort.h"
#include "BspRtc.h"
#include "BspSimPwm.h"

#include "CommLora.h"
#include "CommLoraPro.h"

#include "HandlerKey.h"
#include "HandlerProcess.h"
#include "HandlerPowerManeg.h"

#include "usrOs.h"
#include "CommIap.h"

#include "stask.h"
#include "stimer.h"

#include "sx1276_Hal.h"
#include "sx1276.h"
#include "radio.h"
#include "sx1276_timer.h"
#include "sx1276_user.h"
#include "sx1276-board.h"
#include "sx1276Regs-fsk.h"
#include "sx1276Regs-Lora.h"

#endif

