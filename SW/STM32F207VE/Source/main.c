#include "main.h"
#include "eth/ntp.h"
#include "chucknorris.h"
#include "fm_tuner.h"
#include "wifi.h"


volatile u16 status = 0, standby_active = 0;
volatile s32 on_time = 0;
volatile u32 sec_time = 0;
TIME timedate;
char date_str[14] = {'T','h',' ','0','1','.','0','1','.','1','9','7','0',0}; //Th 01.01.1970
char clock_str[9] = {'0','0',':','0','0',':','0','0',0}; //00:00:00

const char day_tab[7][3] = 
{
  {"Su"},{"Mo"},{"Tu"},{"We"},{"Th"},{"Fr"},{"Sa"} //English
  //{"So"},{"Mo"},{"Di"},{"Mi"},{"Do"},{"Fr"},{"Sa"} //German
};

const char clock_tab[60][3] = 
{
  {"00"},{"01"},{"02"},{"03"},{"04"},{"05"},{"06"},{"07"},{"08"},{"09"},
  {"10"},{"11"},{"12"},{"13"},{"14"},{"15"},{"16"},{"17"},{"18"},{"19"},
  {"20"},{"21"},{"22"},{"23"},{"24"},{"25"},{"26"},{"27"},{"28"},{"29"},
  {"30"},{"31"},{"32"},{"33"},{"34"},{"35"},{"36"},{"37"},{"38"},{"39"},
  {"40"},{"41"},{"42"},{"43"},{"44"},{"45"},{"46"},{"47"},{"48"},{"49"},
  {"50"},{"51"},{"52"},{"53"},{"54"},{"55"},{"56"},{"57"},{"58"},{"59"}
};

u8 Data = 0;
void SysTick_Handler(void)	//100Hz
{
	static u32 sec = 1;
	static u8 secdiv10 = 1;
  u16 s, a;
	
	disk_timerproc();
	keys_timerservice();
	
	//10 Hz
	if(--secdiv10 == 0)
	{
		fm_tuner_timerservice();
#warning COMMENT - zde pridat funkci pro spousteni ADC pro mereni spektra FHT
		
		secdiv10 = 10;
	}
	//1 Hz
  if(--sec == 0)
  {
    sec = 100;
    on_time++;
    sec_time++;

    s  = status;
    s |= SEC_CHANGED;
		
    if(++timedate.s == 60)
    {
      timedate.s = 0;
      s |= MIN_CHANGED;
      if(++timedate.m == 60)
      {
        timedate.m = 0;
        s |= HOUR_CHANGED;
        if(++timedate.h == 24)
        {
          timedate.h = 0;
          s |= DAY_CHANGED;
          settime(sec_time); //set date
        }
        clock_str[0] = clock_tab[timedate.h][0];
        clock_str[1] = clock_tab[timedate.h][1];
      }
      clock_str[3] = clock_tab[timedate.m][0];
      clock_str[4] = clock_tab[timedate.m][1];

      //alarm check
      a = alarm_check(&timedate);
      if(a == 1)
      {
        s |= ALARM_PLAY;
      }
      else if(a == 2)
      {
        s |= ALARM_STANDBY;
      }
    }
    clock_str[6] = clock_tab[timedate.s][0];
    clock_str[7] = clock_tab[timedate.s][1];
    status = s;
  }

  return;
}

char* getclock(void)
{
  return clock_str;
}

char* getdate(void)
{
  return date_str;
}

void gettime(TIME* t)
{
  IntMasterDisable();
  t->year  = timedate.year;
  t->month = timedate.month;
  t->day   = timedate.day;
  t->wday  = timedate.wday;
  t->h     = timedate.h;
  t->m     = timedate.m;
  t->s     = timedate.s;
  IntMasterEnable();

  return;
}

void settime(u32 s)
{
  TIME t;

  IntMasterDisable();
  SysTickIntDisable();
  sec_time = s;
  sectotime(s-1, &t);
  timedate.year  = t.year;
  timedate.month = t.month;
  timedate.day   = t.day;
  timedate.wday  = t.wday;
  timedate.h     = t.h;
  timedate.m     = t.m;
  timedate.s     = t.s;
  SysTickIntEnable();
  IntMasterEnable();

  date_str[0] = day_tab[timedate.wday][0];
  date_str[1] = day_tab[timedate.wday][1];
  date_str[3] = clock_tab[timedate.day][0];
  date_str[4] = clock_tab[timedate.day][1];
  date_str[6] = clock_tab[timedate.month][0];
  date_str[7] = clock_tab[timedate.month][1];
  itoa(timedate.year, &date_str[9], 10);

  clock_str[0] = clock_tab[timedate.h][0];
  clock_str[1] = clock_tab[timedate.h][1];
  clock_str[3] = clock_tab[timedate.m][0];
  clock_str[4] = clock_tab[timedate.m][1];
  clock_str[6] = clock_tab[timedate.s][0];
  clock_str[7] = clock_tab[timedate.s][1];
	
	status |= DAY_CHANGED | HOUR_CHANGED | MIN_CHANGED | SEC_CHANGED;

  DEBUGOUT("Set time: %s %s\r\n", getclock(), getdate());

  return;
}

