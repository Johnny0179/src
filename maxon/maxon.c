#include "maxon/maxon.h"

// struct maxon maxon1 = {
//     .parameter.motor_id_ = 1, .Disable = MaxonDisable, .Enable = MaxonEnable};

s8 MaxonSetCtrlWrd(struct nmt *nmt, u16 ctrl_wrd, struct maxon *motor)
{
  return nmt->TxPdo1(motor->parameter.motor_id_, ctrl_wrd);
}

// disable motor
s8 MaxonDisable(struct nmt *nmt, struct maxon *motor)
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
s8 MaxonEnable(struct nmt *nmt, struct maxon *motor)
{
  //  printf("motor id: %d\n", motor->parameter.status_word_);
  MaxonSetCtrlWrd(nmt, 0x0006, motor);
  delay_ms(DelayEpos);
  // bit5: quick stop
  while ((motor->parameter.status_word_ >> 5 & 1) != 1)
  {
    // xil_printf("\r\wait 0x0006\n");
  }

  MaxonSetCtrlWrd(nmt, 0x000F, motor);
  delay_ms(DelayEpos);
  // wait enable cmd success
  while ((motor->parameter.status_word_ & 0x00ff) != 0x37)
  {
    // xil_printf("\r\wait 0x000F\n");
    // must delay!
    // delay_ms(DelayEpos);
  }
  return CfgSuccess;
}
