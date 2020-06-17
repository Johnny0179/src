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

// motion command
enum motion_cmd
{
    IDLE = 0,
    UPCLAW_HOLD,
    UPCLAW_LOOSE,
    UpwheelMoveUp,
    UpwheelMoveDown,
    PulleysTighten,
    PulleysLockPos,
    PulleysAdmittanceControl,
    PulleysPullUp,
    PulleysPullDown,
    DownclawsHold,
    DownclawsLoose
};

// r5 system command state structure
typedef struct r5_cmd_type
{
    // the rpmsg type
    u8 rpmsg_type;

    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // nmt control
    u8 nmt_control;

    /* controllers command */
    // control frequency
    u32 control_frequency_KHz;

    // motion command
    enum motion_cmd cntrlr_motion_cmd;

    // upclaw hold torque
    s16 upclaw_hold_trq;

    // upclaw loose relative position
    s32 upclaw_rltv_pos;

} r5_cmd;

// inter core communication
int InterCoreCom(struct rpmsg_endpoint *ept, size_t len, void *data);

#endif
