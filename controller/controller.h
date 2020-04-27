#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "openamp/inter_core_com.h"
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

// controller mode
#define POS_MODE 0x01
#define SPEED_MODE 0x03
#define TORQUE_MODE 0x0A


// forward declaration
typedef struct controller_cmd_type controller_cmd;
typedef struct controller_type controller;


// controller type
typedef struct controller_type
{
    // controller command
    controller_cmd *contrl_cmd;

    // controller state

    // maxon motor
    struct maxon *motor;

    //controller poll function
    void (*Poll)(controller *controller, u32 control_freq);

} controller;

// controller poll with specified frequency
//void ControllerPoll(controller *controller, u32 control_freq);

#endif
