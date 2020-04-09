#include "delay/delay.h"

// delay ms
void delay_ms(u32 ms){
//portTICK_PERIOD_MS is 1ms
	vTaskDelay(portTICK_PERIOD_MS*ms);
}

//void delay_us(u32 us){
////usleep(us);
//}

