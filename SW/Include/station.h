#ifndef _STATION_H_
#define _STATION_H_		1U

#include "stm32f2xx.h"
#include "debug.h"
#include "buffer.h"

//----- DEFINES -----
#define STATION_FILE                   "STATION.PLS"

#define STATION_TIMEOUT                (15) //s (play->buffer->play)
#define STATION_TRY                    (3)  //x times

#define STATION_CLOSED                 (0)
#define STATION_OPENED                 (1)
#define STATION_BUFFER                 (2)
#define STATION_OPEN                   (3)
#define STATION_ADDRMOVED              (4)
#define STATION_ERROR                  (5)
#define STATION_ERRTIMEOUT             (6)


//----- PROTOTYPES -----
void                                   station_setbitrate(u16 bitrate);
void                                   station_close(void);
u16					                           station_open(u16 item);
void                                   station_service(void);
void                                   station_closeitem(void);
u16					                           station_openitem(u16 item);

void                                   station_delitem(u16 item);
void                                   station_moveitem(u16 item, u16 direction);
void                                   station_setitemaddr(u16 item, const char *addr);
u16					                           station_getitemaddr(u16 item, char *addr);
void                                   station_setitem(u16 item, const char *name);
void                                   station_getitem(u16 item, char *name);
void                                   station_setitems(u16 items);
u16					                           station_items(void);
void                                   station_init(void);

#endif	/* _STATION_H_ */
