#include "eth.h"
#include "main.h"

#include "eth/dhcp.h"
#include "eth/dns.h"
#include "eth/nbns.h"
#include "eth/ntp.h"
#include "eth/ssdp.h"
#include "eth/upnp.h"
#include "eth/http.h"
#include "eth/shoutcast.h"
#include "eth/rtsp.h"

Device dev;
TCP_Table tcp_table[TCP_ENTRIES];
UDP_Table udp_table[UDP_ENTRIES];
volatile MAC_Addr requested_mac=0UL;
unsigned char eth_txbuf[ETH_MTUSIZE], *eth_rxbuf, eth_rxfifo[ETH_RXFIFO][ETH_MTUSIZE];
u8 eth_txbuf_temp[TX_DESCR_CNT][ETH_MTUSIZE+6], eth_rxfifo_temp[RX_DESCR_CNT][ETH_MTUSIZE+6];	//temporary
volatile u16 rxfifo_head=0, rxfifo_tail=0;
static ETH_DMADESCTypeDef DMATxDescTab[TX_DESCR_CNT];
static ETH_DMADESCTypeDef DMARxDescTab[RX_DESCR_CNT];


void udp_close(u16 idx)
{
  if(idx < UDP_ENTRIES)
  {
    udp_table[idx].status = UDP_CLOSED;
  }

  return;
}

u16 udp_open(u16 idx, MAC_Addr dst_mac, IP_Addr dst_ip, u16 dst_port, u16 src_port, u8 *data, u16 len)
{
  if(ethernet_link())
  {
    if(idx >= UDP_ENTRIES)
    {
      for(idx=0; idx<UDP_ENTRIES; idx++) //look for free table index
      {
        if(udp_table[idx].status == UDP_CLOSED)
        {
           break;
        }
      }
      if(idx >= UDP_ENTRIES)
      {
        DEBUGOUT("Eth: UDP table full\r\n");
        return UDP_ENTRIES;
      }
    }
  
    udp_table[idx].mac        = dst_mac;
    udp_table[idx].ip         = dst_ip;
    udp_table[idx].port       = dst_port;
    udp_table[idx].local_port = src_port;
    udp_table[idx].status     = UDP_OPENED;
    udp_table[idx].time       = 0;
  
    if(data)
    {
      memcpy(&eth_txbuf[UDP_DATASTART], data, len);
    }
    udp_send(idx, len);
  }

  return idx;
}

void udp_send(u16 idx, u16 len)
{
  if(idx < UDP_ENTRIES)
  {
    udp_table[idx].time = 0;

    make_udp_header(idx, len);
    ethernet_put(eth_txbuf, ETH_HEADERLEN+IP_HEADERLEN+UDP_HEADERLEN+len);
  }

  return;
}

void udp_app(u16 idx)
{
  IP_Header *rx_ip;
  UDP_Header *rx_udp;
  unsigned int len;

  rx_ip  = (IP_Header*)  &eth_rxbuf[IP_OFFSET];
  rx_udp = (UDP_Header*) &eth_rxbuf[UDP_OFFSET];

  len = swap16(rx_udp->len) - UDP_HEADERLEN;

  switch(udp_table[idx].local_port)
  {
    case DHCPCLIENT_PORT:
      if(udp_table[idx].port == DHCPSERVER_PORT) //only if server port is correct
      {
        dhcp_udpapp(idx, &eth_rxbuf[UDP_DATASTART], len, &eth_txbuf[UDP_DATASTART]);
      }
      break;
    case DNS_PORT:
      dns_udpapp(idx, &eth_rxbuf[UDP_DATASTART], len, &eth_txbuf[UDP_DATASTART]);
      break;
    case NBNS_PORT:
      if((rx_ip->src_ip&dev.netmask) == (dev.ip&dev.netmask)) //local net only
      {
        nbns_udpapp(idx, &eth_rxbuf[UDP_DATASTART], len, &eth_txbuf[UDP_DATASTART]);
      }
      break;
    case NTP_PORT:
      ntp_udpapp(idx, &eth_rxbuf[UDP_DATASTART], len, &eth_txbuf[UDP_DATASTART]);
      break;
    case SSDP_PORT:
      if((rx_ip->src_ip&dev.netmask) == (dev.ip&dev.netmask)) //local net only
      {
        ssdp_udpapp(idx, (char*)&eth_rxbuf[UDP_DATASTART], len, &eth_txbuf[UDP_DATASTART]);
      }
      break;
  }

  return;
}

void udp_service(void)
{
  ETH_Header *rx_eth;
  IP_Header *rx_ip;
  UDP_Header *rx_udp;
  u16 idx;

  rx_eth = (ETH_Header*) &eth_rxbuf[ETH_OFFSET];
  rx_ip  = (IP_Header*)  &eth_rxbuf[IP_OFFSET];
  rx_udp = (UDP_Header*) &eth_rxbuf[UDP_OFFSET];

  for(idx=0; idx<UDP_ENTRIES; idx++) //look for table index
  {
    if((udp_table[idx].status != UDP_CLOSED)             &&
       (rx_udp->src_port == swap16(udp_table[idx].port)) &&
       (rx_udp->dst_port == swap16(udp_table[idx].local_port)))
    {
      break;
    }
  }

  if(idx < UDP_ENTRIES) //connection in table
  {
    udp_table[idx].time = 0;
    udp_app(idx);
  }
  else  //new connection
  {
    for(idx=0; idx<UDP_ENTRIES; idx++) //look for free table index
    {
      if(udp_table[idx].status == UDP_CLOSED)
      {
         break;
      }
    }
    if(idx >= UDP_ENTRIES)
    {
      DEBUGOUT("Eth: UDP table full\r\n");
    }
    else
    {
      DEBUGOUT("Eth: (%i) UDP new %i -> %i (local)\r\n", idx, swap16(rx_udp->src_port), swap16(rx_udp->dst_port));
      udp_table[idx].mac        = rx_eth->src_mac;
      udp_table[idx].ip         = rx_ip->src_ip;
      udp_table[idx].port       = swap16(rx_udp->src_port);
      udp_table[idx].local_port = swap16(rx_udp->dst_port);
      udp_table[idx].status     = UDP_OPENED;
      udp_table[idx].time       = 0;
      udp_app(idx);
    }
  }

  return;
}

