#include "controller/maxon/maxon.h"
#include "canopen/nmt.h"

// the nmt device
extern struct nmt nmt;

static s8 MaxonSetCtrlWrd(struct nmt *nmt, u16 ctrl_wrd, struct maxon *motor)
{
  return nmt->TxPdo1(motor->parameter.motor_id_, ctrl_wrd);
}

// disable motor
static s8 MaxonDisable(struct nmt *nmt, struct maxon *motor)
{
  MaxonSetCtrlWrd(nmt, 0x0000, motor);
  delay_ms(DelayEpos);
  // wait disable cmd success
  while (motor->parameter.status_word_ != 0x0240)
  {
    delay_ms(DelayEpos);
  }

  return CfgSuccess;
}

// enable motor
static s8 MaxonEnable(struct nmt *nmt, struct maxon *motor)
{
  //  printf("motor id: %d\n", motor->parameter.status_word_);
  MaxonSetCtrlWrd(nmt, 0x0006, motor);
  delay_ms(DelayEpos);
  // bit5: quick stop
  while ((motor->parameter.status_word_ >> 5 & 1) != 1)
  {
  }

  MaxonSetCtrlWrd(nmt, 0x000F, motor);
  delay_ms(DelayEpos);
  // wait enable cmd success
  while ((motor->parameter.status_word_ & 0x00ff) != 0x37)
  {
  }
  return CfgSuccess;
}

/****************define the motors************************/
struct maxon up_claw =
    {
        .parameter.motor_id_ = UP_CLAW,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable};

struct maxon up_wheel =
    {
        .parameter.motor_id_ = UP_WHEEL,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable
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
        .Disable = MaxonDisable,
        .Enable = MaxonEnable
        };

struct maxon down_claw1 =
    {
        .parameter.motor_id_ = DOWN_CLAW1,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable
        };

struct maxon down_claw2 =
    {
        .parameter.motor_id_ = DOWN_CLAW2,
        .Disable = MaxonDisable,
        .Enable = MaxonEnable
        };