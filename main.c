#include "openamp/inter_core_com.h"
#include "xil_printf.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "canopen/can.h"
#include "fan/fan.h"
#include "sysmon/sysmon.h"
#include "controller/controller.h"
#include "common/common.h"
#include "sleep.h"

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* extern parameters */
extern struct robot ccr4;
extern struct nmt nmt;

// the command send to RPU from APU
extern r5_cmd R5_cmd;
extern r5_state R5_state;

/* initial task priority*/
// init the openamp framework first
#define OPENAMP_TASK_PRIORITY 6
TaskHandle_t comm_task;

#define INIT_TASK_PRIORITY 5
static TaskHandle_t SystemInitTaskHandle;

// system monitor task handle
static TaskHandle_t system_monitor_task;
#define SYSMON_TASK_PRIORITY 4

// controllers task
#define CONTROLLERS_TASK_PRIORITY 3
static TaskHandle_t controllers_task;

/* global variables */
// mutex
SemaphoreHandle_t xCANMutex;

// queue
QueueHandle_t xCANQueue = NULL;

static void SysMonTask(void *pvParameters)
{
	//	init sysmon data type
	sysmon sysmon = {
		.fan_refresh_cnt = 0};

	for (;;)
	{
		SysMonPoll(&sysmon);
		// 1s
		vTaskDelay(1000);
	}
}

/* System Init task */
static void SystemInitTask(void *pvParameters)
{
	s8 state;

	// set the core id
	R5_state.r5_id = 0;

	// init system monitor
	SysMonInit();

	// OpenAMP init

	// init fan
	FanInit();

	// init nmt
	state = nmt.Init();

	//	stop nmt
	nmt.Stop(0);

	// start nmt
	nmt.Start();

	if (state != 0)
	{
		RPU_PRINTF("R5-0 init fialed!\n");
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

	RPU_PRINTF("Starting application...\n");
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
	BaseType_t state;

	RPU_PRINTF("------------------------------\n");
	RPU_PRINTF("R5-0 CORE START!\n");

	/* Before a semaphore is used it must be explicitly created.In this example amutex type semaphore is created. */
	xCANMutex = xSemaphoreCreateMutex();

	/* Before a queue can be used it must first be created.The address of the data is transfered between the isr and queue*/
	xCANQueue = xQueueCreate(10, sizeof(struct can_frame));

	/* Create the openamp task */
	// state = xTaskCreate(processing, (const char *)"OpenAMP", 1024, NULL, OPENAMP_TASK_PRIORITY, &comm_task);

	/* Create the system init task */
	state = xTaskCreate(SystemInitTask, (const char *)"Init", 1024, NULL, INIT_TASK_PRIORITY, &SystemInitTaskHandle);

	/* Create the system monitor task */
	  state = xTaskCreate(SysMonTask, (const char *)"System Monitor", 1024, NULL, SYSMON_TASK_PRIORITY, &system_monitor_task);

	/* Create the pulley1 controller task */
	  state = xTaskCreate(ControllersPoll, (const char *)"Controllers", 1024, NULL, CONTROLLERS_TASK_PRIORITY, &controllers_task);

	if (state != pdPASS)
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
