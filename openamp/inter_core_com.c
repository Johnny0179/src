#include "inter_core_com.h"

r5_cmd R5_cmd;
r5_state R5_state;

int InterCoreCom(struct rpmsg_endpoint *ept, size_t len, u8 *rpmsg_type)
{
    int state;
    u8 temp_buff[MAX_RPMSG_SIZE] = {0};

    // process the rpmsg
    switch (*rpmsg_type)
    {
        // read the state of R5 core
    case READ_R5_STATE_FROM_APU:
        R5_state.rpmsg_type = READ_R5_STATE_FROM_APU;
        memcpy(temp_buff, &R5_state, sizeof(r5_state));
        state = rpmsg_send(ept, temp_buff, len);
        break;

    default:
        break;
    }
    return state;
}