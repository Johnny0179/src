#ifndef _COMMON_H_
#define _COMMON_H_

#include "xil_printf.h"

#define RPU_PRINTF(format, ...) xil_printf("\r[r5-0] "format, ##__VA_ARGS__)

// rpmsg type
#define SEND_R5_CMD_FROM_APU 0
#define READ_R5_STATE_FROM_APU 1
#define SEND_CONTROLLER_CMD_FROM_APU 2
#define READ_CONTROLLER_CMD_FROM_APU 3

#endif