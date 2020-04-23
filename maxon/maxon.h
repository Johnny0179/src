#ifndef MAXON_H
#define MAXON_H
#include "canopen/nmt.h"
#include "common/common.h"
// delay_time wait for epos 1ms
#define DelayEpos 							10

// Config parameters
#define CfgSuccess							0
#define CfgFail 								-1


struct maxon_type
{
	/* data */
	u16							motor_id_;
	u16 						mode_display_;
	u16 						serv_error_;
	s16 						status_word_;
	s32 						pos_pv_;
	s32 						SpdSV_;
	s32 						speed_pv_;
	s32 						actual_average_vel_;
	s16 						torque_pv_;
	s16 						actual_average_torque_;
};


// motor operations
// struct motor_ops {
//	 s8 (*Disable)(struct nmt *nmt, struct maxon *motor);
//	 s8 (*Enable)(struct nmt *nmt, struct maxon *motor);
// };

// maxon motor type
struct maxon
{
	struct maxon_type parameter;

	// struct motor_ops ops;
	s8 (*Disable) (struct nmt * nmt, struct maxon * motor);
	s8 (*Enable) (struct nmt * nmt, struct maxon * motor);
};


// set control word
s8 MaxonSetCtrlWrd (struct nmt * nmt, u16 ctrl_wrd, struct maxon * motor);

// disable motor
s8 MaxonDisable (struct nmt * nmt, struct maxon * motor);

// enable motor
s8 MaxonEnable (struct nmt * nmt, struct maxon * motor);

// set speed
s8 MaxonSpeed (struct nmt * nmt, struct maxon * motor, s32 speed);

// set abs_pos
// set relative pos
// set trq
#endif

