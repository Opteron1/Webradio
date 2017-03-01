#ifndef _DHCP_H_
#define _DHCP_H_

#include "stm32f2xx.h"

//----- DEFINES -----
#define DHCPSERVER_PORT                (67)
#define DHCPCLIENT_PORT                (68)

#define DHCP_TIMEOUT                   (2) //seconds

#define DHCP_CLOSED                    (0)
#define DHCP_DISCOVER                  (1)
#define DHCP_REQUEST                   (2)
#define DHCP_ACK                       (3)

//Proto: DHCP (Dynamic Host Configuration Protocol)
#define DHCP_OFFSET                    (ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN)
#define DHCP_HEADERLEN                 (44+64+128)
#define DHCP_OP_REQUEST                (1)
#define DHCP_OP_REPLY                  (2)
#define DHCP_HTYPE_ETH                 (1)
#define DHCP_HLEN_MAC                  (6)
#define DHCP_MCOOKIE                   (0x63825363)
#define DHCP_OPTION_NETMASK            (1)
#define DHCP_OPTION_ROUTER             (3)
#define DHCP_OPTION_TIMESERVER         (4)
#define DHCP_OPTION_DNS                (6)
#define DHCP_OPTION_HOSTNAME           (12)
#define DHCP_OPTION_REQUESTEDIP        (50)
#define DHCP_OPTION_MSGTYPE            (53)
# define DHCP_MSG_DISCOVER             (1)
# define DHCP_MSG_OFFER                (2)
# define DHCP_MSG_REQUEST              (3)
# define DHCP_MSG_DECLINE              (4)
# define DHCP_MSG_ACK                  (5)
# define DHCP_MSG_NAK                  (6)
# define DHCP_MSG_RELEASE              (7)
#define DHCP_OPTION_SERVERID           (54)
#define DHCP_OPTION_PARAMLIST          (55)
#define DHCP_OPTION_CLIENTID           (61)
#define DHCP_OPTION_SMTPSERVER         (69)
#define DHCP_OPTION_POP3SERVER         (70)
typedef struct __attribute__((packed))
{
  u16     op        :  8; // 8bit OP
  u16     htype     :  8; // 8bit HType
  u16     hlen      :  8; // 8bit HLen
  u16     hops      :  8; // 8bit HOPS
  u32     xid       : 32; //32bit ID
  u16     secs      : 16; //16bit Seconds
  u16     flags     : 16; //16bit Flags
  u32    ciaddr    : 32; //32bit Client IP
  u32    yiaddr    : 32; //32bit Your IP
  u32    siaddr    : 32; //32bit Server IP
  u32    giaddr    : 32; //32bit Relay-Agent IP
  union __attribute__((packed))
  {
    uint64_t       mac       : 48; //48bit Client MAC
    uint8_t        addr[16];       //16byte MAC
  } chaddr;
  uint8_t          sname[64];      //64byte Server name
  uint8_t          file[128];      //128byte File
  u32              mcookie   : 32; //32bit Magic Cookie: 0x63825363
  uint8_t          options[312-4]; //312byte Options (includes Magic Cookie)
} DHCP_Header;


//----- PROTOTYPES -----
u16                           dhcp_request(u16 idx, u16 msg);
u16                           dhcp_getcfg(void);
void                          dhcp_udpapp(u16 idx, const u8 *rx, u16 rx_len, u8 *tx);


#endif //_DHCP_H_
