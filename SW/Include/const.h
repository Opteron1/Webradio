#ifndef _CONST_H_
#define _CONST_H_	1U

#include "debug.h"

//----- DEFINES -----
//Application settings
#define VERSION                        "0.10"
#define RELEASE                        //Release version
#define APPNAME                        "WebRadio" //max 15 characters
#if defined DEBUG
# define APPVERSION                    VERSION"a"
#elif defined RELEASE
# define APPVERSION                    VERSION
#else
# define APPVERSION                    VERSION"*"
#endif

//Max characters
#define MAX_NAME                       (32)  // 31 chars + zero (min 32)  "Station Name"
#define MAX_INFO                       (100) // 99 chars + zero (min 32)  "Station info"
#define MAX_ADDR                       (260) //259 chars + zero           "http://192.168.0.100/stream.mp3" or "/test/abc/xyz.mp3"
#define MAX_URLFILE                    (50)  // 49 chars + zero           "/stream.mp3"
#define MAX_FREQ											 (10)	 //  9 chars + zero						"108.0 MHz"


typedef uint64_t MAC_Addr;
typedef u32 IP_Addr;

#endif	/* _CONST_H_ */
