#include "tools.h"


void strshrinkpath(char *path)
{
  u16 i;

  for(i=strlen(path); i!=0; i--)
  {
    if(path[i] == '/')
    {
      break;
    }
  }
  path[i] = 0;

  return;
}

char* strrmvspace(char *dst, const char *src)
{
  u16 i;

  if(*src == 0)
  {
    *dst = 0;
  }
  else
  {
    //at start
    for(i=0; isspace(src[i]); i++);
    strcpy(dst, &src[i]);
  
    //at end
    i=strlen(dst)-1;
    for(i=strlen(dst)-1; isspace(dst[i]); i--)
    {
      dst[i] = 0;
    }
  }

  return dst;
}

char* strtoupper(char *dst, const char *src)
{
  char c;

  while(*src)
  {
    c    = *src++;
    *dst++ = toupper(c);
  }
  *dst = 0;

  return dst;
}

void sectotime(u32 s, TIME *t) //s = seconds from 1970
{
  struct tm *t_struct;
	t_struct = localtime((time_t*)&s);

  t->s     = t_struct->tm_sec;
  t->m     = t_struct->tm_min;
  t->h     = t_struct->tm_hour;
  t->day   = t_struct->tm_mday;
  t->month = t_struct->tm_mon + 1;
  t->year  = t_struct->tm_year + 1900;
  t->wday  = t_struct->tm_wday;

  return;
}

s16 strstrk(char *dst, const char *src, const char *key) //key = "first\0second\third\0\0"
{
  int fnd=0;
  const char *k, *s;
  char c;

  *dst = 0;

  s = src;
  k = key;
  for(;;)
  {
    s = strstri(s, k);
    if(s != 0)
    {
      s += strlen(k);
      while(*s && (*s ==  ' ')){ s++; } //skip spaces
      while(*k){ k++; } k++; //get next key
      if(*k == 0) //last key
      {
        while(*s)
        {
          c = *s++;
          if((c == '<')  ||
             (c == '>')  ||
             (isspace(c)))
          {
            break;
          }
          else
          {
            *dst++ = c;
          }
        }
        *dst = 0;
        fnd  = 1;
        break;
      }
    }
    else
    {
      break;
    }
  }

  return fnd;
}

const char* strstri(const char *s, const char *pattern)
{
  size_t len;

  if(*pattern)
  {
    len = strlen(pattern);
    while(*s)
    {
      if(strncmpi(s, pattern, len) == 0)
      {
        return s;
      }
      s++;
    }
  }

  return 0;
}

#ifdef TOOLS_STRNCMPI
s16 strncmpi(const char *s1, const char *s2, size_t n)
{
  unsigned char c1, c2;

  if(n == 0)
  {
    return 0;
  }
  
  do
  {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if((c1 == 0) || (c2 == 0))
    {
      break;
    }
  }
  while((c1 == c2) && --n);
  
  return c1-c2;
}
#endif

#ifdef TOOLS_ITOA
char* itoa(int val, char *buf, int radix)
{
  char *p;
  unsigned u;

  p = buf;
  if(radix == 0)
  {
    radix = 10;
  }
  if(buf == NULL)
  {
    return NULL;
  }
  if(val < 0)
  {
    *p++ = '-';
    u = -val;
  }
  else
  {
    u = val;
  }
  utoa(u, p, radix);

  return buf;
}
#endif


#ifdef TOOLS_UTOA
char* utoa(unsigned val, char *buf, int radix)
{
  char *s, *p;
  s = "0123456789abcdefghijklmnopqrstuvwxyz";

  if(radix == 0)
  {
    radix = 10;
  }
  if(buf == NULL)
  {
    return NULL;
  }
  if(val < (unsigned)radix)
  {
     buf[0] = s[val];
     buf[1] = '\0';
   }
   else
   {
     for(p = utoa(val / ((unsigned)radix), buf, radix); *p; p++);
     utoa(val % ((unsigned)radix), p, radix);
  }
  return buf;
}
#endif


#ifdef TOOLS_LTOA
char* ltoa(long val, char *buf, int radix)
{
  char *p;
  unsigned long u;

  p = buf;
  if(radix == 0)
  {
    radix = 10;
  }
  if(buf == NULL)
  {
    return NULL;
  }
  if(val < 0)
  {
    *p++ = '-';
    u = -val;
  }
  else
  {
    u = val;
  }
  ultoa(u, p, radix);

  return buf;
}
#endif


#ifdef TOOLS_ULTOA
char* ultoa(unsigned long val, char *buf, int radix)
{
  char *s, *p;
  s = "0123456789abcdefghijklmnopqrstuvwxyz";

  if(radix == 0)
  {
    radix = 10;
  }
  if(buf == NULL)
  {
    return NULL;
  }
  if(val < (unsigned)radix)
  {
     buf[0] = s[val];
     buf[1] = '\0';
   }
   else
   {
     for(p = ultoa(val / ((unsigned)radix), buf, radix); *p; p++);
     ultoa(val % ((unsigned)radix), p, radix);
  }
  return buf;
}
#endif

u16 atou_hex(const char *s)
{
  unsigned int value=0;

  if(!s)
  {
    return 0;
  }  

  while(*s)
  {
    if((*s >= '0') && (*s <= '9'))
    {
      value = (value*16) + (*s-'0');
    }
    else if((*s >= 'A') && ( *s <= 'F'))
    {
      value = (value*16) + (*s-'A'+10);
    }
    else if((*s >= 'a') && (*s <= 'f'))
    {
      value = (value*16) + (*s-'a'+10);
    }
    else
    {
      break;
    }
    s++;
  }

  return value;
}

u16 atou(const char *s)
{
  u16 value=0;

  if(!s)
  {
    return 0;
  }  

  while(*s)
  {
    if((*s >= '0') && (*s <= '9'))
    {
      value = (value*10) + (*s-'0');
    }
    else
    {
      break;
    }
    s++;
  }

  return value;
}

u16 atorgb(const char *s)
{
  unsigned int r, g, b;

  while(!isdigit(*s)){ s++; };
  r = atoi(s);
  while(isdigit(*s)){ s++; }; while(!isdigit(*s)){ s++; };
  g = atoi(s);
  while(isdigit(*s)){ s++; }; while(!isdigit(*s)){ s++; };
  b = atoi(s);

  return RGB(r,g,b);
}

void daytime(char *s, TIME *t)
{
  if((t->h >= 19) || (t->h <= 4))
  {
    strcpy(s, "Good Night !");
  }
  else if(t->h >= 17)
  {
    strcpy(s, "Good Evening !");
  }
  else if(t->h >= 12)
  {
    strcpy(s, "Good Afternoon !");
  }
  else if(t->h >= 10)
  {
    strcpy(s, "Good Day !");
  }
  else if(t->h >= 5)
  {
    strcpy(s, "Good Morning !");
  }

  return;
}
