#ifndef _UTILS_H_
#define _UTILS_H_		1U

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "stm32f2xx.h"
#include "eth.h"
#include "tools.h"

//----- DEFINES -----
#define SWAP16(x)                      ((((x)&0x00FF)<<8)| \
                                        (((x)&0xFF00)>>8))

#define SWAP32(x)                      ((((x)&0xFF000000UL)>>24)| \
                                        (((x)&0x00FF0000UL)>> 8)| \
                                        (((x)&0x0000FF00UL)<< 8)| \
                                        (((x)&0x000000FFUL)<<24))

#define SWAP64(x)                      ((((x)&0xFF00000000000000ULL)>>56)| \
                                        (((x)&0x00FF000000000000ULL)>>40)| \
                                        (((x)&0x0000FF0000000000ULL)>>24)| \
                                        (((x)&0x000000FF00000000ULL)>> 8)| \
                                        (((x)&0x00000000FF000000ULL)<< 8)| \
                                        (((x)&0x0000000000FF0000ULL)<<24)| \
                                        (((x)&0x000000000000FF00ULL)<<40)| \
                                        (((x)&0x00000000000000FFULL)<<56))
																				
#define MULTICAST_MAC(x)               (SWAP64(0x01005E0000000000ULL)|((x&0xFFFF7F00ULL)<<16ULL))

u16 base64_test(char c);
u16 base64_decode(unsigned char *dst, const unsigned char *src, u16 len);
u16 uuid_test(char *uuid);
void uuid_generate(char *uuid);
u16 nbns_decode(char *dst, const char *src);
void nbns_encode(char *dst, const char *src, u16 type);

char* mactoa(MAC_Addr mac_addr);
MAC_Addr atomac(char *s);
char* iptoa(IP_Addr ip_addr);
IP_Addr atoip(char *s);

u16 url_decode(char *dst, const char *src, u16 len);
char* http_skiphd(const char *src, u16 *len);
u16 http_hdparamcontentlen(const char *src);
u16 http_hdparam(char *dst, size_t dst_len, const char *src, const char *param);
u16 http_response(const char *src);
u32 generate_id(void);

void atoaddr(char *s, char *proto, char *user, char *pwrd, char *host, u16 *port, char *file);
void freqtostr(u16 freq, char *freq_str);

uint64_t                     swap64(uint64_t i);
u32                          swap32(u32 i);
u16                          swap16(u16 i);
#endif	/* _UTILS_H_ */
