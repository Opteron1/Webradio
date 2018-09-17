#ifndef _RTC_H_
#define _RTC_H_		1U

#include "stm32f2xx.h"
#include "stm32f2xx_rtc.h"
#include "tools.h"

void rtc_settime(TIME timedate);
TIME rtc_gettime(void);


#endif /* _RTC_H_ */
