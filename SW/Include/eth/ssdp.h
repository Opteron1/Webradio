#ifndef _SSDP_H_
#define _SSDP_H_

#include "stm32f2xx.h"

//----- DEFINES -----
#define SSDP_PORT                      (1900)
#define SSDP_MULTICAST                 ((239UL<<0)|(255UL<<8)|(255UL<<16)|(250UL<<24))

//Proto: SSDP (Simple Service Discovery Protocol)
#define SSDP_OFFSET                    (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)


//----- PROTOTYPES -----
void                                   ssdp_advertise(void);
void                                   ssdp_udpapp(u16 idx, const char *rx, u16 rx_len, unsigned char *tx);


#endif //_SSDP_H_