void tcp_abort(u16 idx)
{
  if(idx < TCP_ENTRIES)
  {
    tcp_table[idx].status = TCP_ABORT;
    tcp_table[idx].flags  = TCP_FLAG_RST;
  }

  return;
}

void tcp_close(u16 idx)
{
  if(idx < TCP_ENTRIES)
  {
    tcp_table[idx].status  = TCP_CLOSE;
    tcp_table[idx].flags  |= TCP_FLAG_FIN;
  }

  return;
}

u16 tcp_open(u16 idx, MAC_Addr dst_mac, IP_Addr dst_ip, u16 dst_port, u16 src_port)
{
#ifdef TCP_MSS
  TCP_Header *tx_tcp;
#endif

  if(ethernet_link())
  {
    if(idx >= TCP_ENTRIES)
    {
      for(idx=0; idx<TCP_ENTRIES; idx++)
      {
        if(tcp_table[idx].status == TCP_CLOSED) //empty entry found
        {
          break;
        }
      }
      if(idx >= TCP_ENTRIES)
      {
        DEBUGOUT("Eth: TCP table full\r\n");
        return TCP_ENTRIES;
      }
    }
  
    tcp_table[idx].mac        = dst_mac;
    tcp_table[idx].ip         = dst_ip;
    tcp_table[idx].port       = dst_port;
    tcp_table[idx].local_port = src_port;
    tcp_table[idx].acknum     = 0UL;
    tcp_table[idx].seqnum     = swap32(generate_id()); //1UL;
    tcp_table[idx].flags      = TCP_FLAG_SYN;
    tcp_table[idx].status     = TCP_OPEN;
    tcp_table[idx].time       = 0;
    tcp_table[idx].error      = 0;
  
#ifdef TCP_MSS
    tx_tcp = (TCP_Header*) &eth_txbuf[TCP_OFFSET];
    tx_tcp->options[0] = 0x02; //kind = 2 (Maximum Segment Size)
    tx_tcp->options[1] = 0x04; //len  = 4 bytes
    tx_tcp->options[2] = (SWAP16(TCP_MSS)>>0)&0xff;
    tx_tcp->options[3] = (SWAP16(TCP_MSS)>>8)&0xff;
    tcp_send(idx, 4, 4);
#else
    tcp_send(idx, 0, 0);
#endif
  }

  return idx;
}

void tcp_send(u16 idx, u16 len, u16 options)
{
  if(idx < TCP_ENTRIES)
  {
    tcp_table[idx].time = 0;

    make_tcp_header(idx, len, options);
    ethernet_put(eth_txbuf, ETH_HEADERLEN+IP_HEADERLEN+TCP_HEADERLEN+len);

    tcp_table[idx].seqnum += len-options;
  }

  return;
}

void tcp_app(u16 idx)
{
  IP_Header *rx_ip;
  TCP_Header *rx_tcp;
  u16 len, hd_len;

  rx_ip  = (IP_Header*)  &eth_rxbuf[IP_OFFSET];
  rx_tcp = (TCP_Header*) &eth_rxbuf[TCP_OFFSET];
  hd_len = (rx_ip->hd_len*4) + (rx_tcp->len*4);
  len    = swap16(rx_ip->len) - hd_len;

  switch(tcp_table[idx].local_port)
  {
    case SHOUTCAST_CLIENTPORT1:
    case SHOUTCAST_CLIENTPORT2:
    case SHOUTCAST_CLIENTPORT3:
      shoutcast_tcpapp(idx, &eth_rxbuf[ETH_HEADERLEN+hd_len], len, &eth_txbuf[TCP_DATASTART]);
      break;
    case RTSP_CLIENTPORT1:
    case RTSP_CLIENTPORT2:
    case RTSP_CLIENTPORT3:
      rtsp_tcpapp(idx, &eth_rxbuf[ETH_HEADERLEN+hd_len], len, &eth_txbuf[TCP_DATASTART]);
      break;
    case HTTP_SERVERPORT:
      //if((rx_ip->src_ip&dev.netmask) == (dev.ip&dev.netmask)) //local net only
      //{
        http_tcpapp(idx, (char*)&eth_rxbuf[ETH_HEADERLEN+hd_len], len, &eth_txbuf[TCP_DATASTART]);
      //}
      break;
    case UPNP_PORT:
      if((rx_ip->src_ip&dev.netmask) == (dev.ip&dev.netmask)) //local net only
      {
        upnp_tcpapp(idx, (char*)&eth_rxbuf[ETH_HEADERLEN+hd_len], len, &eth_txbuf[TCP_DATASTART]);
      }
      break;
  }
  return;
}

