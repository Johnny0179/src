#ifndef NMT_H
#define NMT_H

#include "canopen/can.h"
#include "maxon/maxon.h"
#include "delay/delay.h"

// NMT Command Specifier, sent by master to change a slave state, Should not be
// modified.
#define NMT_Start_Node 0x01
#define NMT_Stop_Node 0x02
#define NMT_Enter_PreOperational 0x80
#define NMT_Reset_Node 0x81
#define NMT_Reset_Comunication 0x82

/* CANopen Function Codes */
#define NMT (u16)0x0 << 7
#define SYNC (u16)0x1 << 7
#define TIME_STAMP (u16)0x2 << 7

/* CANopen Function Codes */
#define PDO1tx (u16)0x3 << 7
#define PDO1rx (u16)0x4 << 7
#define PDO2tx (u16)0x5 << 7
#define PDO2rx (u16)0x6 << 7
#define PDO3tx (u16)0x7 << 7
#define PDO3rx (u16)0x8 << 7
#define PDO4tx (u16)0x9 << 7
#define PDO4rx (u16)0xA << 7
#define SDOtx (u16)0xB << 7
#define SDOrx (u16)0xC << 7
#define NODE_GUARD (u16)0xE << 7
#define LSS (u16)0xF << 7

/* -------------------------NMT functions------------------------------ */
void NMTstart(void);
void NMTstartID(u8 slave_id);
void NMTPreOperation(u8 slave_id);
void NMTstop(u8 slave_id);
void NMTCmdSync();
int NMTInit(void);

// canopen
int NMTTxPdo1(u8 slave_id, u16 ctrl_wrd);

int NMTTxPdo2(u8 slave_id, u16 ctrl_wrd, s32 pos_sv, u16 mode_of_operation);

// for torque set
int NMTTxPdo3(u8 slave_id, s16 target_torque, u16 mode_of_operation);

int NMTTxPdo4(u8 slave_id, s32 speed_set, u16 mode_of_operation);

// rxpdo
int NMTRxPdo1(u8 id);
// int RxPdo2(u8 id);
// int RxPdo3(u8 id);
// int RxPdo4(u8 id);

void MotorParaRead(struct can_frame *recv_frame);



struct nmt {
  // can device
  struct can *can_device;

  // init
  int (*Init)(void);

  // nmt
  void (*Start)(void);
  void (*StartId)(u8 slave_id);
  void (*PreOperation)(u8 slave_id);
  void (*Stop)(u8 slave_id);
  void (*CmdSync)(void);

  // pdo
  int (*TxPdo1)(u8 slave_id, u16 ctrl_wrd);
  int (*TxPdo2)(u8 slave_id, u16 ctrl_wrd, s32 pos_sv, u16 mode_of_operation);
  int (*TxPdo3)(u8 slave_id, s16 target_torque, u16 mode_of_operation);
  int (*TxPdo4)(u8 slave_id, s32 speed_set, u16 mode_of_operation);
  int (*RxPdo1)(u8 id);

  // read parameter
  void (*ParaRead)(struct can_frame *recv_frame);
};

#endif
