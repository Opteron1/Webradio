#include "debug.h"

void debug_out(const char *s, ...)
{
#ifdef DEBUG
  unsigned int i, move;
  char c, str[16], *ptr;
  va_list ap;

  va_start(ap, s);

  for(;;)
  {
    c = *s++;

    if(c == 0)
    {
      break;
    }
    else if(c == '%')
    {
      c = *s++;
      if(isdigit(c) > 0)
      {
        move = c-'0';
        c = *s++;
      }
      else
      {
        move = 0;
      }

      switch(c)
      {
        case 's':
          ptr = va_arg(ap, char *);
          v_uart_puts(ptr);
          break;
        case 'b': //bin
          ltoa(va_arg(ap, long), str, 2);
          if(move)
          {
            for(i=0; str[i]; i++);
            for(; move>i; move--)
            {
              v_uart_putc('0');
            }
          }
          v_uart_puts(str);
          break;
        case 'i': //dec
          ltoa(va_arg(ap, long), str, 10);
          if(move)
          {
            for(i=0; str[i]; i++);
            for(; move>i; move--)
            {
              v_uart_putc('0');
            }
          }
          v_uart_puts(str);
          break;
        case 'u': //unsigned dec
          ultoa(va_arg(ap, unsigned long), str, 10);
          if(move)
          {
            for(i=0; str[i]; i++);
            for(; move>i; move--)
            {
              v_uart_putc('0');
            }
          }
          v_uart_puts(str);
          break;
        case 'x': //hex
          ltoa(va_arg(ap, long), str, 16);
          if(move)
          {
            for(i=0; str[i]; i++);
            for(; move>i; move--)
            {
              v_uart_putc('0');
            }
          }
          v_uart_puts(str);
          break;
      }
    }
    else
    {
      v_uart_putc(c);
    }
  }
  va_end(ap);
#endif
  return;
}

static void v_uart_puts(const char *s)
{
  while(*s)
  {
    v_uart_putc(*s++);
  }
  return;
}

static void v_uart_putc(unsigned int c)
{
  USART_SendData(USART6, c);
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) != SET);
	
  return;
}
