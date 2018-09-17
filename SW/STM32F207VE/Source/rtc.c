#include "rtc.h"

void rtc_settime(TIME timedate)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	ErrorStatus error = ERROR;
	
	time.RTC_Seconds = timedate.s;
	time.RTC_Minutes = timedate.m;
	time.RTC_Hours   = timedate.h;
	
	date.RTC_Date    = timedate.day;
	date.RTC_Month   = timedate.month;
	date.RTC_WeekDay = timedate.wday;
	date.RTC_Year    = timedate.year - 2000U;
	
	
	error  = RTC_SetTime(RTC_Format_BIN, &time);
	error |= RTC_SetDate(RTC_Format_BIN, &date);
}

TIME rtc_gettime(void)
{
	TIME timedate;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	
	RTC_GetTime(RTC_Format_BIN, &time);
	RTC_GetDate(RTC_Format_BIN, &date);
	
	timedate.s = time.RTC_Seconds;
	timedate.m = time.RTC_Minutes;
	timedate.h = time.RTC_Hours;
	
	timedate.day   = date.RTC_Date;
	timedate.month = date.RTC_Month;
	timedate.wday  = date.RTC_WeekDay;
	timedate.year  = date.RTC_Year + 2000U;
	
	return (timedate);
}
