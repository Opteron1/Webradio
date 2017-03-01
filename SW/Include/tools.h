#ifndef _TOOLS_H_
#define _TOOLS_H_		1U

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "stm32f2xx.h"
#include "TFT.h"


//----- DEFINES -----
typedef struct
{
  unsigned int s;     //0-59
  unsigned int m;     //0-59
  unsigned int h;     //0-23
  unsigned int day;   //1-31
  unsigned int month; //1-12
  unsigned int year;  //1970-...
  unsigned int wday;  //0-6 (0=Sunday)
} TIME;


void strshrinkpath(char *path);
char* strrmvspace(char *dst, const char *src);
char* strtoupper(char *dst, const char *src);

void							sectotime(u32 s, TIME *t);
s16								strstrk(char *dst, const char *src, const char *key);
const char*       strstri(const char *s, const char *pattern);

#ifndef strncmpi
# define TOOLS_STRNCMPI
s16                                    strncmpi(const char *s1, const char *s2, size_t n);
#endif
#ifndef itoa
# define TOOLS_ITOA
char*											itoa					(int val, char *buf, int radix);
#endif

#ifndef utoa
# define TOOLS_UTOA
char*                     utoa					(unsigned val, char *buf, int radix);
#endif

#ifndef ltoa
# define TOOLS_LTOA
char*                     ltoa					(long val, char *buf, int radix);
#endif

#ifndef ultoa
# define TOOLS_ULTOA
char*                     ultoa					(unsigned long val, char *buf, int radix);
#endif

u16 atou_hex(const char *s);
u16 atou(const char *s);
u16 atorgb(const char *s);
void daytime(char *s, TIME *t);
#endif //_TOOLS_H_
