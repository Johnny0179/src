#ifndef PS_CAN0_H
#define PS_CAN0_H

#if defined __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xcanps.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "can.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif

/************************** Constant Definitions *****************************/

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC 8
#define CAN_MAX_DLEN 8

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC XIntc
#define CAN_DEVICE_ID XPAR_XCANPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID
#define CAN_INTR_VEC_ID XPAR_INTC_0_CANPS_0_VEC_ID
#else
#define INTC XScuGic
#define CAN_DEVICE_ID XPAR_XCANPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define CAN_INTR_VEC_ID XPAR_XCANPS_0_INTR
#endif
/* Maximum CAN frame length in word */
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS (XCANPS_MAX_FRAME_SIZE / sizeof(u32))

#define FRAME_DATA_LENGTH 8 /* Frame Data field length */

/*
 * Message Id Constant.
 */
#define TEST_MESSAGE_ID 2000

/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 40Kbps, assuming that the
 * the CAN clock is 24MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 */

/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 40 Kbps baudrate assuming the CAN input clock
 * frequency is 24 MHz.
 */
#define TEST_BTR_SYNCJUMPWIDTH 3
#define TEST_BTR_SECOND_TIMESEGMENT 2
#define TEST_BTR_FIRST_TIMESEGMENT 15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define TEST_BRPR_BAUD_PRESCALAR 29

struct can_frame {
  u32 can_id;  /* 32 bit CAN_ID */
  u32 can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
  u8 data[CAN_MAX_DLEN];
};

/************************** Function Prototypes ******************************/
int PsCan0Init(void);
int CanPsIntrInit(INTC *IntcInstPtr, XCanPs *CanInstPtr, u16 CanDeviceId,
                  u16 CanIntrId);

int SendFrame(const struct can_frame *tx_frame);

// void SendHandler(void *CallBackRef);
// void RecvHandler(void *CallBackRef);
// void ErrorHandler(void *CallBackRef, u32 ErrorMask);
// void EventHandler(void *CallBackRef, u32 Mask);

// int SetupInterruptSystem(INTC *IntcInstancePtr, XCanPs *CanInstancePtr,
//                          u16 CanIntrId);

#if defined __cplusplus
}
#endif

#endif
