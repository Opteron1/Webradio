#ifndef _ESP_DEBUG_
#define _ESP_DEBUG_

#define DEBUG_BUFFER_SIZE   100U                    //Debug buffer size
#define DEBUG               1U                      //Enable debug messages

#define COMPILE_YEAR  ((((__DATE__ [7]-'0')*10+(__DATE__[8]-'0'))*10+(__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))

#define COMPILE_MONTH ( (__DATE__[2] == 'n') ? (__DATE__[1] == 'a' ? 1 : 6) \
                      : (__DATE__[2] == 'b') ? 2  \
                      : (__DATE__[2] == 'r') ? (__DATE__[1] == 'a' ? 3 : 4) \
                      : (__DATE__[2] == 'y') ? 5  \
                      : (__DATE__[2] == 'l') ? 7  \
                      : (__DATE__[2] == 'g') ? 8  \
                      : (__DATE__[2] == 'p') ? 9  \
                      : (__DATE__[2] == 't') ? 10 \
                      : (__DATE__[2] == 'v') ? 11 : 12)

#define COMPILE_DAY   ((((__DATE__ [4]==' ')?0:(__DATE__[4]-'0'))*10)+(__DATE__[5]-'0'))


char* dbgprint(const char* format, ...);


#endif	//_ESP_DEBUG_