void tcp_service(void)
{
  ETH_Header *rx_eth;
  IP_Header *rx_ip;
  TCP_Header *rx_tcp, *tx_tcp;
  u16 idx, len, hd_len;

  rx_ip  = (IP_Header*)  &eth_rxbuf[IP_OFFSET];
  rx_tcp = (TCP_Header*) &eth_rxbuf[TCP_OFFSET];

  hd_len = (rx_ip->hd_len*4)+(rx_tcp->len*4);
  len    = swap16(rx_ip->len)-hd_len;

  for(idx=0; idx<TCP_ENTRIES; idx++) //look for table index
  {
    if((tcp_table[idx].status != TCP_CLOSED)             &&
       (rx_tcp->src_port == swap16(tcp_table[idx].port)) &&
       (rx_tcp->dst_port == swap16(tcp_table[idx].local_port)))
    {
      break;
    }
  }

  if(idx < TCP_ENTRIES) //connection in table
  {
    tcp_table[idx].time = 0; //reset connection timeout
    switch(tcp_table[idx].status)
    {
      case TCP_OPENED: //start app
        if(rx_tcp->flags&TCP_FLAG_RST) //abort connection
        {
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_OPENED -> RST -> TCP_CLOSED\r\n", idx);
        }
        else if(rx_tcp->flags&TCP_FLAG_FIN) //close connection
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len+1;
          tcp_table[idx].flags  = TCP_FLAG_ACK;
          tcp_send(idx, 0, 0);
          tcp_table[idx].status = TCP_FIN;
          DEBUGOUT("Eth: (%i) TCP_OPENED -> FIN -> send ACK -> TCP_FIN\r\n", idx);
        }
        else if(rx_tcp->flags&TCP_FLAG_SYN) //open connection
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len+1;
          tcp_table[idx].flags  = TCP_FLAG_SYN|TCP_FLAG_ACK;
#ifdef TCP_MSS
          tx_tcp = (TCP_Header*) &eth_txbuf[TCP_OFFSET];
          tx_tcp->options[0] = 0x02; //kind = 2 (Maximum Segment Size)
          tx_tcp->options[1] = 0x04; //len  = 4 bytes
          tx_tcp->options[2] = (SWAP16(TCP_MSS)>>0)&0xff;
          tx_tcp->options[3] = (SWAP16(TCP_MSS)>>8)&0xff;
          tcp_send(idx, 4, 4);
#else
          tcp_send(idx, 0, 0);
#endif
          tcp_table[idx].seqnum++;
          tcp_table[idx].status = TCP_OPENED;
          DEBUGOUT("Eth: (%i) TCP_OPENED -> SYN -> send SYN+ACK -> TCP_OPENED\r\n", idx);
        }
        else
        {
          if(swap32(rx_tcp->seqnum) != tcp_table[idx].acknum)
          {
            if(swap32(rx_tcp->seqnum) < tcp_table[idx].acknum) //dup frame -> send ack
            {
              unsigned long ack;
              ack = tcp_table[idx].acknum;
              tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len;
              tcp_table[idx].flags  = TCP_FLAG_ACK;
              tcp_send(idx, 0, 0);
              tcp_table[idx].acknum = ack;
              DEBUGOUT("Eth: (%i) TCP_OPENED seq<ack\r\n", idx);
            }
            else                                                 //frame lost -> send last ack
            {
              /*if(++tcp_table[idx].error > TCP_MAXERROR)
              {
                tcp_abort(idx);
                DEBUGOUT("Eth: (%i) TCP_OPENED seq>ack -> TCP_ABORT\n", idx);
              }
              else
              {*/
                tcp_table[idx].flags = TCP_FLAG_ACK;
                tcp_send(idx, 0, 0);
                DEBUGOUT("Eth: (%i) TCP_OPENED seq>ack\r\n", idx);
              //}
            }
          }
          else
          {
            tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len; //next seq nr
            tcp_table[idx].flags  = TCP_FLAG_ACK;
            tcp_table[idx].error  = 0;
            tcp_app(idx);
          }
        }
        break;

      case TCP_OPEN: //wait for SYN && ACK -> sent ACK
        if((rx_tcp->flags&TCP_FLAG_RST) || (rx_tcp->flags&TCP_FLAG_FIN))
        {
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_OPEN -> RST|FIN -> TCP_CLOSED\r\n", idx);
        }
        else if((rx_tcp->flags&TCP_FLAG_SYN) && 
                (rx_tcp->flags&TCP_FLAG_ACK)) //i am client -> ACK
        {
          DEBUGOUT("Eth: (%i) TCP_OPEN -> SYN+ACK -> send ACK -> TCP_OPENED\r\n", idx);
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len+1;
          tcp_table[idx].seqnum = swap32(rx_tcp->acknum);
          tcp_table[idx].flags  = TCP_FLAG_ACK;
          tcp_table[idx].status = TCP_OPENED;
          tcp_send(idx, 0, 0);
          tcp_app(idx);
        }
        else
        {
          DEBUGOUT("Eth: (%i) TCP_OPEN -> send RST -> TCP_CLOSED\r\n", idx);
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len;
          tcp_table[idx].seqnum = swap32(rx_tcp->acknum);
          tcp_table[idx].flags  = TCP_FLAG_RST;
          tcp_send(idx, 0, 0);
          tcp_table[idx].status = TCP_CLOSED;
        }
        break;

      case TCP_ABORT: //sent RST
        tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len;
        tcp_table[idx].seqnum = swap32(rx_tcp->acknum);
        tcp_table[idx].flags  = TCP_FLAG_RST;
        tcp_send(idx, 0, 0);
        tcp_table[idx].status = TCP_CLOSED;
        DEBUGOUT("Eth: (%i) TCP_ABORT -> send RST -> TCP_CLOSED\r\n", idx);
        break;

      case TCP_CLOSE: //sent FIN
        if(rx_tcp->flags&TCP_FLAG_RST)
        {
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_CLOSE -> RST -> TCP_CLOSED\r\n", idx);
        }
        else if(rx_tcp->flags&TCP_FLAG_FIN)
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len+1;
          tcp_table[idx].flags  = TCP_FLAG_ACK;
          tcp_send(idx, 0, 0);
          tcp_table[idx].status = TCP_FIN;
          DEBUGOUT("Eth: (%i) TCP_CLOSE -> FIN -> send ACK -> TCP_FIN\r\n", idx);
        }
        else //I am closing -> send FIN && ACK
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len;
          tcp_table[idx].flags  = TCP_FLAG_FIN|TCP_FLAG_ACK;
          tcp_send(idx, 0, 0);
          tcp_table[idx].seqnum++;
          tcp_table[idx].status = TCP_FIN;
          DEBUGOUT("Eth: (%i) TCP_CLOSE -> send FIN+ACK -> TCP_FIN\r\n", idx);
        }
        break;

      case TCP_FIN: //wait for FIN -> sent ACK
        if(rx_tcp->flags&TCP_FLAG_RST)
        {
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_FIN -> RST -> TCP_CLOSED\r\n", idx);
        }
        else if((rx_tcp->flags&TCP_FLAG_ACK) && 
                (tcp_table[idx].flags&TCP_FLAG_FIN)) //I am closing -> send FIN && ACK
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len;
          tcp_table[idx].flags  = TCP_FLAG_FIN|TCP_FLAG_ACK;
          tcp_send(idx, 0, 0);
          tcp_table[idx].seqnum++;
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_FIN -> ACK -> send FIN+ACK -> TCP_CLOSED\r\n", idx);
        }
        else if(rx_tcp->flags&TCP_FLAG_FIN)
        {
          tcp_table[idx].acknum = swap32(rx_tcp->seqnum)+len+1;
          tcp_table[idx].flags  = TCP_FLAG_ACK;
          tcp_send(idx, 0, 0);
          tcp_table[idx].status = TCP_CLOSED;
          DEBUGOUT("Eth: (%i) TCP_FIN -> FIN -> send ACK -> TCP_CLOSED\r\n", idx);
        }
        break;
    }
  }
  else //new connection
  {
    for(idx=0; idx<TCP_ENTRIES; idx++) //look for free table index
    {
      if(tcp_table[idx].status == TCP_CLOSED)
      {
         break;
      }
    }
    rx_eth = (ETH_Header*) &eth_rxbuf[ETH_OFFSET];

    if(idx >= TCP_ENTRIES)
    {
      DEBUGOUT("Eth: TCP table full\r\n");
    }
    else if(rx_tcp->flags&TCP_FLAG_SYN) //i am server -> SYN+ACK
    {
      http_close(idx); //close/reset index in http table 
      tcp_table[idx].mac        = rx_eth->src_mac;
      tcp_table[idx].ip         = rx_ip->src_ip;
      tcp_table[idx].port       = swap16(rx_tcp->src_port);
      tcp_table[idx].local_port = swap16(rx_tcp->dst_port);
      tcp_table[idx].acknum     = swap32(rx_tcp->seqnum)+len+1;
      tcp_table[idx].seqnum     = swap32(generate_id()); //1UL;
      tcp_table[idx].flags      = TCP_FLAG_SYN|TCP_FLAG_ACK;
      tcp_table[idx].status     = TCP_OPENED;
      tcp_table[idx].time       = 0;
      tcp_table[idx].error      = 0;
#ifdef TCP_MSS
      tx_tcp = (TCP_Header*) &eth_txbuf[TCP_OFFSET];
      tx_tcp->options[0] = 0x02; //kind = 2 (Maximum Segment Size)
      tx_tcp->options[1] = 0x04; //len  = 4 bytes
      tx_tcp->options[2] = (SWAP16(TCP_MSS)>>0)&0xff;
      tx_tcp->options[3] = (SWAP16(TCP_MSS)>>8)&0xff;
      tcp_send(idx, 4, 4);
#else
      tcp_send(idx, 0, 0);
#endif
      tcp_table[idx].seqnum++;
      DEBUGOUT("Eth: (%i) TCP new: %i -> %i\r\n", idx, tcp_table[idx].port, tcp_table[idx].local_port);
    }
    /*else if(rx_tcp->flags&TCP_FLAG_FIN) //close connetion
    {
      tcp_table[idx].mac        = rx_eth->src_mac;
      tcp_table[idx].ip         = rx_ip->src_ip;
      tcp_table[idx].port       = swap16(rx_tcp->src_port);
      tcp_table[idx].local_port = swap16(rx_tcp->dst_port);
      tcp_table[idx].acknum     = swap32(rx_tcp->seqnum)+len+1;
      tcp_table[idx].seqnum     = swap32(rx_tcp->acknum);
      tcp_table[idx].flags      = TCP_FLAG_FIN|TCP_FLAG_ACK;
      tcp_table[idx].status     = TCP_CLOSED;
      tcp_table[idx].time       = 0;
      tcp_table[idx].error      = 0;
      tcp_send(idx, 0, 0);
      DEBUGOUT("Eth: (%i) TCP new close: %i -> %i\n", idx, tcp_table[idx].port, tcp_table[idx].local_port);
    }*/
    else  //abort connetion
    {
      tcp_table[idx].mac        = rx_eth->src_mac;
      tcp_table[idx].ip         = rx_ip->src_ip;
      tcp_table[idx].port       = swap16(rx_tcp->src_port);
      tcp_table[idx].local_port = swap16(rx_tcp->dst_port);
      tcp_table[idx].acknum     = swap32(rx_tcp->seqnum)+len+1;
      tcp_table[idx].seqnum     = swap32(rx_tcp->acknum);
      tcp_table[idx].flags      = TCP_FLAG_RST|TCP_FLAG_ACK;
      tcp_table[idx].status     = TCP_CLOSED;
      tcp_table[idx].time       = 0;
      tcp_table[idx].error      = 0;
      tcp_send(idx, 0, 0);
      DEBUGOUT("Eth: (%i) TCP new abort: %i -> %i\r\n", idx, tcp_table[idx].port, tcp_table[idx].local_port);
    }
  }

  return;
}

