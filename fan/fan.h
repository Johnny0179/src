#ifndef FAN_H_
#define FAN_H_
#define FAN_GPIO								78		//EMIO0
#define FAN_ON									0
#define FAN_OFF 								1
#define GPIO_DIR_OUTPUT 				1


void FanInit ();

void FanOn ();
void FanOff ();

#endif
