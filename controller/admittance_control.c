#include "controller/controller.h"

//maxon motors
extern struct maxon up_claw;
extern struct maxon up_wheel;
extern struct maxon pulley1;
extern struct maxon pulley2;
extern struct maxon down_claw1;
extern struct maxon down_claw2;

//nmt device
extern struct nmt nmt;

extern struct controllers_cmd controllers_command;

// impedance control state
// 0 idle
// 1 inited
static int impedance_control_state = 0;

// Admittance Control
void AdmittanceControl()
{
    // init torque
    s16 pulley1_trq_0 = 0;
    s16 pulley2_trq_0 = 0;

    // current torque
    s16 pulley1_trq_t = 0;
    s16 pulley2_trq_t = 0;

    // last torque
    s16 trq_last = 0;

    s16 pulley1_delta_trq;
    s16 pulley2_delta_trq;

    s32 pulley1_delta_pos;
    s32 pulley2_delta_pos;

    // stiffness factor
    float Kd = 0.04;

    // // init configuration
    // if (impedance_control_state == 0 && controllers_command.pulleys_cmd_.motion_mode != 0)
    // {
    //     // Enable motors
    //     EnableController(motor);

    //     // tighten the pulleys
    //     TightenPulleys();

    //     // get init torque
    //     pulley1_trq_0 = motor->parameter->actual_average_torque_;

    //     // change the state to inited
    //     impedance_control_state = 1;
    // }

    // Enable motors
    EnableController(&pulley1);
    EnableController(&pulley2);

    // tighten the pulleys
    TightenPulleys();

    // lock the pos
    pulley1.RelPos(&nmt, &pulley1, 0);
    pulley1.RelPos(&nmt, &pulley2, 0);

    // delay 3s
    // delay_ms(3000);

    // pulley1_trq_0 = motor->parameter->actual_average_torque_;

    // wait torque be stable, %1 and %2 error
    do
    {
        pulley1_trq_0 = pulley1.parameter->actual_average_torque_;
        pulley2_trq_0 = pulley2.parameter->actual_average_torque_;
        delay_ms(500);
    } while (abs(pulley1.parameter->actual_average_torque_ - pulley1_trq_0) > 10 || abs(pulley2.parameter->actual_average_torque_ - pulley2_trq_0) > 10);

    for (;;)
    {
        // delay 10ms, control frequency 100hz
        delay_ms(10);

        // get current torque
        pulley1_trq_t = pulley1.parameter->actual_average_torque_;
        pulley2_trq_t = pulley2.parameter->actual_average_torque_;

        pulley1_delta_trq = pulley1_trq_0 - pulley1_trq_t;
        pulley2_delta_trq = pulley2_trq_0 - pulley2_trq_t;

        RPU_PRINTF("controller[%d] init torque:%d\n", pulley1.parameter->motor_id_, pulley1_trq_0);
        RPU_PRINTF("controller[%d] current torque:%d\n", pulley1.parameter->motor_id_, pulley1_trq_t);

        RPU_PRINTF("controller[%d] init torque:%d\n", pulley2.parameter->motor_id_, pulley2_trq_0);
        RPU_PRINTF("controller[%d] current torque:%d\n", pulley2.parameter->motor_id_, pulley2_trq_t);

        RPU_PRINTF("\n");

        /*         // delta torque threshold, 1%
        if (abs(pulley1_delta_trq) < 10)
        {
            pulley1_delta_pos = 0;
            RPU_PRINTF("delta torqe:%d\n", 0);
            RPU_PRINTF("delta pos:%d\n", 0);
        }
        else
        {
            pulley1_delta_pos = (s32)((pulley1_delta_trq) / Kd);
            RPU_PRINTF("delta torqe:%d\n", pulley1_delta_trq);
            RPU_PRINTF("delta pos:%d\n", pulley1_delta_pos);

            // only loose
            if (pulley1_delta_pos < 0)
            {
                pulley1->RelPos(&nmt, pulley1, pulley1_delta_pos);
            }
        } */

        // only loose
        if (pulley1_delta_trq < -10)
        {
            pulley1_delta_pos = (s32)((pulley1_delta_trq) / Kd);
            RPU_PRINTF("pulley1 delta torqe:%d\n", pulley1_delta_trq);
            RPU_PRINTF("pulley1 delta pos:%d\n", pulley1_delta_pos);
            pulley1.RelPos(&nmt, &pulley1, pulley1_delta_pos);
        }

        // only loose
        if (pulley2_delta_trq < -10)
        {
            pulley2_delta_pos = (s32)((pulley2_delta_trq) / Kd);
            RPU_PRINTF("pulley2 delta torqe:%d\n", pulley2_delta_trq);
            RPU_PRINTF("pulley2 delta pos:%d\n", pulley2_delta_pos);
            pulley2.RelPos(&nmt, &pulley2, pulley2_delta_pos);
        }

        // trq_last = motor->parameter->actual_average_torque_;
    }

    /*     for (;;)
    {
        delay_ms(2000);
        // tighten the pulleys
        // TightenPulleys();

        // lock the pos
        // motor->RelPos(&nmt, motor, 0);
        delay_ms(2000);
        motor->RelPos(&nmt, motor, -10000);
        // print current pos
        RPU_PRINTF("current pos:%d\n", motor->parameter->pos_pv_);
        delay_ms(2000);
        motor->RelPos(&nmt, motor, 10000);
        RPU_PRINTF("current pos:%d\n", motor->parameter->pos_pv_);
    } */
}
