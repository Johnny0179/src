
#include "nmt.h"

#include "maxon/maxon.h"
#include "robot/robot.h"

extern struct can ps_can0;
extern struct maxon up_claw;
extern struct maxon up_wheel;
extern struct maxon pulley1;
extern struct maxon pulley2;
extern struct maxon down_claw1;
extern struct maxon down_claw2;


struct nmt nmt = {
	.can_device = &ps_can0, 
	.Init = NMTInit, 
	.Start = NMTstart, 
	.StartId = NMTstartID, 
	.PreOperation = NMTPreOperation, 
	.Stop = NMTstop, 
	.CmdSync = NMTCmdSync, 
	.TxPdo1 = NMTTxPdo1, 
	.TxPdo2 = NMTTxPdo2, 
	.TxPdo3 = NMTTxPdo3, 
	.TxPdo4 = NMTTxPdo4, 
	.RxPdo1 = NMTRxPdo1, 
	.ParaRead = MotorParaRead
};


// nmt init
 int NMTInit(void)
{
	return ps_can0.init();
}


// nmt start all nodes
 void NMTstart(void)
{
	delay_ms(DelayEpos);
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id	= NMT;
	nmt_frame.can_dlc	= 2;
	nmt_frame.data[0]	= NMT_Start_Node;
	nmt_frame.data[1]	= 0;

	if (nmt.can_device->send(&nmt_frame) != -1) {
		xil_printf("Start all nodes!\r\n");
	}
	else {
		xil_printf("CAN communication error!\r\n");
	}
}


// nmt start one node
 void NMTstartID(u8 slave_id)
{
	delay_ms(DelayEpos);
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id	= NMT;
	nmt_frame.can_dlc	= 2;
	nmt_frame.data[0]	= NMT_Start_Node;
	nmt_frame.data[1]	= slave_id;

	if (nmt.can_device->send(&nmt_frame) != -1) {
		if (slave_id != 0) {
			xil_printf("Start node%d!\r\n", slave_id);
		}
		else {
			xil_printf("Start all nodes!\r\n");
		}
	}
	else {
		xil_printf("CAN communication error!\n");
	}
}


 void NMTPreOperation(u8 slave_id)
{
	delay_ms(DelayEpos);
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id	= NMT;
	nmt_frame.can_dlc	= 2;
	nmt_frame.data[0]	= NMT_Enter_PreOperational;
	nmt_frame.data[1]	= 0;

	if (nmt.can_device->send(&nmt_frame) != -1) {
		xil_printf("Node%d enter pre operation state!\n", slave_id);
	}
	else {
		xil_printf("CAN communication error!\n");
	}
}


 void NMTstop(u8 slave_id)
{
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id	= NMT;
	nmt_frame.can_dlc	= 2;
	nmt_frame.data[0]	= NMT_Stop_Node;
	nmt_frame.data[1]	= slave_id;

	if (nmt.can_device->send(&nmt_frame) != -1) {
		if (slave_id != 0) {
			xil_printf("Stop node%d!\n", slave_id);
		}
		else {
			xil_printf("Stop all nodes!\n");
		}
	}
	else {
		xil_printf("CAN communication error!\n");
	}
}


// cmd sync
 void NMTCmdSync()
{
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id	= SYNC;
	nmt_frame.can_dlc	= 0;

	nmt.can_device->send(&nmt_frame);
}


// txpdo1
 int NMTTxPdo1(u8 slave_id, u16 ctrl_wrd)
{
	struct can_frame tx_pdo1_frame;

	// tx_pdo1 frame init
	tx_pdo1_frame.can_id = PDO1rx + slave_id;
	tx_pdo1_frame.can_dlc = 2;
	tx_pdo1_frame.data[0] = ctrl_wrd & 0xff;
	tx_pdo1_frame.data[1] = (ctrl_wrd >> 8) & 0xff;

	return nmt.can_device->send(&tx_pdo1_frame);
}


// txpdo2
 int NMTTxPdo2(u8 slave_id, u16 ctrl_wrd, s32 pos_sv, u16 mode_of_operation)
{
	struct can_frame tx_pdo2_frame;

	// tx_pdo2 frame init
	tx_pdo2_frame.can_id = PDO2rx + slave_id;
	tx_pdo2_frame.can_dlc = 7;

	tx_pdo2_frame.data[0] = ctrl_wrd & 0xff;
	tx_pdo2_frame.data[1] = (ctrl_wrd >> 8) & 0xff;

	tx_pdo2_frame.data[2] = pos_sv & 0xff;
	tx_pdo2_frame.data[3] = (pos_sv >> 8) & 0xff;
	tx_pdo2_frame.data[4] = (pos_sv >> 16) & 0xff;
	tx_pdo2_frame.data[5] = (pos_sv >> 24) & 0xff;
	tx_pdo2_frame.data[6] = mode_of_operation;

	return nmt.can_device->send(&tx_pdo2_frame);
}


