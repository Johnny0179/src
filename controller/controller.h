#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "openamp/inter_core_com.h"
#include "controller/maxon/maxon.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

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

// motor command
typedef struct controller_cmd_type
{
    // controller enable control
    u8 controller_state;

    // mode of operation
    u16 mode_of_operation;

    // target relative position
    s32 target_rela_pos;

    // target speed
    s32 target_speed;

    // target torque
    s16 target_torque;
} controller_cmd;

// r5 system command structure
typedef struct controllers_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // control frequency
    float control_frequency_KHz;

    // controllers cmd
    controller_cmd controller1_cmd;
    controller_cmd controller2_cmd;
    controller_cmd controller3_cmd;
    controller_cmd controller4_cmd;
    controller_cmd controller5_cmd;
    controller_cmd controller6_cmd;

} controllers_cmd;

//forward declare
typedef struct controller_type controller;

// controller type
typedef struct controller_type
{
    // controller command
    controller_cmd *contrl_cmd;

    // controller state

    // maxon motor
    struct maxon *motor;

    // pid controller
    void(*Poll)(const controller *ctrler);

}controller;

// controller poll with specified frequency
void ControllersPoll(void *ptr);

#endif
