#ifndef _CARD_H_
#define _CARD_H_	1U

#include "stm32f2xx.h"
#include "debug.h"
#include "buffer.h"
#include "settings.h"
#include "mmc.h"
#include "tools.h"
#include "vs.h"
#include "main.h"


//----- DEFINES -----
#define CARD_TIMEOUT                   (3) //seconds

#define CARD_CLOSED                    (0)
#define CARD_PLAY                      (1)


void card_service(void);
void card_closeitem(void);
u16 card_openfile(const char *file);
u16 card_nextitem(void);
u16 card_openitem(u16 item);
void card_getitem(u16 item, char *name);
u16 card_items(void);
void card_init(void);

#endif	/* _CARD_H_ */
