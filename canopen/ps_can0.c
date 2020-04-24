/******************************************************************************
 *
 * Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
/****************************************************************************/
/**
 *
 * @file xcanps_intr_example.c
 *
 * Contains an example of how to use the XCanPs driver directly.
 * This example shows the usage of the driver/device in interrupt mode.
 *
 * @note
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 40Kbps, assuming that the
 * the CAN clock is 24MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who    Date	Changes
 * ----- -----  -------- -----------------------------------------------
 * 1.00a xd/sv  01/12/10 First release
 * 2.1 adk 		23/08/14 Fixed CR:798792 Peripheral test for CANPS IP in
 *						 SDK claims a 40kbps baud rate
 *but it's not. 3.1   mus    01/14/16 Added support for intc interrupt
 *controller
 * </pre>
 *
 ******************************************************************************/

#include "canopen/ps_can0.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "canopen/nmt.h"
#include "controller/controller.h"
#include "delay/delay.h"
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

#ifndef TESTAPP_GEN
XCanPs CanInstance; /* Instance of the Can driver */
INTC IntcInstance;  /* Instance of the Interrupt Controller driver */

extern XScuGic xInterruptController;
extern struct nmt nmt;
#endif

/* queue */
extern QueueHandle_t xCANQueue;

// mutex
extern SemaphoreHandle_t xCANMutex;

/*
 * Shared variables used to test the callbacks.
 */
volatile int LoopbackError; /* Asynchronous error occurred */
volatile int RecvDone;      /* Received a frame */
volatile int SendDone;      /* Frame was sent successfully */

static void Config(XCanPs *InstancePtr);
static void SendHandler(void *CallBackRef);
static void RecvHandler(void *CallBackRef);
static void ErrorHandler(void *CallBackRef, u32 ErrorMask);
static void EventHandler(void *CallBackRef, u32 Mask);
static int SetupInterruptSystem(INTC *IntcInstancePtr, XCanPs *CanInstancePtr,
                                u16 CanIntrId);

/****************************************************************************/
/**
 *
 * This function is the main function of the Can interrupt example.
 *
 * @param	None.
 *
 * @return
 *		- XST_SUCCESS if the example has completed successfully.
 *		- XST_FAILURE if the example has failed.
 *
 * @note		None.
 *
 *****************************************************************************/
#ifndef TESTAPP_GEN
int PsCan0Init()
{
  int Status;

  xil_printf("ps can0 starts initialization!\r\n");

  /*
   * Run the Can interrupt example.
   */
  Status = CanPsIntrInit(&xInterruptController, &CanInstance, CAN_DEVICE_ID,
                         CAN_INTR_VEC_ID);
  if (Status != XST_SUCCESS)
  {
    xil_printf("ps can0 Initialization failed!\r\n");
    return XST_FAILURE;
  }

  xil_printf("ps can0 initialization succeeded!\r\n");
  return XST_SUCCESS;
}
#endif

/*****************************************************************************/
/**
 *
 * The main entry point for showing the XCanPs driver in interrupt mode.
 * The example configures the device for internal loop back mode, then
 * sends a CAN frame and receives the same CAN frame.
 *
 * @param	IntcInstPtr is a pointer to the instance of the INTC driver.
 * @param	CanInstPtr is a pointer to the instance of the CAN driver which
 *		is going to be connected to the interrupt controller.
 * @param	CanDeviceId is the device Id of the CAN device and is typically
 *		XPAR_<CANPS_instance>_DEVICE_ID value from xparameters.h.
 * @param	CanIntrId is the interrupt Id and is typically
 *		XPAR_<CANPS_instance>_INTR value from xparameters.h.
 *
 * @return	XST_SUCCESS if successful, otherwise driver-specific error code.
 *
 * @note		If the device is not working correctly, this function
 *may enter an infinite loop and will never return to the caller.
 *
 ******************************************************************************/
