#ifndef CAN_H
#define CAN_H

#include "xil_printf.h"

#include "canopen/ps_can0.h"

//struct can_frame {
//  u32 can_id;  /* 32 bit CAN_ID */
//  u32 can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
//  u32 data[CAN_MAX_DLEN];
//};

struct can {
  //	init can device
  int (*init)(void);
  // send a frame
  int (*send)(const struct can_frame *tx_frame);
};


#endif
