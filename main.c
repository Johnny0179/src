#include <metal/alloc.h>
#include <openamp/open_amp.h>
#include "xil_printf.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "openamp/rsc_table.h"
#include "canopen/can.h"
#include "fan/fan.h"
#include "sysmon/sysmon.h"
#include "openamp/platform_info.h"
#include "openamp/rpmsg-echo.h"
#include "controller/controller.h"
#include "common/common.h"
#include "sleep.h"

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* extern parameters */
extern struct robot ccr4;
extern struct nmt nmt;

extern struct maxon pulley1;

/* task handles */
static TaskHandle_t comm_task;
static TaskHandle_t fan_task;
static TaskHandle_t SystemInitTaskHandle;
static TaskHandle_t CANUnpackTaskHandle;

// system monitor task handle
static TaskHandle_t system_monitor_task;

//

// task priority
#define INIT_TASK_PRIORITY 8
#define FAN_TASK_PRIORITY 6
#define CAN_TASK_PRIORITY 3
#define OPENAMP_TASK_PRIORITY 3

/* global variables */
// mutex
SemaphoreHandle_t xCANMutex;

// queue
QueueHandle_t xCANQueue = NULL;

/* Fan task */
static void FanTask(void *pvParameters)
{
	// the temperature of ps and pl
	u16 ps_core_temp, pl_core_temp;
	s8 state;
	nmt.Stop(0);
	delay_ms(1000);
	nmt.Start();
	delay_ms(1000);


	pulley1.Enable(&nmt, &pulley1);
	// delay_ms(1);
	pulley1.Disable(&nmt, &pulley1);

	nmt.Stop(0);
	delay_ms(1000);

	for (;;)
	{
		// nmt.TxPdo1(3, 0x0006);
		// delay_ms(100);
		// nmt.TxPdo1(3, 0x000F);
		// delay_ms(100);

		// get the temperature of the ps and pl core
		ps_core_temp = GetPsCoreTemp();
		pl_core_temp = GetPlCoreTemp();

		ccr4.state[0] = ps_core_temp;
		ccr4.state[1] = pl_core_temp;

		xil_printf("\r\nps core temp:%d\n", ccr4.state[0]);
		xil_printf("\rpl core temp:%d\n", ccr4.state[1]);

		// turn on the fan if the temperature > 40
		if (ps_core_temp >= 40 || pl_core_temp >= 40)
		{
			FanOn();
		}
		else
		{
			FanOff();
		}

		// delay 5s
		vTaskDelay(5000);
	}
}

/* System Init task */
static void SystemInitTask(void *pvParameters)
{
	s8 state;
	// init fan
	FanInit();

	// init system monitor
	SysMonInit();

	// init nmt
	state = nmt.Init();

	if (state == 0)
	{
		RPU_PRINTF("nmt init success!\n");
	}

	/* Terminate this task */
	vTaskDelete(SystemInitTaskHandle);
}

/*CAN unpack task*/
static void CANUnpackTask(void *pvParameters)
{
	// xil_printf("CAN unpack task!\n");
	struct can_frame *recv_frame;

	for (;;)
	{
		/* Block on the queue to wait for data to arrive. */
		xQueueReceive(xCANQueue, recv_frame, portMAX_DELAY);

		RPU_PRINTF("can upack task!\n");

		RPU_PRINTF("can id:%lx\r\n", recv_frame->can_id);
		RPU_PRINTF("can dlc:%lx\r\n", recv_frame->can_dlc);

		for (int i = 0; i < recv_frame->can_dlc; i++)
		{
			RPU_PRINTF("can data[%d]:%lx\r\n", i, recv_frame->data[i]);
		}

		// read the rxpdo
		nmt.ParaRead(recv_frame);
	}
}

/*-----------------------------------------------------------------------------*
 *  Processing Task
 *-----------------------------------------------------------------------------*/
static void processing(void *unused_arg)
{
	void *platform;
	struct rpmsg_device *rpdev;

	LPRINTF("Starting application...\n");
	/* Initialize platform */
	if (platform_init(NULL, NULL, &platform))
	{
		LPERROR("Failed to initialize platform.\n");
	}
	else
	{
		rpdev = platform_create_rpmsg_vdev(platform, 0,
										   VIRTIO_DEV_SLAVE,
										   NULL, NULL);
		if (!rpdev)
		{
			LPERROR("Failed to create rpmsg virtio device.\n");
		}
		else
		{
			app(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev);
		}
	}

	LPRINTF("Stopping application...\n");
	platform_cleanup(platform);

	/* Terminate this task */
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(void)
{
	BaseType_t stat;

	/* Before a semaphore is used it must be explicitly created.In this example amutex type semaphore is created. */
	xCANMutex = xSemaphoreCreateMutex();

	/* Before a queue can be used it must first be created.The address of the data is transfered between the isr and queue*/
	xCANQueue = xQueueCreate(10, sizeof(struct can_frame));

	/* Create the system init task */
	stat = xTaskCreate(SystemInitTask, (const char *)"Init", 1024, NULL, INIT_TASK_PRIORITY, &SystemInitTaskHandle);

	/* Create the openamp task */
	stat = xTaskCreate(processing, (const char *)"OpenAMP", 1024, NULL, OPENAMP_TASK_PRIORITY, &comm_task);

	/* Create the fan task */
	stat = xTaskCreate(FanTask, (const char *)"Fan", configMINIMAL_STACK_SIZE, NULL, FAN_TASK_PRIORITY, &fan_task);

	// /* Create the CAN frame unpack task */
	// xTaskCreate(CANUnpackTask,			   /* The function that implements the task. */
	// 			(const char *)"CANUnpack", /* Text name for the task, provided to assist debugging only. */
	// 			1024,					   /* The stack allocated to the task.*/
	// 			NULL,					   /* The task parameter is not used, so set to NULL. */
	// 			CAN_TASK_PRIORITY,		   /* The task  priority. */
	// 			&CANUnpackTaskHandle);

	if (stat != pdPASS)
	{
		LPERROR("cannot create task\n");
	}
	else
	{
		/* Start running FreeRTOS tasks */
		vTaskStartScheduler();
	}

	/* Will not get here, unless a call is made to vTaskEndScheduler() */
	while (1)
		;

	/* suppress compilation warnings*/
	return 0;
}