void icmp_service(void)
{
  ETH_Header *rx_eth;
  IP_Header *rx_ip;
  ICMP_Header *rx_icmp, *tx_icmp;
  u16 len;

  rx_eth  = (ETH_Header*)  &eth_rxbuf[ETH_OFFSET];
  rx_ip   = (IP_Header*)   &eth_rxbuf[IP_OFFSET];
  rx_icmp = (ICMP_Header*) &eth_rxbuf[ICMP_OFFSET];
  tx_icmp = (ICMP_Header*) &eth_txbuf[ICMP_OFFSET];

  switch(rx_icmp->type)
  {
    case ICMP_ECHO_REQ:
      len = swap16(rx_ip->len)-IP_HEADERLEN-ICMP_HEADERLEN;
      make_ip_header(rx_eth->src_mac, rx_ip->src_ip, (IP_HEADERLEN+ICMP_HEADERLEN+len), IP_PROTO_ICMP);

      tx_icmp->type     = ICMP_ECHO_REP;
      tx_icmp->code     = 0x00;
      tx_icmp->checksum = SWAP16(0x0000);
      memcpy(&eth_txbuf[ICMP_DATASTART], &eth_rxbuf[ICMP_DATASTART], len);
      tx_icmp->checksum = checksum_ip((unsigned char*)tx_icmp, ICMP_HEADERLEN+len);

      ethernet_put(eth_txbuf, ETH_HEADERLEN+IP_HEADERLEN+ICMP_HEADERLEN+len);
      DEBUGOUT("Eth: Ping\r\n");
      break;
  }

  return;
}

