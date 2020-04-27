#ifndef INTER_CORE_COM_H
#define INTER_CORE_COM_H

#include <metal/alloc.h>
#include <openamp/framework/platform_info.h>
#include <openamp/framework/rpmsg-echo.h>
#include <openamp/framework/rsc_table.h>
#include <openamp/open_amp.h>

#include "controller/controller.h"

// max rpmsg size is 496 bytes
#define MAX_RPMSG_SIZE 496

// define the rpmsg type
#define SEND_R5_CMD_FROM_APU 1
#define READ_R5_STATE_FROM_APU 2
#define SEND_CONTROLLER_CMD_FROM_APU 3
#define READ_CONTROLLER_STATE_FROM_APU 4

// r5 system state structure
typedef struct r5_state_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // ps core temperature
    u8 ps_core_temp;

    // pl core temperature
    u8 pl_core_temp;

    // nmt state
    u8 nmt_state;

} r5_state;

// r5 system command state structure
typedef struct r5_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // nmt control
    u8 nmt_control;

} r5_cmd;

// send controller command
typedef struct controller_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // motor id
    u8 motor_id;

    // motor enable
    u8 motor_enable;

    // mode of operation
    u16 mode_of_operation;

    // target absolute position
    s32 target_abs_pos;

    // target relative position
    s32 target_rel_pos;

    // target speed
    s32 target_speed;

    // target torque
    s16 target_torque;

} controller_cmd;

// inter core communication
int InterCoreCom(struct rpmsg_endpoint *ept, size_t len, void *data);

#endif