#ifndef _ALARM_H_
#define _ALARM_H_		1U

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm32f2xx.h"
#include "tools.h"
#include "debug.h"
#include "menu.h"
#include "mmc.h"
#include "settings.h"
#include "menu_dlg.h"


//----- DEFINES -----
#define ALARMTIMES                     (8)
#define ALARM_FILE                     "ALARM.INI"
#define ALARM_FILEITEMS                (3)


typedef struct
{
  u16 wdays;  //Bit0=Sunday, Bit1=Monday, Bit2=Tueday...
  u16 h;
  u16 m;
  u16 action; //0=nothing, 1=play, 2=standby
} ALARMTIME;

u16 alarm_check(TIME *time);
u16 alarm_getfile(char *dst, u16 nr);
u16 alarm_getvol(void);
u16 alarm_settime(u16 item, ALARMTIME *time);
u16 alarm_gettime(u16 item, ALARMTIME *time);
u16 alarm_parsetime(const char *src, ALARMTIME *time);
void alarm_load(void);
u16 alarm_openitem(u16 item);
void alarm_getitem(u16 item, char *name);
u16 alarm_items(void);
void alarm_init(void);

#endif /* _ALARM_H_ */
