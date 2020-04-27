
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

// controller poll with specified frequency
static void ControllerPoll(controller *controller, u32 control_freq)
{
    // if disable
    if (0 == (*controller->contrl_cmd).motor_enable)
    {
        // disable the motor
        (*controller->motor).Disable(&nmt, controller->motor);
    }
    else
    {
        // if already enabled
        // enable the motor
        (*controller->motor).Enable(&nmt, controller->motor);

        // choose the operation mode
        switch ((*controller->contrl_cmd).mode_of_operation)
        {
            // pos mode
        case POS_MODE:
            /* code */
            break;

            // speed mode
        case SPEED_MODE:
            /* code */
            break;

            // torque mode
        case TORQUE_MODE:
            /* code */
            break;

        default:
            break;
        }
    }
}

/* define the controllers */

// controller parameter
controller_cmd pulley1_cmd;

// pulley1 controller
controller pulley1_controller = {
    .contrl_cmd = &pulley1_cmd,
    .motor = &pulley1,
	.Poll=ControllerPoll
    };
