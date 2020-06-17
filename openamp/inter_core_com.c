#include "inter_core_com.h"

extern r5_cmd R5_cmd;
extern r5_state R5_state;

// controllers command
extern struct controllers_cmd controllers_command;

int InterCoreCom(struct rpmsg_endpoint *ept, size_t len, void *data)
{
    int state;
    u8 temp_buff[MAX_RPMSG_SIZE] = {0};
    u8 *ptr = (u8 *)data;

    // process the rpmsg
    switch (*ptr)
    {

    // send cmd to R5 core
    case SEND_R5_CMD_FROM_APU:
        // copy the command
        memcpy(&R5_cmd, (u8 *)data, sizeof(r5_cmd));
        break;

    // read the state of R5 core
    case READ_R5_STATE_FROM_APU:
        R5_state.rpmsg_type = READ_R5_STATE_FROM_APU;
        // copy the R5 state
        memcpy(temp_buff, &R5_state, sizeof(r5_state));
        // send back the R5 state
        state = rpmsg_send(ept, temp_buff, len);
        break;

    // send the command of controller
    case SEND_CONTROLLER_CMD_FROM_APU:
        // copy the controllers command
        // memcpy(&controllers_command, (u8 *)data, sizeof(struct controllers_cmd));
        break;

    default:
        break;
    }

    return state;
}