s32 getdeltatime(s32 t)
{
  s32 s;
  s = on_time;
  return (s-t);
}

s32 getontime(void)
{
  s32 s;
  s = on_time;
  return s;
}

u16 standby_isactive(void)
{
  return standby_active;
}

void standby_off(void)
{
  standby_active = 0;
  return;
}

u16 standby(u16 param)
{
  u16 i, alarm = 0;
  u32 t;
  char tmp[32];

  DEBUGOUT("Standby\r\n");

  standby_active = 1;

  vs_stop();
  vs_setvolume(0); //0 -> analog power off
  delay_ms(10);
  //USB_OFF();

  //draw clock
  xv_TFT_Clear(RGB(0,0,0));
  tmp[0] = clock_str[0];
  tmp[1] = clock_str[1];
  tmp[2] = clock_str[2];
  tmp[3] = clock_str[3];
  tmp[4] = clock_str[4];
  tmp[5] = 0;
  xv_TFT_Text((LCD_WIDTH/2)-(5*(DIGITFONT_WIDTH/2)), (LCD_HEIGHT/2)-(DIGITFONT_HEIGHT/2), tmp, DIGITFONT, RGB(255,255,255), RGB(0,0,0));

  //try to get time from ntp
//#ifndef DEBUG
  t = ntp_gettime();
  if(t){ settime(t); }
//#endif

  cpu_speed(1); //low speed & reduce LED power%

  while(standby_active == 1)
  {
    eth_service();

    IntMasterDisable();
    i = status;
    status &= ~i;
    IntMasterEnable();

    if(i & MIN_CHANGED)
    {
      if(i & ALARM_PLAY)
      {
        alarm = 1;
        break;
      }

      tmp[0] = clock_str[0];
      tmp[1] = clock_str[1];
      tmp[3] = clock_str[3];
      tmp[4] = clock_str[4];
      xv_TFT_Text((LCD_WIDTH/2)-(5*(DIGITFONT_WIDTH/2)), (LCD_HEIGHT/2)-(DIGITFONT_HEIGHT/2), tmp, DIGITFONT, RGB(255,255,255), RGB(0,0,0));
    }

    if(keys_sw() || (ir_cmd() == SW_POWER))
    {
      if(alarm == 0)
      {
        xv_TFT_SetBacklight(80);
        daytime(tmp, &timedate);
        xv_TFT_Text(10, 10, tmp, NORMALFONT, RGB(255,255,255), RGB(0,0,0));

        chucknorris_rfact();
        xv_TFT_SetBacklight(100);
      }
      break;
    }
  }

  //USB_ON(); //speaker on

  cpu_speed(0); //high speed & LED 100%

  //clear cmds
  keys_sw();
  keys_steps();
  ir_cmd();

  standby_active = 0;

  if(alarm != 0)
  {
    menu_alarm();
  }

  return 0;
}

