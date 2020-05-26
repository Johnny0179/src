
#include <controller/maxon/maxon.h>
#include "nmt.h"

#include "controller/controller.h"
#include "common/common.h"
#include <stdlib.h>

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
	.ParaRead = MotorParaRead};

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
	nmt_frame.can_id = NMT;
	nmt_frame.can_dlc = 2;
	nmt_frame.data[0] = NMT_Start_Node;
	nmt_frame.data[1] = 0;

	if (nmt.can_device->send(&nmt_frame) != -1)
	{
		RPU_PRINTF("Start all nodes!\r\n");
	}
	else
	{
		xil_printf("CAN communication error!\r\n");
	}
}

// nmt start one node
void NMTstartID(u8 slave_id)
{
	delay_ms(DelayEpos);
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id = NMT;
	nmt_frame.can_dlc = 2;
	nmt_frame.data[0] = NMT_Start_Node;
	nmt_frame.data[1] = slave_id;

	if (nmt.can_device->send(&nmt_frame) != -1)
	{
		if (slave_id != 0)
		{
			RPU_PRINTF("Start node%d!\r\n", slave_id);
		}
		else
		{
			RPU_PRINTF("Start all nodes!\r\n");
		}
	}
	else
	{
		RPU_PRINTF("CAN communication error!\n");
	}
}

void NMTPreOperation(u8 slave_id)
{
	delay_ms(DelayEpos);
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id = NMT;
	nmt_frame.can_dlc = 2;
	nmt_frame.data[0] = NMT_Enter_PreOperational;
	nmt_frame.data[1] = 0;

	if (nmt.can_device->send(&nmt_frame) != -1)
	{
		xil_printf("Node%d enter pre operation state!\n", slave_id);
	}
	else
	{
		xil_printf("CAN communication error!\n");
	}
}

void NMTstop(u8 slave_id)
{
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id = NMT;
	nmt_frame.can_dlc = 2;
	nmt_frame.data[0] = NMT_Stop_Node;
	nmt_frame.data[1] = slave_id;

	if (nmt.can_device->send(&nmt_frame) != -1)
	{
		if (slave_id != 0)
		{
			xil_printf("Stop node%d!\n", slave_id);
		}
		else
		{
			RPU_PRINTF("Stop all nodes!\n");
		}
	}
	else
	{
		RPU_PRINTF("CAN communication error!\n");
	}
}

// cmd sync
void NMTCmdSync()
{
	struct can_frame nmt_frame;

	// nmt frame init
	nmt_frame.can_id = SYNC;
	nmt_frame.can_dlc = 0;

	nmt.can_device->send(&nmt_frame);
}

// txpdo1
int NMTTxPdo1(u8 slave_id, u16 ctrl_wrd)
{
	struct can_frame tx_pdo1_frame;
	// force the type change
	tx_pdo1_frame.can_id = (u32)(PDO1rx) + (u32)(slave_id);

	// RPU_PRINTF("PDO1rx:%lx\n",(u32)PDO1rx);
	// RPU_PRINTF("slave_id:%lx\n",(u32)slave_id);
	// RPU_PRINTF("TXPDO1 CAN ID:%lx\n",tx_pdo1_frame.can_id);

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
	tx_pdo2_frame.can_id = (u32)(PDO2rx) + (u32)(slave_id);
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
	tx_pdo3_frame.can_id = (u32)(PDO3rx) + (u32)(slave_id);

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
	tx_pdo4_frame.can_id = (u32)(PDO4rx) + (u32)(slave_id);
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
	rx_pdo1_frame.can_id = (u32)(PDO1tx) + (u32)(id);
	rx_pdo1_frame.can_dlc = 0;

	return nmt.can_device->send(&rx_pdo1_frame);
}

void MotorParaRead(struct can_frame *recv_frame)
{
	struct maxon_type *motor_para;

	u16 cob_id = recv_frame->can_id & (~0x007F);
	u16 SlaveId = (recv_frame->can_id & 0x7F);

	// RPU_PRINTF("\rcan id :0x%x\n",SlaveId);

	// choose the slave
	switch (SlaveId)
	{
	case UP_CLAW:
		motor_para = up_claw.parameter;
		break;

	case UP_WHEEL:
		motor_para = up_wheel.parameter;
		break;

	case PULLEY1:
		motor_para = pulley1.parameter;
		break;

	case PULLEY2:
		motor_para = pulley2.parameter;
		break;

	case DOWN_CLAW1:
		motor_para = down_claw1.parameter;
		break;

	case DOWN_CLAW2:
		motor_para = down_claw2.parameter;
		break;

	default:
		break;
	}

	motor_para->motor_id_ = SlaveId;

	switch (cob_id)
	{
	// 0x180
	case PDO1tx:
		motor_para->status_word_ = (u16)(recv_frame->data[1] << 8) | recv_frame->data[0];
		motor_para->torque_pv_ = (s16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
		motor_para->pos_pv_ =
			(s32)((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) | (recv_frame->data[5] << 8) | recv_frame->data[4]);
		break;

	// 0x280
	case PDO2tx:
		motor_para->status_word_ = (recv_frame->data[1] << 8) | recv_frame->data[0];
		motor_para->torque_pv_ = (s16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
		motor_para->speed_pv_ =
			(s32)((recv_frame->data[7] << 24) | (recv_frame->data[6] << 16) | (recv_frame->data[5] << 8) | recv_frame->data[4]);
		break;

	// 0x380
	case PDO3tx:
		motor_para->actual_average_vel_ =
			(s32)((recv_frame->data[3] << 24) | recv_frame->data[2] << 16 | (recv_frame->data[1] << 8) | recv_frame->data[0]);
		motor_para->actual_average_torque_ = (s16)((recv_frame->data[5] << 8) | recv_frame->data[4]);
		break;

	// 0x480
	case PDO4tx:
		motor_para->status_word_ = (u16)(recv_frame->data[1] << 8) | recv_frame->data[0];
		motor_para->serv_error_ = (u16)((recv_frame->data[3] << 8) | recv_frame->data[2]);
		motor_para->torque_pv_ = (s16)((recv_frame->data[5] << 8) | recv_frame->data[4]);
		motor_para->mode_display_ = recv_frame->data[6];
		break;

	default:
		break;
	}

	// RPU_PRINTF("motor_para->motor_id_:0x%x\n",motor_para->motor_id_);
	// RPU_PRINTF("(motor_para->status_word_ >> 5 & 1):%x\n",(motor_para->status_word_ >> 5 & 1));
	// RPU_PRINTF("(motor_para->status_word_):%x\n",(motor_para->status_word_));
}