void arp_request(IP_Addr ip)
{
  if((ip&dev.netmask) != (dev.ip&dev.netmask))
  {
    ip = dev.router;
  }

  make_arp_header(0ULL, 0ULL, ip, ARP_OP_REQUEST);
  ethernet_put(eth_txbuf, ETH_HEADERLEN+ARP_HEADERLEN);

  return;
}

MAC_Addr arp_getmac(IP_Addr ip)
{
  s32 timeout, timeout_arp;

  requested_mac = 0ULL;

  if(ethernet_link())
  {
    arp_request(ip);
  
    timeout     = getontime()+ETH_TIMEOUT;
    timeout_arp = getontime()+1;
    for(;;)
    {
      eth_service();
  
      if(requested_mac != 0ULL)
      {
        break;
      }
      if(getdeltatime(timeout_arp) > 0)
      {
        timeout_arp = getontime()+1;
        arp_request(ip);
      }
      if(getdeltatime(timeout) > 0)
      {
        break;
      }
    }
  }

  return requested_mac;
}

void arp_service(void)
{
  ETH_Header *rx_eth;
  ARP_Header *rx_arp/*, *tx_arp*/;

  rx_eth = (ETH_Header*) &eth_rxbuf[ETH_OFFSET];
  rx_arp = (ARP_Header*) &eth_rxbuf[ARP_OFFSET];
  //tx_arp = (ARP_Header*) &eth_txbuf[ARP_OFFSET];

  if((rx_arp->hw_type  == ARP_HW_TYPE)  &&
     (rx_arp->pro_type == ARP_PRO_TYPE) &&
     (rx_arp->hw_len   == ARP_HW_LEN)   &&
     (rx_arp->pro_len  == ARP_PRO_LEN)  &&
     (rx_arp->dst_ip   == dev.ip))
  {
    switch(rx_arp->op)
    {
      case ARP_OP_REQUEST: //arp request
        make_arp_header(rx_eth->src_mac, rx_arp->src_mac, rx_arp->src_ip, ARP_OP_REPLY);
        ethernet_put(eth_txbuf, ETH_HEADERLEN+ARP_HEADERLEN);
        break;

      case ARP_OP_REPLY: //arp reply
        requested_mac = rx_arp->src_mac;
        break;
    }
  }

  return;
}

void make_udp_header(u16 idx, u16 len)
{
  UDP_Header *tx_udp;

  tx_udp = (UDP_Header*) &eth_txbuf[UDP_OFFSET];

  make_ip_header(udp_table[idx].mac, udp_table[idx].ip, (IP_HEADERLEN+UDP_HEADERLEN+len), IP_PROTO_UDP);

  tx_udp->src_port = swap16(udp_table[idx].local_port);
  tx_udp->dst_port = swap16(udp_table[idx].port);
  tx_udp->len      = swap16(UDP_HEADERLEN+len);
  tx_udp->checksum = SWAP16(0x0000);

  return;
}

u16 checksum_tcp(unsigned char *s, u16 len, IP_Addr dst_ip)
{
  u32 sum=0;

  //include TCP Pseudo-Header
  sum += (uint16_t)(dev.ip);
  sum += (uint16_t)(dev.ip>>16);
  sum += (uint16_t)(dst_ip);
  sum += (uint16_t)(dst_ip>>16);
  sum += swap16(len);
  sum += SWAP16(IP_PROTO_TCP);

  for(; len > 1; len-=2)
  {
    sum += *((uint16_t*)s);
    s   += 2;
  }

  if(len) //add left-over byte
  {
    sum += *(unsigned char*)s;
  }

  while(sum>>16)
  {
    sum = (sum&0xFFFF)+(sum>>16);
  }

  return ~sum;
}

