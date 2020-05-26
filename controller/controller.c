
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

// controllers parameter
controllers_cmd controllers_command = {
    // defualt control frequency is 0.01 KHz
    .control_frequency_KHz = 0.01};

// enable controller
static void EnableController(const controller *controller)
{
    // check if already enabled?
    if ((controller->motor->parameter->status_word_ & 0x00ff) != 0x37)
    {
        // enable the motor
        RPU_PRINTF("enable controller:%d\n",controller->motor->parameter->motor_id_);
        controller->motor->Enable(&nmt, controller->motor);
    }
}

// impedance control
static void ImpedanceControl(const struct maxon *motor)
{
    s16 init_torque, current_torque;
    // get current torque
    current_torque=motor->parameter->actual_average_torque_;

    RPU_PRINTF("controller[%d] current torque: %d\n",motor->parameter->motor_id_,current_torque);

}

/* define the controllers */
// pulleys control
static void PulleyControl(const controller *controller)
{
    switch (controller->pulley_control_cmd_->motion_mode)
    {
    case IDLE_MODE:
        // if not disabled
        if (controller->motor->parameter->status_word_ != 0x0240)
        {
            controller->motor->Disable(&nmt, controller->motor);
        }
        break;

    case FOLLOW_MODE:
        EnableController(controller);
        ImpedanceControl(controller->motor);
        break;

    case PULL_MODE:
        /* code */
        break;

    default:
        break;
    }
    // //check if enabled
    // if (controller->pulley_control_cmd_->state == CONTROLLER_ENABLE)
    // {
    //     // check if already enabled?
    //     if ((controller->motor->parameter->status_word_ & 0x00ff) != 0x37)
    //     {
    //         // enable the motor
    //         RPU_PRINTF("enable motor\n");
    //         controller->motor->Enable(&nmt, controller->motor);
    //     }

    //     // choose the operation mode
    //     switch (controller->pulley_control_cmd_->motion_mode)
    //     {
    //     case FOLLOW_MODE:
    //         /* code */
    //         break;

    //     case PULL_MODE:
    //         /* code */
    //         break;

    //     default:
    //         break;
    //     }
    // } // disable the controller
    // else if (controller->pulley_control_cmd_->state == CONTROLLER_DISABLE)
    // {
    //     // not disabled
    //     if (controller->motor->parameter->status_word_ != 0x0240)
    //     {
    //         controller->motor->Disable(&nmt, controller->motor);
    //     }
    // }
}
// pulley1 controller
static controller pulley1_controller = {
    .pulley_control_cmd_ = &controllers_command.pulley1_cmd_,
    .motor = &pulley1,
    .poll = PulleyControl};

// //Up claw control
// static void UpClawControl(const controller *controller)
// {
//     switch (controller->contrl_cmd->claw_control_cmd_.motion_mode)
//     {

//     case CLAW_HOLD:
//         //    claw hold
//         break;

//     case CLAW_LOOSE:
//         // claw loose

//         break;
//     default:
//         break;
//     }
// }

// //down claws control
// static void DownClawControl(const controller *controller)
// {
//     switch (controller->contrl_cmd->claw_control_cmd_.motion_mode)
//     {

//     case CLAW_HOLD:
//         //    claw hold
//         break;

//     case CLAW_LOOSE:
//         // claw loose

//         break;
//     default:
//         break;
//     }
// }

// // upwheel control
// static void UpWheelControl(const controller *controller)
// {
//     // move to target distance
// }

// // controller poll with specified frequency KHz
// static void ControllerPoll(const controller *controller)
// {
//     // if disable
//     if (CONTROLLER_DISABLE == controller->contrl_cmd->controller_state)
//     {
//         // disable the motor
//         controller->motor->Disable(&nmt, controller->motor);
//     }
//     else if (CONTROLLER_ENABLE == controller->contrl_cmd->controller_state)
//     {
//         // check if already enabled?
//         if ((controller->motor->parameter->status_word_ & 0x00ff) != 0x37)
//         {
//             // enable the motor
//             RPU_PRINTF("enable motor\n");
//             controller->motor->Enable(&nmt, controller->motor);
//         }

//         /* choose motor */
//         // if up claw
//         if (controller->motor->parameter->motor_id_ == 1)
//         {
//             // up claw control
//             UpClawControl(controller);
//         } //if down claws
//         else if (controller->motor->parameter->motor_id_ == 5 || controller->motor->parameter->motor_id_ == 6)
//         {
//             //down claw control
//             DownClawControl(controller);
//         } //if upwheel
//         else if (controller->motor->parameter->motor_id_ == 2)
//         {
//             // upwheel control
//         } //if pulleys
//         else if (controller->motor->parameter->motor_id_ == 3 || controller->motor->parameter->motor_id_ == 4)
//         {
//             // pulleys control
//         }
//     }
// }

// controllers poll
void ControllersPoll(void *ptr)
{
    /* test here */

    //	enable pulley1 controller
    controllers_command.pulley1_cmd_.motion_mode = FOLLOW_MODE;

    for (;;)
    {

        // pulley1 controller poll
        pulley1_controller.poll(&pulley1_controller);

        vTaskDelay((int)(1 / controllers_command.control_frequency_KHz));
    }
}
