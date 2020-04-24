#ifndef _SYSMON_H_
#define _SYSMON_H_

// r5 system state structure
typedef struct r5_state_type
{
    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // ps core temperature
    u8 ps_core_temp;

    // pl core temperature
    u8 pl_core_temp;

} r5_state;

// r5 system command state structure
typedef struct r5_cmd_type
{
    // the id of the r5 core, 0 or 1
    u8 r5_id;

    // nmt control
    u8 nmt_control;    

} r5_cmd;

int SysMonInit(void);

// get the temperature of ps core
u16 GetPsCoreTemp(void);

// get the temperature of pl core
u16 GetPlCoreTemp(void);

#endif
