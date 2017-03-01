#ifndef _SHARE_H_
#define _SHARE_H_		1U

#include "stm32f2xx.h"

//----- DEFINES -----
#define SHARE_FILE                     "SHARE.PLS"


//----- PROTOTYPES -----
void                          share_service(void);
void                          share_closeitem(void);
u16                           share_openitem(u16 item);
void                          share_getitem(u16 item, char *name);
u16                           share_items(void);
void                          share_init(void);


#endif //_SHARE_H_
