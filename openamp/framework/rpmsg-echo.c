/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include "xil_printf.h"
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include <openamp/framework/platform_info.h>
#include <openamp/framework/rpmsg-echo.h>
#include <openamp/framework/rsc_table.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "openamp/inter_core_com.h"
#include "common/common.h"
#include "sysmon/sysmon.h"

static int shutdown_req = 0;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
							 uint32_t src, void *priv)
{
	(void)priv;
	(void)src;

	int state;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG)
	{
		LPRINTF("shutdown message is received.\n");
		shutdown_req = 1;
		return RPMSG_SUCCESS;
	}

	// //	internel core communication
	// state = InterCoreCom(ept, len, data);

	// /* Send data back to master */
	// if (state < 0)
	// {
	// 	LPERROR("rpmsg process failed\n");
	// }

	/* Send data back to master */
	if (rpmsg_send(ept, data, len) < 0)
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
	struct rpmsg_endpoint lept;

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

/*-----------------------------------------------------------------------------*
 *  Processing Task
 *-----------------------------------------------------------------------------*/
void OpenAMPInit()
{
}