void make_tcp_header(u16 idx, u16 len, u16 options)
{
  TCP_Header *tx_tcp;

  tx_tcp = (TCP_Header*) &eth_txbuf[TCP_OFFSET];

  make_ip_header(tcp_table[idx].mac, tcp_table[idx].ip, (IP_HEADERLEN+TCP_HEADERLEN+len), IP_PROTO_TCP);

  tx_tcp->src_port = swap16(tcp_table[idx].local_port);
  tx_tcp->dst_port = swap16(tcp_table[idx].port);
  tx_tcp->seqnum   = swap32(tcp_table[idx].seqnum);
  if(tcp_table[idx].flags & TCP_FLAG_ACK)
  {
    tx_tcp->acknum = swap32(tcp_table[idx].acknum);
  }
  else
  {
    tx_tcp->acknum = 0UL;
  }
  tx_tcp->len      = (TCP_HEADERLEN+options)/4;
  tx_tcp->reserved = 0x00;
  tx_tcp->flags    = tcp_table[idx].flags;

  if(tcp_table[idx].local_port >= 1000) //station stream
  {
    u16 s, f;
    s = buf_size();
    f = buf_free();
    if      (f > s/2){ tx_tcp->window = SWAP16(TCP_WINDOW/1); }
    else if (f > s/4){ tx_tcp->window = SWAP16(TCP_WINDOW/2); }
    else if (f > s/8){ tx_tcp->window = SWAP16(TCP_WINDOW/4); }
    else             { tx_tcp->window = SWAP16(32);           }
  }
  else
  {
    tx_tcp->window = SWAP16(TCP_WINDOW);
  }

  tx_tcp->checksum = SWAP16(0x0000);
  tx_tcp->urgent   = SWAP16(0x0000);
  tx_tcp->checksum = checksum_tcp((unsigned char*)tx_tcp, TCP_HEADERLEN+len, tcp_table[idx].ip);

  return;
}

u16 checksum_ip(unsigned char *s, u16 len)
{
  u32 sum=0;

  for(; len > 1; len-=2)
  {
    sum += *((uint16_t*)s);
    s   += 2;
  }

  if(len) //add left-over byte
  {
    sum += *(unsigned char*)s;
  }

  while(sum>>16)
  {
    sum = (sum&0xFFFF)+(sum>>16);
  }

  return ~sum;
}

void make_ip_header(MAC_Addr dst_mac, IP_Addr dst_ip, u16 len, u16 proto)
{
  IP_Header *tx_ip;

  tx_ip = (IP_Header*) &eth_txbuf[IP_OFFSET];

  make_eth_header(dst_mac, ETH_TYPE_IP);

  tx_ip->ver      = 0x04; //v4
  tx_ip->hd_len   = IP_HEADERLEN/4;
#ifdef ETH_USE_DSCP
  tx_ip->tos      = 0xB8; //0xB8 = Expedited Forwarding (QoS: DSCP)
#else
  tx_ip->tos      = 0x00;
#endif
  tx_ip->len      = swap16(len);
  tx_ip->id       = SWAP16(0x0000);
  tx_ip->flg_offs = SWAP16(0x4000); //do not fragment
  tx_ip->ttl      = 128; //max. hops
  tx_ip->proto    = proto; //protocol
  tx_ip->checksum = SWAP16(0x0000);
  tx_ip->dst_ip   = dst_ip;
  tx_ip->src_ip   = dev.ip;

  tx_ip->checksum = checksum_ip((unsigned char*)tx_ip, IP_HEADERLEN);

  return;
}

void make_arp_header(MAC_Addr dst_mac, MAC_Addr arp_dst_mac, IP_Addr arp_dst_ip, u16 op)
{
  ARP_Header *tx_arp;

  tx_arp = (ARP_Header*) &eth_txbuf[ARP_OFFSET];

  make_eth_header(dst_mac, ETH_TYPE_ARP);

  tx_arp->hw_type  = ARP_HW_TYPE;
  tx_arp->pro_type = ARP_PRO_TYPE;
  tx_arp->hw_len   = ARP_HW_LEN;
  tx_arp->pro_len  = ARP_PRO_LEN;
  tx_arp->op       = op;
  tx_arp->src_mac  = dev.mac;
  tx_arp->src_ip   = dev.ip;
  tx_arp->dst_mac  = arp_dst_mac;
  tx_arp->dst_ip   = arp_dst_ip;

  return;
}

void make_eth_header(MAC_Addr dst_mac, u16 type)
{
  ETH_Header *tx_eth;

  tx_eth = (ETH_Header*) &eth_txbuf[ETH_OFFSET];

  if(dst_mac != 0ULL)
  {
    tx_eth->dst_mac = dst_mac;
  }
  else
  {
    tx_eth->dst_mac = 0xFFFFFFFFFFFFULL; //broadcast
  }

  tx_eth->src_mac = dev.mac;
  tx_eth->type    = type;

  return;
}

