#ifndef DELAY_H
#define DELAY_H

/* freertos includes */
#include "FreeRTOS.h"
//#include "queue.h"
//#include "semphr.h"
//#include "task.h"

/* xlinx includes */
#include "xil_types.h"
#include <sleep.h>



// delay n ms, this function can only be used in the task!
void delay_ms(u32 ms);
void delay_us(u32 us);

#endif
