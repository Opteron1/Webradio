#ifndef	_ICONS_H_
#define	_ICONS_H_		1U

#include "stm32f2xx.h"

/*----- DEFINES -----*/
#define ICON_STATION			img_station
#define ICON_SHARE				img_share
#define ICON_CARD					img_card
#define ICON_CLOCK				img_clock
#define ICON_SETTINGS			img_settings
#define ICON_BACK					img_back
#define ICON_POWER				img_power

#define ICON_LARGE				(0)
#define ICON_NORMAL				(1)
#define ICON_SMALL				(2)

/*----- GLOBALS -----*/
extern const u32 img_station[3][32];
extern const u32 img_share[3][32];
extern const u32 img_card[3][32];
extern const u32 img_clock[3][32];
extern const u32 img_settings[3][32];
extern const u32 img_back[3][32];
extern const u32 img_power[3][32];



#endif		/* _ICONS_H_ */
