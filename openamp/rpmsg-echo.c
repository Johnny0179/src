/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include "xil_printf.h"
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "rsc_table.h"
#include "platform_info.h"
#include "rpmsg-echo.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "controller/controller.h"
static struct rpmsg_endpoint lept;
static int shutdown_req = 0;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

#include "openamp/rpmsg-echo.h"

// extern
extern struct robot ccr4;
extern struct maxon up_claw;
extern struct maxon up_wheel;
extern struct maxon pulley1;
extern struct maxon pulley2;
extern struct maxon down_claw1;
extern struct maxon down_claw2;

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
							 uint32_t src, void *priv)
{
	(void)priv;
	(void)src;
	int state, i;

	//	  u8 rpmsg_payload[MAX_RPMSG_SIZE];
	u16 *ptr = malloc(MAX_RPMSG_SIZE * sizeof(u8));

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG)
	{
		LPRINTF("shutdown message is received.\n");
		shutdown_req = 1;
		return RPMSG_SUCCESS;
	}

	/* process the data */
	//
	ptr = (u16 *)data;
	//   (*ptr)++;

	// check if is the last payload

	switch ((*ptr))
	{

		u8 temp_buff[MAX_RPMSG_SIZE] = {0};
		// write robot contrl cmd and read robot state ?
	case ROBOT_RD_WR:

		// write the cmd
		for (i = 0; i < MAX_RPMSG_SIZE / 2 / 2; i++)
		{
			ccr4.cmd[i] = *ptr;
			ptr++;
		}

		// read the state
		for (i = 0; i < MAX_RPMSG_SIZE / 2 / 2; i++)
		{
			*ptr = ccr4.state[i];
			ptr++;
		}

		// // copy the robot command
		// memcpy(&ccr4.cmd, data, MAX_RPMSG_SIZE / 2);

		// // read the state parameter
		// // copy the robot state
		// memcpy((&temp_buff + MAX_RPMSG_SIZE / 2), &ccr4.state, MAX_RPMSG_SIZE / 2);

		state = rpmsg_send(ept, data, len);
		break;

	case UP_CLAW:
		memcpy(temp_buff, &up_claw.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	case UP_WHEEL:
		memcpy(temp_buff, &up_wheel.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	case PULLEY1:
		memcpy(temp_buff, &pulley1.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	case PULLEY2:
		memcpy(temp_buff, &pulley2.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	case DOWN_CLAW1:
		memcpy(temp_buff, &down_claw1.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	case DOWN_CLAW2:
		memcpy(temp_buff, &down_claw2.parameter, MAX_RPMSG_SIZE);
		state = rpmsg_send(ept, temp_buff, len);
		break;

	default:
		break;
	}

	/* Send data back to master */
	if (state < 0)
	{
		LPERROR("rpmsg_send failed\n");
	}

	return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPRINTF("unexpected Remote endpoint destroy\n");
	shutdown_req = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct rpmsg_device *rdev, void *priv)
{
	int ret;

	/* Initialize RPMSG framework */
	LPRINTF("Try to create rpmsg endpoint.\n");

	ret = rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
						   0, RPMSG_ADDR_ANY, rpmsg_endpoint_cb,
						   rpmsg_service_unbind);
	if (ret)
	{
		LPERROR("Failed to create endpoint.\n");
		return -1;
	}

	LPRINTF("Successfully created rpmsg endpoint.\n");

	while (1)
	{

		platform_poll(priv);
		/* we got a shutdown request, exit */
		if (shutdown_req)
		{
			break;
		}
	}
	rpmsg_destroy_ept(&lept);

	return 0;
}
