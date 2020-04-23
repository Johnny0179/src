#include "delay/delay.h"

// delay ms
void delay_ms(u32 ms)
{
	usleep(1000 * ms);
}

void delay_us(u32 us)
{
	usleep(us);
}
