#include "controller/controller.h"
#include "controller/maxon/maxon.h"

//maxon motors
extern struct maxon up_claw;
extern struct maxon up_wheel;
extern struct maxon pulley1;
extern struct maxon pulley2;
extern struct maxon down_claw1;
extern struct maxon down_claw2;

//nmt device
extern struct nmt nmt;

// task handlers
extern TaskHandle_t pulleys_control_task;

// controllers parameter
r5_cmd R5_cmd = {
    // defualt control frequency is 0.1 KHz
    .control_frequency_KHz = 0.1,
    // defualt upclaw hold torque 30%
    .upclaw_hold_trq = 300,
    // defualt upclaw loose relative distance
    .upclaw_rltv_pos = -344108};

// enable controller
void EnableController(const struct maxon *motor)
{
    // check if already enabled?
    if ((motor->parameter->status_word_ & 0x00ff) != 0x37)
    {
        // enable the motor
        RPU_PRINTF("Enable controller[%d]\n", motor->parameter->motor_id_);
        motor->Enable(&nmt, motor);
    }
}

// tighten the pulleys
void TightenPulleys(void)
{

    // tighten the pulley, 12% torque
    pulley1.Torque(&nmt, &pulley1, 120);
    pulley2.Torque(&nmt, &pulley2, 120);

    //    // wait the toruqe reach the target, 1% error
    //    while (abs(pulley1.parameter->actual_average_torque_ - 1) > 10)
    //    {
    //    	RPU_PRINTF("Current torque:%d\n", pulley1.parameter->actual_average_torque_);
    //    }

    // wait pulley tighten complete
    while (pulley1.parameter->actual_average_vel_ != 0 || pulley2.parameter->actual_average_vel_ != 0)
    {
    }
}

// upclaw hold
static void UpClawHold(void)
{
    // enable upclaw
    up_claw.Enable(&nmt, &up_claw);
    //set the torque,30%
    up_claw.Torque(&nmt, &up_claw, R5_cmd.upclaw_hold_trq);
    // wait hold complete
    while (up_claw.parameter->actual_average_vel_ != 0)
    {
    }
}

//upclaw loose
static void UpClawLoose(void)
{
    // enable upclaw
    up_claw.Enable(&nmt, &up_claw);

    // set relative pos
    up_claw.RelPos(&nmt, &up_claw, R5_cmd.upclaw_rltv_pos);
}

/**
 * MotionControlFSM() - Motion Control Finite State Machine
 * @R5_cmd: the controller command from APU
 *
 * Returns .
 */
static void MotionControlFSM(r5_cmd *R5_cmd)
{
    // choose the motion mode
    switch (R5_cmd->cntrlr_motion_cmd)
    {
    // Idle state
    case IDLE:
        // do nothing

        break;

    // upclaw hold
    case UPCLAW_HOLD:
        UpClawHold();
        break;

    // upclaw loose
    case UPCLAW_LOOSE:
        UpClawLoose();
        break;

    default:
        break;
    }
}

// controllers task control
void ControllersPoll(void *ptr)
{
    /* test here */
    RPU_PRINTF("motion test\n");
    //	enable pulley1 controller
    R5_cmd.cntrlr_motion_cmd = UPCLAW_HOLD;

    for (;;)
    {
        // controller FSM
        MotionControlFSM(&R5_cmd);

        vTaskDelay((int)(1 / R5_cmd.control_frequency_KHz));
    }
}
