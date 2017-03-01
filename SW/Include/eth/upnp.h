#ifndef _UPNP_H_
#define _UPNP_H_		1U

#include "stm32f2xx.h"

//----- DEFINES -----
#define UPNP_PORT                      (8080)


//----- PROTOTYPES -----
u16                           upnp_getport(void);
char*                         upnp_getuuid(void);
void                          upnp_tcpapp(u16 idx, const char *rx, u16 rx_len, unsigned char *tx);


#endif //_UPNP_H_