// txpdo3
 int NMTTxPdo3(u8 slave_id, s16 target_torque, u16 mode_of_operation)
{
	struct can_frame tx_pdo3_frame;

	// tx_pdo4 frame init
	tx_pdo3_frame.can_id = PDO3rx + slave_id;

	tx_pdo3_frame.can_dlc = 3;
	tx_pdo3_frame.data[0] = target_torque & 0xff;
	tx_pdo3_frame.data[1] = (target_torque >> 8) & 0xff;
	tx_pdo3_frame.data[2] = mode_of_operation;

	return nmt.can_device->send(&tx_pdo3_frame);
}


// txpdo4
 int NMTTxPdo4(u8 slave_id, s32 speed_set, u16 mode_of_operation)
{
	struct can_frame tx_pdo4_frame;

	// tx_pdo4 frame init
	tx_pdo4_frame.can_id = PDO4rx + slave_id;
	tx_pdo4_frame.can_dlc = 5;
	tx_pdo4_frame.data[0] = speed_set & 0xff;
	tx_pdo4_frame.data[1] = (speed_set >> 8) & 0xff;
	tx_pdo4_frame.data[2] = (speed_set >> 16) & 0xff;
	tx_pdo4_frame.data[3] = (speed_set >> 24) & 0xff;
	tx_pdo4_frame.data[4] = mode_of_operation;

	return nmt.can_device->send(&tx_pdo4_frame);
}


// rxpdo1
 int NMTRxPdo1(u8 id)
{
	struct can_frame rx_pdo1_frame;
	rx_pdo1_frame.can_id = PDO1tx + id;
	rx_pdo1_frame.can_dlc = 0;

	return nmt.can_device->send(&rx_pdo1_frame);
}


 void MotorParaRead(struct can_frame * recv_frame)
{
	u16 			cob_id = recv_frame->can_id & (~0x007F);
	u16 			SlaveId = (recv_frame->can_id & 0x7F);

	struct maxon motor;
	motor.parameter.motor_id_ = SlaveId;

	switch (cob_id)
	{
		// 0x180
		case PDO1tx:
			motor.parameter.status_word_ = (u16) (recv_frame->data[1] << 8) | recv_frame->data[0];
			motor.parameter.torque_pv_ = (s16) ((recv_frame->data[3] << 8) | recv_frame->data[2]);
			motor.parameter.pos_pv_ =
				 (s32) ((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) | (recv_frame->data[5] << 8) | recv_frame->data[4]);
			break;

		// 0x280
		case PDO2tx:
			motor.parameter.status_word_ = (recv_frame->data[1] << 8) | recv_frame->data[0];
			motor.parameter.torque_pv_ = (s16) ((recv_frame->data[3] << 8) | recv_frame->data[2]);
			motor.parameter.speed_pv_ =
				 (s32) ((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) | (recv_frame->data[5] << 8) | recv_frame->data[4]);
			break;

		// 0x380
		case PDO3tx:
			motor.parameter.actual_average_vel_ =
				 (s32) ((recv_frame->data[3] << 24) | recv_frame->data[2] << 16 | (recv_frame->data[1] << 8) | recv_frame->data[0]);
			motor.parameter.actual_average_torque_ = (s16) ((recv_frame->data[5] << 8) | recv_frame->data[4]);
			break;

		// 0x480
		case PDO4tx:
			motor.parameter.status_word_ = (u16) (recv_frame->data[1] << 8) | recv_frame->data[0];
			motor.parameter.serv_error_ = (u16) ((recv_frame->data[3] << 8) | recv_frame->data[2]);
			motor.parameter.torque_pv_ = (s16) ((recv_frame->data[5] << 8) | recv_frame->data[4]);
			motor.parameter.mode_display_ = recv_frame->data[6];
			break;

		default:
			break;
	}

	switch (motor.parameter.motor_id_)
	{
		case UP_CLAW:
			up_claw.parameter = motor.parameter;
			break;

		case UP_WHEEL:
			up_wheel.parameter = motor.parameter;
			break;

		case PULLEY1:
			pulley1.parameter = motor.parameter;
			break;

		case PULLEY2:
			pulley2.parameter = motor.parameter;
			break;

		case DOWN_CLAW1:
			down_claw1.parameter = motor.parameter;
			break;

		case DOWN_CLAW2:
			down_claw2.parameter = motor.parameter;
			break;

		default:
			break;
	}

}