int CanPsIntrInit(INTC *IntcInstPtr, XCanPs *CanInstPtr, u16 CanDeviceId,
                  u16 CanIntrId)
{
  int Status;
  XCanPs_Config *ConfigPtr;

  /*
   * Initialize the Can device.
   */
  ConfigPtr = XCanPs_LookupConfig(CanDeviceId);
  if (ConfigPtr == NULL)
  {
    return XST_FAILURE;
  }
  XCanPs_CfgInitialize(CanInstPtr, ConfigPtr, ConfigPtr->BaseAddr);

  /*
   * Run self-test on the device, which verifies basic sanity of the
   * device and the driver.
   */
  Status = XCanPs_SelfTest(CanInstPtr);
  if (Status != XST_SUCCESS)
  {
    return XST_FAILURE;
  }

  /*
   * Configure CAN device.
   */
  Config(CanInstPtr);

  /*
   * Set interrupt handlers.
   */
  XCanPs_SetHandler(CanInstPtr, XCANPS_HANDLER_SEND, (void *)SendHandler,
                    (void *)CanInstPtr);
  XCanPs_SetHandler(CanInstPtr, XCANPS_HANDLER_RECV, (void *)RecvHandler,
                    (void *)CanInstPtr);
  XCanPs_SetHandler(CanInstPtr, XCANPS_HANDLER_ERROR, (void *)ErrorHandler,
                    (void *)CanInstPtr);
  XCanPs_SetHandler(CanInstPtr, XCANPS_HANDLER_EVENT, (void *)EventHandler,
                    (void *)CanInstPtr);

  /*
   * Initialize the flags.
   */
  SendDone = FALSE;
  RecvDone = FALSE;
  LoopbackError = FALSE;

  /*
   * Connect to the interrupt controller.
   */
  Status = SetupInterruptSystem(IntcInstPtr, CanInstPtr, CanIntrId);
  if (Status != XST_SUCCESS)
  {
    xil_printf("interrupt system set up failed!\n");
    return XST_FAILURE;
  }
  else
  {
    xil_printf("interrupt system set up success!\n");
  }

  /*
   * Enable all interrupts in CAN device.
   */
  XCanPs_IntrEnable(CanInstPtr, XCANPS_IXR_ALL);

/*
   * Enter Normal Mode.
   */
#if CAN_TEST
  // loopback mode for test
  XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_LOOPBACK);
  while (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_LOOPBACK)
    ;

#else
  XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_NORMAL);
  while (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_NORMAL)
    ;
#endif

  //   /*
  //    * Loop back a frame. The RecvHandler is expected to handle
  //    * the frame reception.
  //    */
  //   SendFrame(CanInstPtr); /* Send a frame */

  //   /*
  //    * Wait here until both sending and reception have been completed.
  //    */
  //   while ((SendDone != TRUE) || (RecvDone != TRUE))
  //     ;

  //   /*
  //    * Check for errors found in the callbacks.
  //    */
  //   if (LoopbackError == TRUE) {
  //     return XST_LOOPBACK_ERROR;
  //   }

  return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function configures CAN device. Baud Rate Prescaler Register (BRPR) and
 * Bit Timing Register (BTR) are set in this function.
 *
 * @param	InstancePtr is a pointer to the driver instance.
 *
 * @return	None.
 *
 * @note		If the CAN device is not working correctly, this
 *function may enter an infinite loop and will never return to the caller.
 *
 ******************************************************************************/
static void Config(XCanPs *InstancePtr)
{
  /*
   * Enter Configuration Mode if the device is not currently in
   * Configuration Mode.
   */
  XCanPs_EnterMode(InstancePtr, XCANPS_MODE_CONFIG);
  while (XCanPs_GetMode(InstancePtr) != XCANPS_MODE_CONFIG)
    ;

  /*
   * Setup Baud Rate Prescaler Register (BRPR) and
   * Bit Timing Register (BTR).
   */
  XCanPs_SetBaudRatePrescaler(InstancePtr, TEST_BRPR_BAUD_PRESCALAR);
  XCanPs_SetBitTiming(InstancePtr, TEST_BTR_SYNCJUMPWIDTH,
                      TEST_BTR_SECOND_TIMESEGMENT, TEST_BTR_FIRST_TIMESEGMENT);
}

/*****************************************************************************/
/**
 *
 * Send a CAN frame.
 *
 * @param	InstancePtr is a pointer to the driver instance.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
int SendFrame(const struct can_frame *tx_frame)
{
  u8 *FramePtr;
  int Index;
  int Status;
  u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
  /*
   * Create correct values for Identifier and Data Length Code Register.
   */
  TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)tx_frame->can_id, 0, 0, 0, 0);
  TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)tx_frame->can_dlc);
  // TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);

  /*
   * Now fill in the data field with known values so we can verify them
   * on receive.
   */
  FramePtr = (u8 *)(&TxFrame[2]);
  for (Index = 0; Index < tx_frame->can_dlc; Index++)
  {
    *FramePtr++ = tx_frame->data[Index];
  }

  // xil_printf("send can id:%x\r\n", TxFrame[0]);
  // xil_printf("send can dlc:%x\r\n", TxFrame[1]);
  /*
   * Now wait until the TX FIFO is not full and send the frame.
   */
  while (XCanPs_IsTxFifoFull(&CanInstance) == TRUE)
  {
    xil_printf("\rTXFIFO full!\n");
  }

  Status = XCanPs_Send(&CanInstance, TxFrame);
  // if (Status != XST_SUCCESS) {
  //   /*
  //    * The frame could not be sent successfully.
  //    */
  //   LoopbackError = TRUE;
  //   // SendDone = TRUE;
  //   // RecvDone = TRUE;
  // }
  return Status;
}

