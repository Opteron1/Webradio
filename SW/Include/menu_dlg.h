#ifndef _MENU_DLG_H_
#define _MENU_DLG_H_		1U

#include "stm32f2xx.h"
#include "menu.h"
#include "alarm.h"
#include "eth.h"


char next_char(char c, u16 dir);
u16 dlg_ip(const char* title, IP_Addr *value);
u16 dlg_rgb(const char* title, u16 *value);
u16 dlg_or(const char* title, s16 *value, s16 p1, s16 p2);
u16 dlg_nr(const char* title, s16 *value, s16 min, s16 max, s16 step);
u16 dlg_str(const char* title, const char *value, u16 len, char *buf, u16 buf_len);
u16 dlg_rawir(u16 i);
u16 dlg_msg(const char* title, const char *msg);
s16 dlg_service(void);

#endif	/* _MENU_DLG_H_ */
