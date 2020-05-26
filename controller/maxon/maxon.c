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
    MaxonSetCtrlWrd(nmt, 0x0000, motor);
    // delay_ms(DelayEpos);
    // wait disable cmd success
    while (motor->parameter->status_word_ != 0x0240)
    {
        // delay_ms(DelayEpos);
    }

    return CfgSuccess;
}

// enable motor
static s8 MaxonEnable(const struct nmt *nmt, struct maxon *motor)
{
    // RPU_PRINTF("enable motor: %d\n", motor->parameter->motor_id_);
    MaxonSetCtrlWrd(nmt, 0x0006, motor);
    // delay_ms(DelayEpos);
    // bit5: quick stop
    while ((motor->parameter->status_word_ >> 5 & 1) != 1)
    {
        // RPU_PRINTF("motor status: %x\n", motor->parameter->status_word_);
    }

    MaxonSetCtrlWrd(nmt, 0x000F, motor);
    // delay_ms(DelayEpos);
    // wait enable cmd success
    while ((motor->parameter->status_word_ & 0x00ff) != 0x37)
    {
    }
    return CfgSuccess;
}

/****************define the motors************************/
//motor parameters
struct maxon_type up_claw_para = {
    .motor_id_ = 1};

struct maxon_type up_wheel_para = {
    .motor_id_ = 2};

struct maxon_type pulley1_para = {
    .motor_id_ = 3};

struct maxon_type pulley2_para = {
    .motor_id_ = 4};

struct maxon_type down_claw1_para = {
    .motor_id_ = 5};

struct maxon_type down_claw2_para = {
    .motor_id_ = 6};

// maxon motros
struct maxon up_claw =
    {
        .parameter = &up_claw_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon up_wheel =
    {
        .parameter = &up_wheel_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon pulley1 =
    {
        .parameter = &pulley1_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon pulley2 =
    {
        .parameter = &pulley2_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon down_claw1 =
    {
        .parameter = &down_claw1_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon down_claw2 =
    {
        .parameter = &down_claw2_para,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};
