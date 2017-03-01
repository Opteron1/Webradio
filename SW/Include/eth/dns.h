#ifndef _DNS_H_
#define _DNS_H_

#include "stm32f2xx.h"

//----- DEFINES -----
#define DNS_PORT                       (53)

#define DNS_TIMEOUT                    (3) //seconds

//Proto: DNS Answer
#define DNSA_HEADERLEN                 (12)
typedef struct __attribute__((packed))
{
  u16     name      :  16; //16bit Name
  u16     type      :  16; //16bit Type
  u16     clas      :  16; //16bit Class
  u32     ttl       :  32; //32bit Time to live
  u16     rdlen     :  16; //16bit Data len
  //data
  u32     addr      :  32; //32bit IP Address
} DNS_Answer;

//Proto: DNS (Domain Name System)
#define DNS_OFFSET                     (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)
#define DNS_HEADERLEN                  (12)
#define DNS_DATASTART                  (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN+DNS_HEADERLEN)
#define DNS_FLAGS_RESPONSE             (0x8000)
#define DNS_FLAGS_QUERY                (0x0100)
#define DNS_TYPE_A                     (0x0001)
#define DNS_TYPE_CNAME                 (0x0005)
#define DNS_CLASS_IN                   (0x0001)
typedef struct __attribute__((packed))
{
  u16     id        : 16; //16bit ID
  u16     flags     : 16; //16bit Flags
/*u16     qr        :  1; // 1bit QR query or response
  u16     opcode    :  4; // 4bit Opcode
  u16     flags     :  7; // 7bit Flags
  u16     rcode     :  4; // 4bit RCode*/
  u16     qdcount   : 16; //16bit Entries in question section
  u16     ancount   : 16; //16bit Entries in answer section
  u16     nscount   : 16; //16bit Entries in authority section
  u16     arcount   : 16; //16bit Entries in additional records section
} DNS_Header;


//----- PROTOTYPES -----
u16                           dns_request(u16 idx, const char *domain);
IP_Addr                       dns_getip(const char *domain);
void                          dns_udpapp(u16 idx, const u8 *rx, u16 rx_len, u8 *tx);


#endif //_DNS_H_
