#ifndef _COMMON_H_
#define _COMMON_H_

#include "xil_printf.h"

#define RPU_PRINTF(format, ...) xil_printf("\r[r5-0] " format, ##__VA_ARGS__)



#endif