
#include "controller/controller.h"

// rpmsg to  write robot cmd and read robot state
// struct robot_para ccr4;
#define CMD_STATE_SIZE 248

// the nmt device
extern struct nmt nmt;

// define the motors
struct maxon up_claw =
{
	.parameter.motor_id_ = UP_CLAW, 
	// .Disable = MaxonDisable, 
	// .Enable = MaxonEnable
};


struct maxon up_wheel =
{
	.parameter.motor_id_ = UP_WHEEL, 
	// .Disable = MaxonDisable, 
	// .Enable = MaxonEnable
};


struct maxon pulley1 =
{
	.parameter.motor_id_ = PULLEY1, 
	.Disable = MaxonDisable, 
	.Enable = MaxonEnable
};


struct maxon pulley2 =
{
	.parameter.motor_id_ = PULLEY2, 
	// .Disable = MaxonDisable, 
	// .Enable = MaxonEnable
};


struct maxon down_claw1 =
{
	.parameter.motor_id_ = DOWN_CLAW1, 
	// .Disable = MaxonDisable, 
	// .Enable = MaxonEnable
};


struct maxon down_claw2 =
{
	.parameter.motor_id_ = DOWN_CLAW2, 
	// .Disable = MaxonDisable, 
	// .Enable = MaxonEnable
};

// define the robot
struct robot ccr4 =
{
	.Init = RobotInit
};



// RobotInit
int RobotInit (void)
{
	// nmt init
	nmt.Init ();
	nmt.Start ();
	return ROBOT_SUCCESS;
}


