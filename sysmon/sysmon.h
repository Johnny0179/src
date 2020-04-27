#ifndef _SYSMON_H_
#define _SYSMON_H_
#include "openamp/inter_core_com.h"

typedef struct sysmon_type
{
    int fan_refresh_cnt;
//    _Bool nmt_start_flag;
//    _Bool nmt_stop_flag;
} sysmon;

int SysMonInit(void);

// get the temperature of ps core
u16 GetPsCoreTemp(void);

// get the temperature of pl core
u16 GetPlCoreTemp(void);

// system monitor poll
void SysMonPoll(sysmon *sysmon);

#endif