void eth_timerservice(void) //called every second
{
  u16 idx;
#ifdef TCP_MSS
  TCP_Header *tx_tcp;
#endif

  //clean up TCP table
  for(idx=0; idx<TCP_ENTRIES; idx++)
  {
    if(tcp_table[idx].status == TCP_CLOSED)
    {
      continue;
    }
    else if(++tcp_table[idx].time > TCP_TIMEOUT)
    {
      if(++tcp_table[idx].error > TCP_MAXERROR) //abort connection
      {
        tcp_table[idx].flags  = TCP_FLAG_RST;
        tcp_table[idx].status = TCP_CLOSED;
        tcp_send(idx, 0, 0);
      }
      else
      {
        tcp_table[idx].time = 0;
        switch(tcp_table[idx].status)
        {
          case TCP_OPENED: //send last ack
            tcp_table[idx].flags = TCP_FLAG_ACK;
            tcp_send(idx, 0, 0);
            DEBUGOUT("Eth: (%i) TCP_OPENED (timer)\r\n", idx);
            break;
          case TCP_OPEN: //send syn
            tcp_table[idx].flags = TCP_FLAG_SYN;
#ifdef TCP_MSS
            tx_tcp = (TCP_Header*) &eth_txbuf[TCP_OFFSET];
            tx_tcp->options[0] = 0x02; //kind = 2 (Maximum Segment Size)
            tx_tcp->options[1] = 0x04; //len  = 4 bytes
            tx_tcp->options[2] = (SWAP16(TCP_MSS)>>0)&0xff;
            tx_tcp->options[3] = (SWAP16(TCP_MSS)>>8)&0xff;
            tcp_send(idx, 4, 4);
#else
            tcp_send(idx, 0, 0);
#endif
            DEBUGOUT("Eth: (%i) TCP_OPEN (timer)\r\n", idx);
            break;
          case TCP_ABORT: //close connection
            tcp_table[idx].flags  = TCP_FLAG_RST;
            tcp_send(idx, 0, 0);
            tcp_table[idx].status = TCP_CLOSED;
            DEBUGOUT("Eth: (%i) TCP_ABORT (timer)\r\n", idx);
            break;
          case TCP_CLOSE: //send fin
            tcp_table[idx].flags  = TCP_FLAG_FIN|TCP_FLAG_ACK;
            tcp_send(idx, 0, 0);
            tcp_table[idx].seqnum++;
            tcp_table[idx].flags  = TCP_FLAG_RST;
            tcp_send(idx, 0, 0);
            tcp_table[idx].status = TCP_CLOSED;
            DEBUGOUT("Eth: (%i) TCP_CLOSE (timer)\r\n", idx);
            break;
          case TCP_FIN:
            tcp_table[idx].status = TCP_CLOSED;
            DEBUGOUT("Eth: (%i) TCP_FIN (timer)\r\n", idx);
            break;
        }
      }
    }
  }

  //clean up UDP table
  for(idx=0; idx<UDP_ENTRIES; idx++)
  {
    if(udp_table[idx].status == UDP_CLOSED)
    {
      continue;
    }
    else if(++udp_table[idx].time > UDP_TIMEOUT)
    {
      udp_table[idx].status = UDP_CLOSED; //close connection
    }
  }

  return;
}

void eth_service(void)
{
  u32 time;
  static long last_time=0L;
  ETH_Header *rx_eth;
  IP_Header *rx_ip;

  //run timer service ervery second
  time = getontime();
  if(time != last_time)
  {
    last_time = time;
    eth_timerservice();
  }

  //check for data
  if(eth_rxget() == 0)
  {
    return;
  }


  rx_eth = (ETH_Header*) &eth_rxbuf[ETH_OFFSET];
  rx_ip  = (IP_Header*) &eth_rxbuf[IP_OFFSET];
  switch(rx_eth->type)
  {
    case ETH_TYPE_IP:
      if((rx_ip->dst_ip == dev.ip)       || /*dst = dev*/
         (rx_ip->dst_ip == 0xFFFFFFFFUL) || /*dst = broadcast*/
         (((rx_ip->dst_ip&dev.netmask) == (dev.ip&dev.netmask)) && ((rx_ip->dst_ip&(~dev.netmask)) == (~dev.netmask))) || /*dst = broadcast local*/
         (dev.ip == 0UL)) /*dev = 0 (during DHCP)*/
      {
        switch(rx_ip->proto)
        {
          case IP_PROTO_ICMP: icmp_service(); break;
          case IP_PROTO_TCP:  tcp_service();  break;
          case IP_PROTO_UDP:  udp_service();  break;
        }
      }
      else if( ((rx_eth->dst_mac&0x000080FFFFFFULL)      == SWAP64(0x01005E0000000000ULL)) &&
               ((rx_ip->dst_ip&0xF0UL)                   == 0xE0UL) &&
              (((rx_eth->dst_mac&0xFFFF7F000000ULL)>>16) == (rx_ip->dst_ip&0xFFFF7F00UL))) //multicast
      {
        switch(rx_ip->proto)
        {
          //case IP_PROTO_ICMP: icmp_service(); break;
          //case IP_PROTO_TCP:  tcp_service();  break;
          case IP_PROTO_UDP:  udp_service();  break;
        }
      }
      break;

    case ETH_TYPE_ARP:
      arp_service();
      break;
  }

  return;
}

void eth_setname(char *name) //set NB & UPnP name (upper case)
{
  u16 i, len;

  len = strlen(name);
  if(len == 0)
  {
    return;
  }
  else if(len >= 16)
  {
    len = 15;
  }

  for(i=0; i<len; i++)
  {
    dev.name[i] = toupper(name[i]); //copy name and convert to uppercase
  }
  dev.name[i] = 0;

  return;
}


void         eth_setsummer(s16 on)          { dev.summer   = on; }
void         eth_settimediffh(s16 hours)    { dev.timediff = hours*3600; }
void         eth_settimediff(s16 seconds)   { dev.timediff = seconds; }
void         eth_setntp(IP_Addr ntp)        { dev.ntp      = ntp; }
void         eth_setdns(IP_Addr dns)        { dev.dns      = dns; }
u16          eth_setdhcp(int on)            { dev.dhcp     = on; 
                                              if(on){ return dhcp_getcfg(); }
                                              else  { return 1; } }

