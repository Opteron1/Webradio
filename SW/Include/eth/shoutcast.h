#ifndef _SHOUTCAST_H_
#define _SHOUTCAST_H_		1U

#include "stm32f2xx.h"

//----- DEFINES -----
#define SHOUTCAST_SERVERPORT           (80)
#define SHOUTCAST_CLIENTPORT1          (1001)
#define SHOUTCAST_CLIENTPORT2          (1002)
#define SHOUTCAST_CLIENTPORT3          (1003)

#define SHOUTCAST_TIMEOUT              (15) //s
#define SHOUTCAST_TRY                  (3)  //times

#define SHOUTCAST_CLOSED               (0)
#define SHOUTCAST_CLOSE                (1)
#define SHOUTCAST_OPENED               (2)
#define SHOUTCAST_HEADER               (3)
#define SHOUTCAST_OPEN                 (4)
#define SHOUTCAST_ADDRMOVED            (5)
#define SHOUTCAST_SERVERFULL           (6)
#define SHOUTCAST_ERROR                (7)
#define SHOUTCAST_ERRTIMEOUT           (8)


//----- PROTOTYPES -----
void                          shoutcast_close(void);
u16                           shoutcast_open(void);
void                          shoutcast_putogg(const unsigned char *s, u16 len);
void                          shoutcast_putdata(const unsigned char *s, u16 len);
void                          shoutcast_tcpapp(u16 idx, const unsigned char *rx, u16 rx_len, unsigned char *tx);


#endif //_SHOUTCAST_H_
