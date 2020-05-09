
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

// controller parameter
controller_cmd pulley1_cmd;

// controllers parameter
controllers_cmd controllers_command = {
    // defualt control frequency is 0.01 KHz
    .control_frequency_KHz = 0.01};

// controller poll with specified frequency KHz
static void ControllerPoll(const controller *controller)
{

    // if disable
    if (1 == controller->contrl_cmd->controller_state)
    {
        // disable the motor
        controller->motor->Disable(&nmt, controller->motor);
    }
    else if (2 == controller->contrl_cmd->controller_state)
    {
        // check if already enabled?
        if ((controller->motor->parameter->status_word_ & 0x00ff) != 0x37)
        {
            // enable the motor
            controller->motor->Enable(&nmt, controller->motor);
        }

        // choose the operation mode
        switch (controller->contrl_cmd->mode_of_operation)
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

// pulley1 controller
static controller pulley1_controller = {
    .contrl_cmd = &pulley1_cmd,
    .motor = &pulley1,
    .Poll = ControllerPoll};

// controllers poll
void ControllersPoll(void *ptr)
{
    for (;;)
    {

        // pulley1 controller poll
        pulley1_controller.Poll(&pulley1_controller);

        vTaskDelay((int)(1000 / controllers_command.control_frequency_KHz));
        // vTaskDelay(100000);
    }
}
