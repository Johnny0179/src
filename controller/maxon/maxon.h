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

/* mode of operation */
// position mode
#define PPM 0x01
//   speed mode
#define PVM 0x03
// CST, torque mode
#define CST 0x0A

/* motor canopen parameters */
#define ServOnPre 0x0006
#define ServOn 0x000F
#define ServOff 0x0000

#define ServAbsPosSet 0x003F
#define ServRelPosSet 0x007F
#define ServHaltBit 0x0100

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
	struct maxon_type *parameter;

	// motor control
	s8 (*Disable)(const struct nmt *nmt, struct maxon *motor);
	s8 (*Enable)(const struct nmt *nmt, struct maxon *motor);

	// set torque
	s8 (*Torque)(const struct nmt *nmt, struct maxon *motor, s16 torque);
	// set absolute position
	s8 (*AbsPos)(const struct nmt *nmt, struct maxon *motor, s32 abs_pos);

	// set relative position
	s8 (*RelPos)(const struct nmt *nmt, struct maxon *motor, s32 rel_pos);
	// set speed
};

// r5 system state structure
typedef struct r5_state_type
{
	// the rpmsg type
	u8 rpmsg_type;

	// the id of the r5 core, 0 or 1
	u8 r5_id;

	// ps core temperature
	u8 ps_core_temp;

	// pl core temperature
	u8 pl_core_temp;

	// nmt state
	u8 nmt_state;

	// motor state
	struct maxon_type upclaw_parameter;
	struct maxon_type upwheel_parameter;
	struct maxon_type pulley1_parameter;
	struct maxon_type pulley2_parameter;
	struct maxon_type downclaw1_parameter;
	struct maxon_type downclaw2_parameter;

} r5_state;

#endif
