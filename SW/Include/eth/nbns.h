#ifndef _NBNS_H_
#define _NBNS_H_		1U

#include "stm32f2xx.h"

//----- DEFINES -----
#define NBNS_PORT                      (137)

//Proto: NBNS Question
#define NBNSQ_HEADERLEN                (38)
#define NBNSQ_TYPE_NB                  (0x0020)
#define NBNSQ_CLASS_IN                 (0x0001)
typedef struct __attribute__((packed))
{
  u16     len       :   8; // 8bit Len: 32
  char    name[33];        //33byte Name + null
  u16     type      :  16; //16bit Type
  u16     clas      :  16; //16bit Class
} NBNS_Question;

//Proto: NBNS Answer
#define NBNSA_HEADERLEN                (50)
#define NBNSA_TYPE_NB                  (0x0020)
#define NBNSA_CLASS_IN                 (0x0001)
typedef struct __attribute__((packed))
{
  u16     len       :   8; // 8bit Len: 32
  char    name[33];        //33byte Name + null
  u16     type      :  16; //16bit Type
  u16     clas      :  16; //16bit Class
  u32     ttl       :  32; //32bit Time to live
  u16     rdlen     :  16; //16bit Data len
  u16     flags     :  16; //16bit Flags    - rdata
  u32     addr      :  32; //32bit IP Addr  - rdata
} NBNS_Answer;

//Proto: NBNS (NetBIOS Name service)
#define NBNS_OFFSET                    (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)
#define NBNS_HEADERLEN                 (12)
#define NBNS_OPMASK                    (0x7800)
#define NBNS_REPLYMASK                 (0x000F)
#define NBNS_FLAG_RESPONSE             (1<<15)
#define NBNS_FLAG_QUERY                (0<<15)
#define NBNS_FLAG_AUTHORITY            (1<<10)
#define NBNS_OP_QUERY                  (0<<11)

typedef struct __attribute__((packed))
{
  u16     id        :  16; //16bit Transaction ID
  u16     flags_op  :  16; //16bit Flags
  u16     qdcount   :  16; //16bit Question Entries
  u16     ancount   :  16; //16bit Answer RRs
  u16     nscount   :  16; //16bit Authority RRs
  u16     arcount   :  16; //16bit Additional RRs
  union __attribute__((packed))
  {
    NBNS_Question qd;
    NBNS_Answer   an;
  } data;
} NBNS_Header;


//----- PROTOTYPES -----
void                                   nbns_reply(u16 idx, u16 id);
void                                   nbns_udpapp(u16 idx, const u8 *rx, u16 rx_len, u8 *tx);


#endif //_NBNS_H_
