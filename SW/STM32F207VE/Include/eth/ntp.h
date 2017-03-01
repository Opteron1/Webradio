#ifndef _NTP_H_
#define _NTP_H_

#include "stm32f2xx.h"

//----- DEFINES -----
#define NTP_PORT                       (123)

#define NTP_TIMEOUT                    (2) //seconds

//Proto: NTP (Network Time Protocol)
#define NTP_OFFSET                     (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)
#define NTP_HEADERLEN                  (48)
typedef struct __attribute__((packed))
{
  u16     flags     :  8; // 8bit Flags: LI, VN, Mode
  u16     stratum   :  8; // 8bit Stratum
  u16     poll      :  8; // 8bit Poll
  u16     prec      :  8; // 8bit Precision
  u32     root_del  : 32; //32bit Root delay
  u32     root_dis  : 32; //32bit Root dispersion
  u32     ref_id    : 32; //32bit Reference identifier
  uint64_t         ref_ts    : 64; //64bit Reference timestamp
  uint64_t         org_ts    : 64; //64bit Originate timestamp
  uint64_t         rcv_ts    : 64; //64bit Receive timestamp
  uint64_t         trn_ts    : 64; //64bit Transmit timestamp
} NTP_Header;


//----- PROTOTYPES -----
u16                          ntp_request(u16 idx);
u32                          ntp_gettime(void);
void                         ntp_udpapp(u16 idx, const unsigned char *rx, u16 rx_len, unsigned char *tx);


#endif //_NTP_H_