int main(void)
{
	u16 i;
	u16 reset_flags = 0;	// (LPWRRST | WWDGRST | IWDGRST | SOFTRST | PORRST | EXT | none | none)
	
	xv_system_init();
	ir_init();
	
	DEBUGOUT("\n"APPNAME" v"APPVERSION" ("__DATE__" "__TIME__")\r\n");
	DEBUGOUT("Hardware: "STM_NAME", "LCD_NAME"\r\n");
	
	//init mmc & mount filesystem
  DEBUGOUT("Init Memory Card...\r\n");
  fs_mount();
	
	//init lcd
  DEBUGOUT("Init LCD...\r\n");
	xv_TFT_Init();
	
	//show start-up screen
  xv_TFT_Clear(DEFAULT_BGCOLOR);
  xv_TFT_Rectangle( 0, 0, LCD_WIDTH-1, 10, DEFAULT_EDGECOLOR, 1);
  xv_TFT_Text(30, 2, APPNAME" v"APPVERSION, SMALLFONT, DEFAULT_BGCOLOR, DEFAULT_EDGECOLOR);
  xv_TFT_Rectangle( 0, LCD_HEIGHT-1-13, LCD_WIDTH-1, LCD_HEIGHT-1, DEFAULT_EDGECOLOR, 1);
  xv_TFT_Text(20, LCD_HEIGHT-1-10, "www.dummy-website.cz", SMALLFONT, DEFAULT_BGCOLOR, DEFAULT_EDGECOLOR);
  xv_TFT_Text(10, 20, "HW:"STM_NAME","LCD_NAME, SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR);
	
	//get reset cause
  if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET)  {reset_flags |= 0x04;		DEBUGOUT("Reset: LPWRRST\r\n");}	
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)  {reset_flags |= 0x08;		DEBUGOUT("Reset: WWDGRST\r\n");}	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)  {reset_flags |= 0x10;		DEBUGOUT("Reset: IWDGRST\r\n");}	
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET)		{reset_flags |= 0x20;		DEBUGOUT("Reset: SFTRST\r\n");}	
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)		{reset_flags |= 0x40;		DEBUGOUT("Reset: PORRST\r\n");}	
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET)		{reset_flags |= 0x80;		DEBUGOUT("Reset: PINRST\r\n");}
	if(reset_flags != 0)	{RCC_ClearFlag();}
	
	if(reset_flags) //reset_flags = reset cause
  {
    i = xv_TFT_Text(10,  35, "Reset:", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4;
    if(reset_flags & RCC_FLAG_LPWRRST){ i = xv_TFT_Text(i, 35, "LPWRRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    if(reset_flags & RCC_FLAG_WWDGRST){ i = xv_TFT_Text(i, 35, "WWDGRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    if(reset_flags & RCC_FLAG_IWDGRST){ i = xv_TFT_Text(i, 35, "IWDGRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    if(reset_flags & RCC_FLAG_SFTRST) { i = xv_TFT_Text(i, 35, "SFTRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    if(reset_flags & RCC_FLAG_PORRST) { i = xv_TFT_Text(i, 35, "PORRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    if(reset_flags & RCC_FLAG_PINRST) { i = xv_TFT_Text(i, 35, "PINRST", SMALLFONT, DEFAULT_EDGECOLOR, DEFAULT_BGCOLOR) + 4; }
    i = 35+15;
  }
  else
  {
    i = 35;
  }
	
	//Check FM Tuner RDA5807
	if(fm_tuner_initialize() == RDA5807_OK_ERR)	// No error
	{
		xv_TFT_Text(10, i, "FM Tuner: Installed", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("FM Tuner: Installed\r\n");
	}
	else	// RDA5807 not connected of HW error has occured
	{
		xv_TFT_Text(10, i, "FM Tuner: Not installed", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("FM Tuner: Not installed\r\n");
	}
	i += 10;
	
	//Check Wi-fi module ESP8266
	if(wifi_esp8266_init() == ESP8266_INSTALLED)	//ESP8266 module connected
	{
		xv_TFT_Text(10, i, "Wi-fi module: Installed", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("Wi-fi module: Installed\r\n");
	}
	else	// ESP8266 not installed or ESP8266 has bad firmware
	{
		xv_TFT_Text(10, i, "Wi-fi module: Not installed", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("Wi-fi module: Not installed\r\n");
	}
	i += 10;
	
	//show mmc state
  if(disk_status(0) == 0)
  {
    xv_TFT_Text(10, i, "Memory Card: OK", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("Memory Card: OK\r\n");
  }
  else
  {
    xv_TFT_Text(10, i, "Memory Card: Error", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
		DEBUGOUT("Memory Card: Error\r\n");
  }
  i += 10;
	
	//init ethernet
  xv_TFT_Text(10,  i, "Init Ethernet...", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR);
	i += 10;
  eth_init();
	
	//load settings
  xv_TFT_Text(10, i, "Load Settings...", SMALLFONT, DEFAULT_FGCOLOR, DEFAULT_BGCOLOR); i += 10;
  settings_read();
	
	IntMasterEnable();
	

	menu_setbgcolor(DEFAULT_BGCOLOR);
  menu_setfgcolor(DEFAULT_FGCOLOR);
  menu_setselcolor(DEFAULT_SELCOLOR);
  menu_setedgecolor(DEFAULT_EDGECOLOR);
	menu_init();
	//ADC_SoftwareStartConv(ADC1);
	//settime(46*365*24*3600UL + (3600*24*30*11) + (3600*24*3) + (3599*24));
	while(1)
	{
		eth_service();
		IntMasterDisable();
    i = status;
    status &= ~i;
    IntMasterEnable();

    if(i & MIN_CHANGED)
    {
      if(i & ALARM_PLAY)
      {
        menu_alarm();
      }
      else if(i & ALARM_STANDBY)
      {
        standby(0);
        i |= DRAWALL;
      }
    }
		fht_service();
		menu_service(i);
	}
}
