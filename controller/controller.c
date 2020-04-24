
#include "controller/controller.h"

// rpmsg to  write robot cmd and read robot state
// struct robot_para ccr4;
#define CMD_STATE_SIZE 248

// define the robot
struct robot ccr4 =
	{
		.Init = RobotInit};

// RobotInit
int RobotInit(void)
{
	// nmt init
	//	nmt.Init ();
	//	nmt.Start ();
	//	return ROBOT_SUCCESS;
}