void         eth_setrouter(IP_Addr r)       { dev.router   = r; }
void         eth_setnetmask(IP_Addr nm)     { dev.netmask  = nm; }
void         eth_setip(IP_Addr ip)          { dev.ip       = ip; }
void         eth_setmac(MAC_Addr mac)       { dev.mac      = mac; ethernet_setmac(mac); }

char*        eth_getname(void)              { return dev.name; }
s16          eth_getsummer(void)            { return dev.summer; }
s16          eth_gettimediffh(void)         { return dev.timediff/3600; }
s16          eth_gettimediff(void)          { return dev.timediff; }
IP_Addr      eth_getntp(void)               { return dev.ntp; }
IP_Addr      eth_getdns(void)               { return dev.dns; }
s16          eth_getdhcp(void)              { return dev.dhcp; }
IP_Addr      eth_getrouter(void)            { return dev.router; }
IP_Addr      eth_getnetmask(void)           { return dev.netmask; }
IP_Addr      eth_getip(void)                { return dev.ip; }
MAC_Addr     eth_getmac(void)               { return dev.mac; }

u16 eth_rxget(void)
{
  u16 head, tail;

  head = rxfifo_head;
  tail = rxfifo_tail;
  if(head != tail)
  {
    eth_rxbuf = eth_rxfifo[tail];
    if(++tail >= ETH_RXFIFO)
    {
      tail = 0;
    }
    rxfifo_tail = tail;
    return 1;
  }

  return 0;
}

u16 eth_rxput(void)
{
  u16 head;

  head = rxfifo_head;
  if(ETH_HandleRxPkt(eth_rxfifo[head]) != 0)
  {
    if(++head >= ETH_RXFIFO)
    {
      head = 0;
    }
    rxfifo_head = head;
    return 1;
  }

  return 0;
}

void eth_init(void)
{
  DEBUGOUT("Eth: init\r\n");

  requested_mac = 0UL;
  memset(&dev, 0, sizeof(dev));
  memset(&tcp_table, 0, sizeof(tcp_table));
  memset(&udp_table, 0, sizeof(udp_table));
  memset(&eth_txbuf, 0, sizeof(eth_txbuf));
  memset(&eth_rxfifo, 0, sizeof(eth_rxfifo));
  eth_rxbuf   = eth_rxfifo[0];
  rxfifo_head = 0;
  rxfifo_tail = 0;

  eth_setname(APPNAME);
	//eth_setmac(atomac(DEFAULT_MAC));

  return;
}

void xv_eth_init_ethernet_init(void)
{
	u8 i = 100U;
	ETH_InitTypeDef ETH_InitStructure;
	
	/* Hard reset PHY */
	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET);	/* Reset */
	while(i--);
	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);		/* Release */
	
	ETH_DeInit();
	/* Software reset */
  ETH_SoftwareReset();
  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);
	
	ETH_StructInit(&ETH_InitStructure);
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
	//ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Enable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
	
	
	ETH_Init(&ETH_InitStructure, PHY_ADDR);
	
	eth_setmac(atomac(DEFAULT_MAC));
	//ethernet_setmac(0x0000000123456789);
	
	ETH_DMATxDescChainInit(&DMATxDescTab[0], eth_txbuf_temp[0], TX_DESCR_CNT);
	//ETH_DMATxDescChainInit(&DMATxDescTab, eth_txbuf_temp/*packet[0]*/, 1);
	ETH_DMARxDescChainInit(&DMARxDescTab[0], eth_rxfifo_temp[0], RX_DESCR_CNT);
	
	
	/* Enable Ethernet Rx interrrupt */
  for(i=0; i<RX_DESCR_CNT; i++)
  {
		ETH_SetDMARxDescOwnBit(&DMARxDescTab[i]);
    ETH_DMARxDescReceiveITConfig(&DMARxDescTab[i], ENABLE);
		ETH_DMARxDescSecondAddressChainedCmd(&DMARxDescTab[i], ENABLE);
  }
	for (i=0; i<TX_DESCR_CNT; i++)
	{
		//ETH_DMATxDescTransmitITConfig(&DMATxDescTab[i], ENABLE);
		ETH_DMATxDescSecondAddressChainedCmd(&DMATxDescTab[i], ENABLE);
		ETH_DMATxDescCRCCmd(&DMATxDescTab[i], ENABLE);
		//ETH_SetDMATxDescOwnBit(&DMATxDescTab[i]);
	}
	
	//ETH_DMATxDescChecksumInsertionConfig(&DMATxDescTab, ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	//ETH_DMATxDescShortFramePaddingCmd(&DMATxDescTab, ENABLE);

	
	/*ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_AIS| ETH_DMA_IT_ER | ETH_DMA_IT_FBE
									| ETH_DMA_IT_ET | ETH_DMA_IT_RWT | ETH_DMA_IT_RPS | ETH_DMA_IT_RBU
									| ETH_DMA_IT_R | ETH_DMA_IT_TU | ETH_DMA_IT_RO | ETH_DMA_IT_TJT
									| ETH_DMA_IT_TBU | ETH_DMA_IT_TPS | ETH_DMA_IT_T, ENABLE);*/
	ETH_DMAITConfig(ETH_DMA_IT_NIS /*| ETH_DMA_IT_ER */| ETH_DMA_IT_R /*| ETH_DMA_IT_TBU | ETH_DMA_IT_T*/, ENABLE);
	
	ETH_Start();
}
