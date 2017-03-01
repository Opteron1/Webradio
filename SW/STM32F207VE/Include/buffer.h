#ifndef _BUFFER_H_
#define _BUFFER_H_		1U

#include "stm32f2xx.h"
#include "ff.h"
#include "diskio.h"
#include "eth.h"
#include "const.h"
#include "vs.h"



//----- DEFINES -----
#define CARD_READBUF                   (1024)   //bytes

typedef struct
{
  char name[MAX_NAME];
  char info[MAX_INFO];
  char file[MAX_ADDR];
} MENUBUFFER;

typedef struct
{
  char         name[MAX_NAME];
  char         info[MAX_INFO];
  char         addr[MAX_ADDR];
  char         host[MAX_ADDR];
  MAC_Addr     mac;
  IP_Addr      ip;
  u16					 port;
  char         file[MAX_URLFILE];
} STATIONBUFFER;

typedef struct
{
  char name[MAX_NAME];
  char info[MAX_INFO];
  char file[MAX_ADDR];
} SHAREBUFFER;

typedef struct
{
  char          name[MAX_NAME];
  char          info[MAX_INFO];
  char          file[MAX_ADDR];
  FIL           fsrc;
  unsigned char buf[CARD_READBUF];
} CARDBUFFER;

typedef struct
{
	char					name[MAX_NAME];
	char					frequency[MAX_FREQ];
} TUNERBUFFER;

typedef union
{
  MENUBUFFER    menu;
  STATIONBUFFER station;
  SHAREBUFFER   share;
  CARDBUFFER    card;
	TUNERBUFFER		tuner;
} BUFFER;


//----- GLOBALS -----
extern BUFFER gbuf;

void buf_service(void);

void buf_puts(const unsigned char *s, u16 len);
u16 buf_size(void);
u16 buf_free(void);
u16 buf_len(void);
void buf_reset(void);

#endif	/* _BUFFER_H_ */
