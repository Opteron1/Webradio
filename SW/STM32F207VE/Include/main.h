#ifndef _MAIN_H_
#define _MAIN_H_	1U

#include "stm32f2xx.h"
#include "system_init.h"
#include "TFT.h"
#include "eth.h"
#include "debug.h"
#include "io.h"
#include "menu.h"
#include "vs.h"
#include "tools.h"
#include "alarm.h"
#include "mmc.h"
#include "const.h"


//----- DEFINES -----
#define DRAWALL                        (1<<0)
#define SEC_CHANGED                    (1<<1)
#define MIN_CHANGED                    (1<<2)
#define HOUR_CHANGED                   (1<<3)
#define DAY_CHANGED                    (1<<4)
#define ALARM_PLAY                     (1<<5)
#define ALARM_STANDBY                  (1<<6)
#define DRAW_FHT											 (1<<7)


char* getclock(void);
char* getdate(void);
void gettime(TIME* t);
void settime(u32 s);
s32 getdeltatime(s32 t);
s32 getontime(void);
u16 standby_isactive(void);
void standby_off(void);
u16 standby(u16 param);

#endif /* _MAIN_H_ */
