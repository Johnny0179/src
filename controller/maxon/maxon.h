#ifndef MAXON_H
#define MAXON_H
#include "canopen/nmt.h"
#include "common/common.h"

#include "controller/controller.h"

// delay_time wait for epos 1ms
#define DelayEpos 1

// Config parameters
#define CfgSuccess 0
#define CfgFail -1

struct maxon_type
{
	/* data */
	u16 motor_id_;
	u16 mode_display_;
	u16 serv_error_;
	s16 status_word_;
	s32 pos_pv_;
	s32 SpdSV_;
	s32 speed_pv_;
	s32 actual_average_vel_;
	s16 torque_pv_;
	s16 actual_average_torque_;
};

// maxon motor type
struct maxon
{
	struct maxon_type parameter;

	// struct motor_ops ops;
	s8 (*Disable)(struct nmt *nmt, struct maxon *motor);
	s8 (*Enable)(struct nmt *nmt, struct maxon *motor);
};

// set abs_pos
// set relative pos
// set trq
#endif
