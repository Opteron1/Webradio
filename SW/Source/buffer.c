#include "buffer.h"


BUFFER gbuf;


void buf_service(void)
{
  u16 len;
	/*
  if(fm_size())
  {
    len = fm_len();
    while(len)
    {
      if(vs_buffree() < 32)
      {
        break;
      }
      if(len < 32)
      {
        fm_gets(gbuf.card.buf, len);
        vs_bufputs(gbuf.card.buf, len);
        break;
      }
      else
      {
        fm_gets(gbuf.card.buf, 32);
        vs_bufputs(gbuf.card.buf, 32);
        len -= 32;
      }
    }
  }
	*/

  return;
}

void buf_puts(const unsigned char *s, u16 len)
{
	/*
  if(fm_size())
  {
    return fm_puts(s, len);
  }
	*/
  vs_bufputs(s, len);
	return;
}

u16 buf_size(void)
{
	/*
  if(fm_size())
  {
    return fm_size();
  }
	*/
  return VS_BUFSIZE;
}

u16 buf_free(void)
{
  /*
	if(fm_size())
  {
    return fm_free();
  }
	*/
  return vs_buffree();
}

u16 buf_len(void)
{
  u16 len;

  len = vs_buflen();
	/*
  if(fm_size())
  {
    len += fm_len();
  }
	*/
  return len;
}

void buf_reset(void)
{
  //fm_reset();
  vs_bufreset();

  return;
}
