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

// class can {
// private:
//  /* data */
////   int s_;
//
//  // CAN filter number, 8 canopen devices, each has 4 TxPDO.
////   static const __u8 kNodeNum = 6;
//
////   static const int kCanFilterNum = 24;
//
// public:
//  can(/* args */);
//  ~can();
//
//  int send(const struct can_frame *tx_frame);
//  int receive(can_frame *recv_frame);
//};

// struct remoteproc_ops {
//	struct remoteproc *(*init)(struct remoteproc *rproc,
//				   struct remoteproc_ops *ops, void *arg);
//	void (*remove)(struct remoteproc *rproc);
//	void *(*mmap)(struct remoteproc *rproc,
//		      metal_phys_addr_t *pa, metal_phys_addr_t *da,
//		      size_t size, unsigned int attribute,
//		      struct metal_io_region **io);
//	int (*handle_rsc)(struct remoteproc *rproc, void *rsc, size_t len);
//	int (*config)(struct remoteproc *rproc, void *data);
//	int (*start)(struct remoteproc *rproc);
//	int (*stop)(struct remoteproc *rproc);
//	int (*shutdown)(struct remoteproc *rproc);
//	int (*notify)(struct remoteproc *rproc, uint32_t id);
//};

#endif
