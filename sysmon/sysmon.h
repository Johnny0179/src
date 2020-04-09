#ifndef _SYSMON_H_
#define _SYSMON_H_

int SysMonInit(void);

// get the temperature of ps core
u16 GetPsCoreTemp(void);

// get the temperature of pl core
u16 GetPlCoreTemp(void);

#endif
