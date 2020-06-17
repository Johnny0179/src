#include "controller/maxon/maxon.h"
#include "canopen/nmt.h"

// the nmt device
extern struct nmt nmt;

// Set EPOS control word
static s8 MaxonSetCtrlWrd(const struct nmt *nmt, u16 ctrl_wrd, struct maxon *motor)
{
    return nmt->TxPdo1(motor->parameter->motor_id_, ctrl_wrd);
}

// disable motor
static s8 MaxonDisable(const struct nmt *nmt, struct maxon *motor)
{
    // check if disabled
    if (motor->parameter->status_word_ != 0x0240)
    {
        MaxonSetCtrlWrd(nmt, 0x0000, motor);
        // delay_ms(DelayEpos);
        // wait disable cmd success
        while (motor->parameter->status_word_ != 0x0240)
        {
            // delay_ms(DelayEpos);
        }
    }

    return CfgSuccess;
}

// enable motor
static s8 MaxonEnable(const struct nmt *nmt, struct maxon *motor)
{
    // check if already enabled?
    if ((motor->parameter->status_word_ & 0x00ff) != 0x37)
    {
        // RPU_PRINTF("enable motor: %d\n", motor->parameter->motor_id_);
        MaxonSetCtrlWrd(nmt, 0x0006, motor);

        // bit5: quick stop
        while ((motor->parameter->status_word_ >> 5 & 1) != 1)
        {
            // RPU_PRINTF("motor status: %x\n", motor->parameter->status_word_);
        }

        MaxonSetCtrlWrd(nmt, 0x000F, motor);

        // wait enable cmd success
        while ((motor->parameter->status_word_ & 0x00ff) != 0x37)
        {
        }
    }

    return CfgSuccess;
}

//set absolute pos
static s8 MaxonAbsPos(const struct nmt *nmt, struct maxon *motor, s32 abs_pos)
{
    //	save init pos
    s32 init_pos = motor->parameter->pos_pv_;
    RPU_PRINTF("set absolute pos:%d\n", abs_pos);
    nmt->TxPdo2(motor->parameter->motor_id_, ServAbsPosSet, abs_pos, PPM);

    //check if Setpoint acknowledged
    while ((motor->parameter->status_word_ >> 12 & 1) != 1)
    {
        delay_ms(2);
        //check if mode set
        if (motor->parameter->mode_display_ != PPM)
        {
            nmt->TxPdo2(motor->parameter->motor_id_, ServAbsPosSet, abs_pos, PPM);
        }
        // start configuration
        MaxonSetCtrlWrd(nmt, 0x000F, motor);
        delay_ms(2);
    }

    // wait motor start rotating
    while (abs(motor->parameter->pos_pv_ - init_pos) < 1000)
    {
        RPU_PRINTF("current trq:%d\n", motor->parameter->actual_average_torque_);
    }

    // config success
    return CfgSuccess;
}

//set relative pos
static s8 MaxonRelPos(const struct nmt *nmt, struct maxon *motor, s32 rel_pos)
{
    //	save init pos
    s32 init_pos = motor->parameter->pos_pv_;
    RPU_PRINTF("set relative pos:%d\n", rel_pos);

    // not lock the pos
    if (rel_pos != 0)
    {
        nmt->TxPdo2(motor->parameter->motor_id_, ServRelPosSet, rel_pos, PPM);
        delay_ms(2);
        //wait Setpoint acknowledged
        while (motor->parameter->mode_display_ != PPM || abs(motor->parameter->pos_pv_ - init_pos) < 500)
        {

            nmt->TxPdo2(motor->parameter->motor_id_, ServRelPosSet, rel_pos, PPM);
            // start configuration
            MaxonSetCtrlWrd(nmt, 0x000F, motor);
            delay_ms(2);
        }

        //    wait target reach, 500 inc error
        while (abs(motor->parameter->pos_pv_ - init_pos) < abs(abs(rel_pos) - 500))
        {
            // RPU_PRINTF("abs(motor->parameter->pos_pv_ - rel_pos):%d\n", abs(motor->parameter->pos_pv_ - rel_pos));
        }
    }
    else
    {
        nmt->TxPdo2(motor->parameter->motor_id_, ServRelPosSet, rel_pos, PPM);
        delay_ms(2);

        // wait mode change success
        while (motor->parameter->mode_display_ != PPM)
        {
            nmt->TxPdo2(motor->parameter->motor_id_, ServRelPosSet, rel_pos, PPM);
            delay_ms(2);
        }
    }

    // config success
    return CfgSuccess;
}

// set target torque
static s8 MaxonTorque(const struct nmt *nmt, struct maxon *motor, s16 torque)
{
    RPU_PRINTF("set motor[%d] torque\n", motor->parameter->motor_id_);

    nmt->TxPdo3(motor->parameter->motor_id_, torque, CST);
    delay_ms(2);
    // wait the toruqe reach the target, 1% error
    while (abs(motor->parameter->actual_average_torque_ - torque) > 10)
    {

        nmt->TxPdo3(motor->parameter->motor_id_, torque, CST);
        delay_ms(2);
    }

    // config success
    return CfgSuccess;
}

/****************define the motors************************/
// r5 state
r5_state R5_state = {
    .upclaw_parameter.motor_id_ = 1,
    .upwheel_parameter.motor_id_ = 2,
    .pulley1_parameter.motor_id_ = 3,
    .pulley2_parameter.motor_id_ = 4,
    .downclaw1_parameter.motor_id_ = 5,
    .downclaw2_parameter.motor_id_ = 6};

// maxon motros
struct maxon up_claw =
    {
        .parameter = &R5_state.upclaw_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};

struct maxon up_wheel =
    {
        .parameter = &R5_state.upwheel_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};

struct maxon pulley1 =
    {
        .parameter = &R5_state.pulley1_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};

struct maxon pulley2 =
    {
        .parameter = &R5_state.pulley2_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};

struct maxon down_claw1 =
    {
        .parameter = &R5_state.downclaw1_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};

struct maxon down_claw2 =
    {
        .parameter = &R5_state.downclaw2_parameter,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable,
        .AbsPos = MaxonAbsPos,
        .RelPos = MaxonRelPos,
        .Torque = MaxonTorque};
