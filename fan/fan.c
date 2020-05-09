#include "../../src/fan/fan.h"

#include <stdio.h>
//#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xparameters.h"

#include "xgpiops.h"
#include "common/common.h"



static XGpioPs 				lGpioInstance;
static XGpioPs_Config * lpConfig;

void FanInit ()
{
	//gpio0
	lpConfig						= XGpioPs_LookupConfig (XPAR_XGPIOPS_0_DEVICE_ID);

	if (!lpConfig)
		{
		RPU_PRINTF ("can't find this gpio\n");

		}else{
			RPU_PRINTF("fan init!\n");
		}

	XGpioPs_CfgInitialize (&lGpioInstance, lpConfig, lpConfig->BaseAddr);

	// fan gpio output
	XGpioPs_SetDirectionPin (&lGpioInstance, FAN_GPIO, 1); 
	XGpioPs_SetOutputEnablePin (&lGpioInstance, FAN_GPIO, 1); 
	XGpioPs_WritePin (&lGpioInstance, FAN_GPIO, FAN_OFF);
}


void FanOn ()
{
	XGpioPs_WritePin (&lGpioInstance, FAN_GPIO, FAN_ON);

}


void FanOff ()
{
	XGpioPs_WritePin (&lGpioInstance, FAN_GPIO, FAN_OFF);

}



