#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "controller/maxon/maxon.h"

/* rpmsg definition */
// max rpmsg size is 496 bytes
#define MAX_RPMSG_SIZE 496

// motor number
#define MOTOR_NUM 6

// Write robot cmd
#define ROBOT_RD_WR 0

// Read robot state
#define ROBOT_STATE 1

// motor id
#define UP_CLAW 1
#define UP_WHEEL 2
#define PULLEY1 3
#define PULLEY2 4
#define DOWN_CLAW1 5
#define DOWN_CLAW2 6

// flag
#define ROBOT_SUCCESS 0
#define ROBOT_FAIL -1

// robot init
int RobotInit(void);

// robot command struct
struct robot_cmd
{
  /* data */
};

// robot state struct
struct robot_state
{
  /* data */
  u16 ps_core_temp;
  u16 pl_core_temp;
};

// robot struct
struct robot
{
  u16 cmd[124];
  u16 state[124];
  int (*Init)(void);
};

// controller struct
struct controller
{

  // controller reference command
  u16 ref;

  // maxon motor
//  struct maxon maxon;
};

#endif
