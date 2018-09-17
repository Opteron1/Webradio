#ifndef _HTTP_FILES_H_
#define _HTTP_FILES_H_

#include "stm32f2xx.h"

//----- DEFINES -----
#define UNKNOWN_FILE                   (0)
#define HTML_FILE                      (1)
#define XML_FILE                       (2)
#define JS_FILE                        (3)
#define CSS_FILE                       (4)
#define TXT_FILE                       (5)
#define ICON_FILE                      (6)
#define GIF_FILE                       (7)
#define JPEG_FILE                      (8)


typedef struct
{
  const char *name;
  const u16 type;
  const unsigned char *data;
  const u16 len;
} HTTPFILE;

typedef struct
{
  const char *name; //name
  const u16 format; //format
  void*(*get)(void); 
} HTTPVAR;


//----- PROTOTYPES -----
unsigned int                  http_printf(char *dst, unsigned int format, unsigned int param, ...);
u16                           http_fparse(char *dst, u16 file, u16 *start, u16 len, u16 param);
unsigned int                  http_fdata(unsigned char *dst, unsigned int file, unsigned int start, unsigned int len);
unsigned int                  http_flen(unsigned int file, unsigned int param);
unsigned int                  http_ftype(unsigned int file);
u16                           http_fid(const char *name, u16 *param);


#endif //_HTTP_FILES_H_
