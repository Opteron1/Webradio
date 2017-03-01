#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "tools.h"
#include "main.h"
#include "eth.h"
#include "utils.h"
#include "eth/ntp.h"


volatile u32 ntp_time=0UL;


u16 ntp_request(u16 idx)
{
  MAC_Addr mac;
  NTP_Header *tx_ntp;

  mac = arp_getmac(eth_getntp());

  tx_ntp = (NTP_Header*) &eth_txbuf[NTP_OFFSET];

  memset(tx_ntp, 0, NTP_HEADERLEN);

  tx_ntp->flags = (0<<6)|(1<<3)|(3<<0); //LI=0 | VN=1 | Mode=3 -> Client

  idx = udp_open(idx, mac, eth_getntp(), NTP_PORT, NTP_PORT, 0, NTP_HEADERLEN);

  return idx;
}


u32 ntp_gettime(void)
{
  s32 timeout, timeout_ntp;
  u16 idx;

  ntp_time = 0UL;

  idx = ntp_request(UDP_ENTRIES);

  if(idx < UDP_ENTRIES)
  {
    timeout     = getontime()+ETH_TIMEOUT;
    timeout_ntp = getontime()+NTP_TIMEOUT;
    for(;;)
    {
      eth_service();
  
      if(ntp_time != 0UL)
      {
        break;
      }
      if(getdeltatime(timeout_ntp) > 0)
      {
        timeout_ntp = getontime()+NTP_TIMEOUT;
        idx = ntp_request(idx);
      }
      if(getdeltatime(timeout) > 0)
      {
        break;
      }
    }
  
    udp_close(idx);
  }

  return ntp_time;
}


void ntp_udpapp(u16 idx, const unsigned char *rx, u16 rx_len, unsigned char *tx)
{
  const NTP_Header *rx_ntp;
  u32 time;

  DEBUGOUT("NTP: UDP app\r\n");
  
  rx_ntp = (const NTP_Header*) rx;

  if((rx_ntp->flags&0x07) == 4) //Mode=4 -> Server
  {
    time  = swap32(rx_ntp->trn_ts);
    time -= 2208988800UL; //seconds: 1900-1970
    time += eth_gettimediff();
    if(eth_getsummer()) //summer time
    {
      time += 3600; //add one hour
    }
    ntp_time = time;
  }

  return;
}