/*****************************************************************************/
/**
 *
 * Callback function (called from interrupt handler) to handle confirmation of
 * transmit events when in interrupt mode.
 *
 * @param	CallBackRef is the callback reference passed from the interrupt
 *		handler, which in our case is a pointer to the driver instance.
 *
 * @return	None.
 *
 * @note		This function is called by the driver within interrupt
 *context.
 *
 ******************************************************************************/
static void SendHandler(void *CallBackRef)
{
  // xil_printf("\r can send isr!\n");
  // BaseType_t xHigherPriorityTaskWoken;

  // /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
  //   *it will get set to pdTRUE inside the interrupt safe API function if a
  //   *context switch is required.
  //   */
  // xHigherPriorityTaskWoken = pdFALSE;

  // /* 'Give' the semaphore to unblock the task, passing in the address of
  //  *xHigherPriorityTaskWoken as the interrupt safe API function's
  //  *pxHigherPriorityTaskWoken parameter. */
  // xSemaphoreGiveFromISR(xCANMutex, &xHigherPriorityTaskWoken);

  // /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR(). If
  //  *xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
  //  *then calling portYIELD_FROM_ISR() will request a context switch. If
  //  *xHigherPriorityTaskWoken is still pdFALSE then calling
  //  *portYIELD_FROM_ISR() will have no effect. Unlike most FreeRTOS ports, the
  //  *Windows port requires the ISR to return a value - the return statement
  //  *is inside the Windows version of portYIELD_FROM_ISR(). */
  // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*****************************************************************************/
/**
 *
 * Callback function (called from interrupt handler) to handle frames received
 *in interrupt mode.  This function is called once per frame received. The
 *driver's receive function is called to read the frame from RX FIFO.
 *
 * @param	CallBackRef is the callback reference passed from the interrupt
 *		handler, which in our case is a pointer to the device instance.
 *
 * @return	None.
 *
 * @note		This function is called by the driver within interrupt
 *context.
 *
 ******************************************************************************/
static void RecvHandler(void *CallBackRef)
{
  XCanPs *CanPtr = (XCanPs *)CallBackRef;
  int Status;
  int Index;
  u8 *FramePtr;
  struct can_frame *recv_frame;
  u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

  BaseType_t xHigherPriorityTaskWoken;

  /* As always, xHigherPriorityTaskWoken is initialized to pdFALSE to be able to
   *detect it getting set to pdTRUE inside an interrupt safe API function. Note
   *that as an interrupt safe API function can only set xHigherPriorityTaskWoken
   *to pdTRUE, it is safe to use the same xHigherPriorityTaskWoken variable in
   *both the call to xQueueReceiveFromISR() and the call to
   *xQueueSendToBackFromISR().
   */

  xHigherPriorityTaskWoken = pdFALSE;

  Status = XCanPs_Recv(CanPtr, RxFrame);
  if (Status != XST_SUCCESS)
  {
    xil_printf("receive error!\r\n");
  }
  else
  {
    xil_printf("\rreceive ISR!\r\n");
  }

  // get the can id
  recv_frame->can_id = (u32)(((RxFrame[0] >> 9) >> 8) >> 4);

  // get the can dlc
  recv_frame->can_dlc = (u32)((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) >> 28);

  FramePtr = (u8 *)(&RxFrame[2]);
  for (Index = 0; Index < recv_frame->can_dlc; Index++)
  {
    recv_frame->data[Index] = *FramePtr++;
  }

  // xil_printf("Receive can id:%lx\r\n", recv_frame->can_id);
  // xil_printf("Receive can dlc:%lx\r\n", recv_frame->can_dlc);
  // for (int i = 0; i < recv_frame->can_dlc; i++)
  // {
  //   xil_printf("can data[%d]:%lx\r\n", i, recv_frame->data[i]);
  // }

  // read motor parameters
  nmt.ParaRead(recv_frame);

  // send to can frame unpack task
  // xQueueSendToBackFromISR(xCANQueue, recv_frame, &xHigherPriorityTaskWoken);

  /*
   * Verify Identifier and Data Length Code.
   */
  //   if (RxFrame[0] !=
  //       (u32)XCanPs_CreateIdValue((u32)TEST_MESSAGE_ID, 0, 0, 0, 0)) {
  //     LoopbackError = TRUE;
  //     RecvDone = TRUE;
  //     return;
  //   }

  //   if ((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) != TxFrame[1]) {
  //     LoopbackError = TRUE;
  //     RecvDone = TRUE;
  //     return;
  //   }

  //   /*
  //    * Verify the Data field contents.
  //    */
  //   FramePtr = (u8 *)(&RxFrame[2]);
  //   for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
  //     if (*FramePtr++ != (u8)Index) {
  //       LoopbackError = TRUE;
  //       break;
  //     }
  //   }

  // RecvDone = TRUE;
}

/*****************************************************************************/
/**
 *
 * Callback function (called from interrupt handler) to handle error interrupt.
 * Error code read from Error Status register is passed into this function.
 *
 * @param	CallBackRef is the callback reference passed from the interrupt
 *		handler, which in our case is a pointer to the driver instance.
 * @param	ErrorMask is a bit mask indicating the cause of the error.
 *		Its value equals 'OR'ing one or more XCANPS_ESR_* defined in
 *		xcanps_hw.h.
 *
 * @return	None.
 *
 * @note		This function is called by the driver within interrupt
 *context.
 *
 ******************************************************************************/
static void ErrorHandler(void *CallBackRef, u32 ErrorMask)
{

  if (ErrorMask & XCANPS_ESR_ACKER_MASK)
  {
    /*
     * ACK Error handling code should be put here.
     */
  }

  if (ErrorMask & XCANPS_ESR_BERR_MASK)
  {
    /*
     * Bit Error handling code should be put here.
     */
  }

  if (ErrorMask & XCANPS_ESR_STER_MASK)
  {
    /*
     * Stuff Error handling code should be put here.
     */
  }

  if (ErrorMask & XCANPS_ESR_FMER_MASK)
  {
    /*
     * Form Error handling code should be put here.
     */
  }

  if (ErrorMask & XCANPS_ESR_CRCER_MASK)
  {
    /*
     * CRC Error handling code should be put here.
     */
  }

  /*
   * Set the shared variables.
   */
  LoopbackError = TRUE;
  RecvDone = TRUE;
  SendDone = TRUE;
}

/*****************************************************************************/
/**
 *
 * Callback function (called from interrupt handler) to handle the following
 * interrupts:
 *   - XCANPS_IXR_BSOFF_MASK:	Bus Off Interrupt
 *   - XCANPS_IXR_RXOFLW_MASK:	RX FIFO Overflow Interrupt
 *   - XCANPS_IXR_RXUFLW_MASK:	RX FIFO Underflow Interrupt
 *   - XCANPS_IXR_TXBFLL_MASK:	TX High Priority Buffer Full Interrupt
 *   - XCANPS_IXR_TXFLL_MASK:	TX FIFO Full Interrupt
 *   - XCANPS_IXR_WKUP_MASK:	Wake up Interrupt
 *   - XCANPS_IXR_SLP_MASK:	Sleep Interrupt
 *   - XCANPS_IXR_ARBLST_MASK:	Arbitration Lost Interrupt
 *
 *
 * @param	CallBackRef is the callback reference passed from the
 *		interrupt Handler, which in our case is a pointer to the
 *		driver instance.
 * @param	IntrMask is a bit mask indicating pending interrupts.
 *		Its value equals 'OR'ing one or more of the XCANPS_IXR_*_MASK
 *		value(s) mentioned above.
 *
 * @return	None.
 *
 * @note		This function is called by the driver within interrupt
 *context. This function should be changed to meet specific application needs.
 *
 ******************************************************************************/
static void EventHandler(void *CallBackRef, u32 IntrMask)
{
  XCanPs *CanPtr = (XCanPs *)CallBackRef;

  if (IntrMask & XCANPS_IXR_BSOFF_MASK)
  {
    /*
     * Entering Bus off status interrupt requires
     * the CAN device be reset and reconfigured.
     */
    XCanPs_Reset(CanPtr);
    Config(CanPtr);
    return;
  }

  if (IntrMask & XCANPS_IXR_RXOFLW_MASK)
  {
    /*
     * Code to handle RX FIFO Overflow Interrupt should be put here.
     */
  }

  if (IntrMask & XCANPS_IXR_RXUFLW_MASK)
  {
    /*
     * Code to handle RX FIFO Underflow Interrupt
     * should be put here.
     */
  }

  if (IntrMask & XCANPS_IXR_TXBFLL_MASK)
  {
    /*
     * Code to handle TX High Priority Buffer Full
     * Interrupt should be put here.
     */
  }

  if (IntrMask & XCANPS_IXR_TXFLL_MASK)
  {
    /*
     * Code to handle TX FIFO Full Interrupt should be put here.
     */
    // xil_printf("\rTXFIFO full!\n");
  }

  if (IntrMask & XCANPS_IXR_WKUP_MASK)
  {
    /*
     * Code to handle Wake up from sleep mode Interrupt
     * should be put here.
     */
  }

  if (IntrMask & XCANPS_IXR_SLP_MASK)
  {
    /*
     * Code to handle Enter sleep mode Interrupt should be put here.
     */
  }

  if (IntrMask & XCANPS_IXR_ARBLST_MASK)
  {
    /*
     * Code to handle Lost bus arbitration Interrupt
     * should be put here.
     */
  }
}

/*****************************************************************************/
/**
 *
 * This function sets up the interrupt system so interrupts can occur for the
 * CAN. This function is application-specific since the actual system may or
 * may not have an interrupt controller. The CAN could be directly connected
 * to a processor without an interrupt controller. The user should modify this
 * function to fit the application.
 *
 * @param	IntcInstancePtr is a pointer to the instance of the ScuGic.
 * @param	CanInstancePtr contains a pointer to the instance of the CAN
 *		which is going to be connected to the interrupt
 *		controller.
 * @param	CanIntrId is the interrupt Id and is typically
 *		XPAR_<CANPS_instance>_INTR value from xparameters.h.
 *
 * @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note		None.
 *
 ****************************************************************************/
static int SetupInterruptSystem(INTC *IntcInstancePtr, XCanPs *CanInstancePtr,
                                u16 CanIntrId)
{
  int Status = 0;
#ifdef XPAR_INTC_0_DEVICE_ID
#ifndef TESTAPP_GEN
  /* Initialize the interrupt controller and connect the ISRs */
  Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
  if (Status != XST_SUCCESS)
  {
    xil_printf("Failed init intc\r\n");
    return XST_FAILURE;
  }
#endif
  /*
   * Connect the driver interrupt handler
   */
  Status = XIntc_Connect(IntcInstancePtr, CanIntrId,
                         (XInterruptHandler)XCanPs_IntrHandler, CanInstancePtr);
  if (Status != XST_SUCCESS)
  {
    xil_printf("Failed connect intc\r\n");
    return XST_FAILURE;
  }
#ifndef TESTAPP_GEN
  /*
   * Start the interrupt controller such that interrupts are enabled for
   * all devices that cause interrupts.
   */
  Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
  if (Status != XST_SUCCESS)
  {
    return XST_FAILURE;
  }
#endif

  /*
   * Enable the interrupt for the CAN device.
   */
  XIntc_Enable(IntcInstancePtr, CanIntrId);
#ifndef TESTAPP_GEN
  Xil_ExceptionInit();
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                               (Xil_ExceptionHandler)XIntc_InterruptHandler,
                               (void *)IntcInstancePtr);
#endif
#else
#ifndef TESTAPP_GEN
  // XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

  //  Xil_ExceptionInit();

  // /*
  //  * Initialize the interrupt controller driver so that it is ready to
  //  * use.
  //  */
  // IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
  // if (NULL == IntcConfig) {
  //   return XST_FAILURE;
  // }

  // Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
  //                                IntcConfig->CpuBaseAddress);
  // if (Status != XST_SUCCESS) {
  //   return XST_FAILURE;
  // }

  // /*
  //  * Connect the interrupt controller interrupt handler to the hardware
  //  * interrupt handling logic in the processor.
  //  */
  // Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
  //                              (Xil_ExceptionHandler)XScuGic_InterruptHandler,
  //                              IntcInstancePtr);
#endif

  /*
   * Connect the device driver handler that will be called when an
   * interrupt for the device occurs, the handler defined above performs
   * the specific interrupt processing for the device.
   */
  Status = XScuGic_Connect(IntcInstancePtr, CanIntrId,
                           (Xil_InterruptHandler)XCanPs_IntrHandler,
                           (void *)CanInstancePtr);
  if (Status != XST_SUCCESS)
  {
    return Status;
  }

  // enable Nested Interrupts
  //    XScuGic_CPUWriteReg(IntcInstancePtr, XSCGIC_BIN_PT_OFFSET, 0x03);

  /*
   * Enable the interrupt for the CAN device.
   */
  XScuGic_EnableIntr(IntcInstancePtr, CanIntrId);
#endif
#ifndef TESTAPP_GEN
  /*
   * Enable interrupts in the Processor.
   */
//  Xil_ExceptionEnable();
#endif
  return XST_SUCCESS;
}
