#include "canopen/can.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "common/common.h"
extern SemaphoreHandle_t xCANMutex;

// init ps can0
int CANInit(void) { return PsCan0Init(); }

int Send(const struct can_frame *tx_frame)
{
  int status;
  /* The mutex is created before the scheduler is started, so already exists by
  the time this task executes.

  Attempt to take the mutex, blocking indefinitely to
  wait for the mutex if it is not available straight away. The call to
  xSemaphoreTake() will only return when the mutex has been successfully obtained,
  so there is no need to check the function return value. If any other delay
  period was used then the code must check that xSemaphoreTake() returns pdTRUE
  before accessing the shared resource (which in this case is standard out). As
  noted earlier in this book, indefinite time outs are not recommended for
  production code. */
  xSemaphoreTake(xCANMutex, portMAX_DELAY);

  status = SendFrame(tx_frame);

  if (status == 0)
  {
    // RPU_PRINTF("can send success!\n");
    // give back the mutex
    xSemaphoreGive(xCANMutex);
    return status;
  }
  else
  {
    // RPU_PRINTF("can send error!\n");
    // give back the mutex
    xSemaphoreGive(xCANMutex);

    return -1;
  }
}

//
struct can ps_can0 = {
    .init = PsCan0Init,
    .send = Send
    };
