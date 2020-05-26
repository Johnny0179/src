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
// #define MOTOR_NUM 6

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

// controller state control
#define CONTROLLER_DISABLE 1
#define CONTROLLER_ENABLE 2

// pulley operation mode

// pull master direction
#define PULL_UP 1
#define PULL_DOWN 2

// claw control
#define CLAW_HOLD 1
#define CLAW_LOOSE 2

// claw control command
typedef struct claw_control_cmd_type
{
    // motion mode
    u8 motion_mode;

    // hold torque;
    s16 hold_torque;

    // loose distance
    s32 loose_dis;

} claw_control_cmd;

// upwheel control command
typedef struct upwheel_control_cmd_type
{
    // motion mode
    u8 motion_mode;

    // move distance
    s32 move_dis;

} upwheel_control_cmd;

// define pulley motion mode
#define IDLE_MODE 0 //idle mode
#define FOLLOW_MODE 1 //follow slave, impedance control
#define PULL_MODE 2   //pull master

// pulley controll command
typedef struct pulley_control_cmd_type
{
    //choose motion mode
    u8 motion_mode;

    // follow slave
    u8 follow_slave;

    // pull master distance
    s32 pull_dis;

} pulley_control_cmd;

// // motor command
// typedef struct controller_cmd_type
// {
//     // controller enable control
//     u8 controller_state;

//     // claw control
//     claw_control_cmd claw_control_cmd_;

//     // move distance for upwheel
//     upwheel_control_cmd upwheel_control_cmd_;

//     // pulley control
//     pulley_control_cmd pulley_control_cmd_;

// } controller_cmd;

// r5 system command structure
typedef struct controllers_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // control frequency
    float control_frequency_KHz;

    // controllers cmd
    claw_control_cmd upclaw_cmd_;
    upwheel_control_cmd upwheel_cmd_;
    pulley_control_cmd pulley1_cmd_;
    pulley_control_cmd pulley2_cmd_;
    claw_control_cmd downclaw1_cmd_;
    claw_control_cmd downclaw2_cmd_;

} controllers_cmd;

//forward declare
typedef struct controller_type controller;

// controller type
typedef struct controller_type
{
    // controller command
    claw_control_cmd *claw_control_cmd_;
    upwheel_control_cmd *upwheel_control_cmd_;
    pulley_control_cmd *pulley_control_cmd_;

    // enable controller
    void (*enable)(const controller *controller);
    // controller poll
    void (*poll)(const controller *controller);

    // Impedance control

    // maxon motor
    struct maxon *motor;

} controller;

// controller poll with specified frequency
void ControllersPoll(void *ptr);

#endif
