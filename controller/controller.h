#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "controller/maxon/maxon.h"

/* rpmsg definition */


// motor number
#define MOTOR_NUM 6



// motor id
#define UP_CLAW 1
#define UP_WHEEL 2
#define PULLEY1 3
#define PULLEY2 4
#define DOWN_CLAW1 5
#define DOWN_CLAW2 6

// init motor controllers
// int ControllersInit(u8 num_of_motors);

// controller poll with specified frequency
void ControllerPoll(u32 control_freq);

#endif
