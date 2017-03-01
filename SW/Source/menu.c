#include "menu.h"

#include "fm_tuner.h"

#define SUB_STATION  		(0)
#define SUB_FM_STATION  (1)
//#define SUB_SHARE    	(1)
#define SUB_CARD     		(2)
#define SUB_ALARM    		(3)
#define SUB_SETTINGS 		(4)
#define SUB_BACK     		(5)
#define SUB_STANDBY  		(6)
#define MAINITEMS    		(7)

const MAINMENUITEM mainmenu[MAINITEMS] =
{
  {"Station",  		{&img_station[0][0],  &img_station[1][0],  &img_station[2][0]},  station_init,  station_items,	station_getitem,  station_openitem,  station_closeitem,	station_service},
	{"FM_Station",  {&img_station[0][0],  &img_station[1][0],  &img_station[2][0]},  fm_tuner_init, fm_tuner_items,	fm_tuner_getitem, fm_tuner_openitem, fm_tuner_closeitem,	fm_tuner_service},
//  {"Share",    	{&img_share[0][0],    &img_share[1][0],    &img_share[2][0]},    share_init,    share_items,    share_getitem,    share_openitem,    share_closeitem,   share_service},
  {"Card",     		{&img_card[0][0],     &img_card[1][0],     &img_card[2][0]},     card_init,     card_items,     card_getitem,     card_openitem,     card_closeitem,    card_service},
  {"Alarm",    		{&img_clock[0][0],    &img_clock[1][0],    &img_clock[2][0]},    alarm_init,    alarm_items,    alarm_getitem,    alarm_openitem,    0,                 0},
  {"Settings", 		{&img_settings[0][0], &img_settings[1][0], &img_settings[2][0]}, settings_init, settings_items, settings_getitem, settings_openitem, 0,                 0},
  {"Back",     		{&img_back[0][0],     &img_back[1][0],     &img_back[2][0]},     0,             0,              0,                0,                 0,                 0},
  {"Standby",  		{&img_power[0][0],    &img_power[1][0],    &img_power[2][0]},    0,             0,              0,                standby,           0,                 0},
};

#define MODE_INFO (0) //normal info screen
#define MODE_MAIN (1) //animated main menu
#define MODE_SUB  (2) //list menu

u16 menu_mode=0, menu_sub=0, menu_items=0, menu_first=0, menu_last=0, menu_sel=0, menu_lastsel=0;
u16 menu_status=0, menu_format=0, menu_bitrate=0;
u16 bgcolor=0xffff, fgcolor=0, selcolor=0, edgecolor=0;

u16 menu_openfile(char *file)
{
  u16 ret;
  char tmp[MAX_ADDR];

  if(file[0] == 0)
  {
    return 1;
  }

  strncpy(tmp, file, MAX_ADDR-1);
  tmp[MAX_ADDR-1] = 0;

  if(mainmenu[menu_sub].close)
  {
    mainmenu[menu_sub].close();
  }

  ret = 1;
  if(isdigit(tmp[0])) //station number
  {
    menu_sub = SUB_STATION;
    mainmenu[menu_sub].init();

    if(station_open(atoi(tmp)) == STATION_OPENED)
    {
      ret = 0;
    }
  }
  else //path to card file
  {
    menu_sub = SUB_CARD;
    mainmenu[menu_sub].init();
    if(card_openfile(tmp) == MENU_PLAY)
    {
      menu_setname(tmp);
      menu_setinfo("");
      ret = 0;
    }
  }
  menu_mode    = MODE_INFO;
  menu_items   = 1;
  menu_first   = 0;
  menu_last    = 0;
  menu_sel     = 0;
  menu_lastsel = 0;
  menu_drawwnd(1);

  return ret;
}

void menu_stopfile(void)
{
  if(mainmenu[menu_sub].close)
  {
    mainmenu[menu_sub].close();
  }

  menu_drawwnd(1);

  return;
}

unsigned int menu_sw(void)
{
  switch(menu_mode)
  {
    case MODE_INFO:
      if(menu_items == 0) //main
      {
        menu_mode    = MODE_MAIN;
        menu_items   = 0;
        menu_sel     = menu_sub;
        menu_lastsel = menu_sel;
      }
      else                //sub
      {
        if(mainmenu[menu_sub].close)
        {
          mainmenu[menu_sub].close();
        }
        menu_mode = MODE_SUB;
        if(menu_last == 0)
        {
          menu_items   = mainmenu[menu_sub].items();
          menu_first   = menu_sel;
          menu_last    = (menu_items >= MENU_LINES)?(MENU_LINES-1):(menu_items-1);
          menu_sel     = 0;
          menu_lastsel = 0;
        }
      }
      break;

    case MODE_MAIN:
      if(mainmenu[menu_sel].items) //sub
      {
        menu_mode  = MODE_SUB;
        menu_sub   = menu_sel;
        if(mainmenu[menu_sub].init)
        {
          mainmenu[menu_sub].init();
        }
        menu_items   = mainmenu[menu_sub].items();
        menu_first   = 0;
        menu_last    = (menu_items >= MENU_LINES)?(MENU_LINES-1):(menu_items-1);
        menu_sel     = 0;
        menu_lastsel = 0;
      }
      else                         //open item
      {
        if(mainmenu[menu_sel].open) //open and stay in main mode
        {
          mainmenu[menu_sel].open(0);
          menu_lastsel = (menu_sel==(MAINITEMS-1))?0:(menu_sel+1);
        }
        else //back
        {
          menu_mode  = MODE_INFO;
          menu_items = 0;
        }
      }
      break;

    case MODE_SUB:
      if(mainmenu[menu_sub].open)
      {
        switch(mainmenu[menu_sub].open(menu_sel))
        {
          case MENU_NOP:
            break;
          case MENU_PLAY:
            menu_mode = MODE_INFO;
            break;
          case MENU_UPDATE:
          case MENU_ERROR:
            menu_items   = mainmenu[menu_sub].items();
            menu_first   = 0;
            menu_last    = (menu_items>=MENU_LINES)?(MENU_LINES-1):(menu_items-1);
            menu_sel     = 0;
            menu_lastsel = 0;
            break;
          case MENU_BACK:
            menu_mode    = MODE_MAIN;
            menu_items   = 0;
            menu_sel     = menu_sub;
            menu_lastsel = menu_sel;
            break;
        }
      }
      break;
  }

  return 1;
}

unsigned int menu_swlong(void)
{
  switch(menu_mode)
  {
    case MODE_INFO:
      break;

    case MODE_MAIN:
      menu_mode  = MODE_INFO;
      menu_items = 0;
      break;

    case MODE_SUB:
      menu_mode    = MODE_MAIN;
      menu_items   = 0;
      menu_sel     = menu_sub;
      menu_lastsel = menu_sel;
      break;
  }

  return 1;
}

void menu_up(void)
{
  switch(menu_mode)
  {
    case MODE_INFO:
			if(fm_tuner_get_status() == FM_TUNER_PLAY)
			{
				fm_tuner_set_volume(fm_tuner_get_volume()+1);
			}
			else
			{
				vs_setvolume(vs_getvolume()+2);
			}
      menu_drawvol();
      break;

    case MODE_MAIN:
      if(menu_sel < (MAINITEMS-1))
      {
        menu_sel++;
      }
      else
      {
        menu_sel = 0;
      }
      menu_drawwndmain(0);
      break;

    case MODE_SUB:
      if(menu_sel < menu_last)
      {
        menu_sel++;
      }
      else if(menu_sel < (menu_items-1))
      {
        menu_sel++;
        menu_first++;
        menu_last++;
      }
      break;
  }

  return;
}


void menu_down(void)
{
  switch(menu_mode)
  {
    case MODE_INFO:
			if(fm_tuner_get_status() == FM_TUNER_PLAY)
			{
				fm_tuner_set_volume((fm_tuner_get_volume()>0)?fm_tuner_get_volume()-1:0);
			}
			else
			{
				vs_setvolume((vs_getvolume()<=2)?1:(vs_getvolume()-2));
			}
      menu_drawvol();
      break;

    case MODE_MAIN:
      if(menu_sel > 0)
      {
        menu_sel--;
      }
      else
      {
        menu_sel = (MAINITEMS-1);
      }
      menu_drawwndmain(0);
      break;

    case MODE_SUB:
      if(menu_sel > menu_first)
      {
        menu_sel--;
      }
      else if(menu_sel > 0)
      {
        menu_sel--;
        menu_first--;
        menu_last--;
      }
      break;
  }

  return;
}

void menu_steps(int steps)
{
  if(steps != 0)
  {
    if(menu_mode == MODE_INFO)
    {
			if(fm_tuner_get_status() == FM_TUNER_PLAY)
			{
				fm_tuner_set_volume(((fm_tuner_get_volume()+steps) < 0)?0:fm_tuner_get_volume()+steps);
			}
			else
			{
				vs_setvolume(vs_getvolume()+steps);
			}
      menu_drawvol();
    }
    else
    {
      if(steps > 0)
      {
        if(steps > MENU_LINES)
        {
          steps = MENU_LINES;
        }
        while(steps--)
        {
          menu_up();
        }
      }
      else
      {
        if(steps < -MENU_LINES)
        {
          steps = -MENU_LINES;
        }
        while(steps++)
        {
          menu_down();
        }
      }
    }
  }

  return;
}

void menu_service(u16 u16_draw)
{
	u16 redraw = 0U;
	static u16 menu_standbytimer=0;
	
	menu_steps(keys_steps());
	
	//rotary encoder
  switch(keys_sw())
  {
    case SW_PRESSED:
      redraw |= menu_sw();
      break;
    case SW_PRESSEDLONG:
      redraw |= menu_swlong();
      break;
  }
	
	//ir remote control
  switch(ir_cmd())
  {
    case SW_VOLP:
      switch(menu_mode)
      {
        case MODE_INFO:
					if(fm_tuner_get_status() == FM_TUNER_PLAY)
					{
						fm_tuner_set_volume(fm_tuner_get_volume()+1);
					}
					else
					{
						vs_setvolume(vs_getvolume()+4);
					}
					menu_drawvol();
					break;
        case MODE_MAIN: menu_steps(+1);                                                         break;
        case MODE_SUB:  menu_steps(+MENU_LINES);                                                break;
      }
      break;
    case SW_VOLM:
      switch(menu_mode)
      {
        case MODE_INFO:
					if(fm_tuner_get_status() == FM_TUNER_PLAY)
					{
						fm_tuner_set_volume((fm_tuner_get_volume()>0)?fm_tuner_get_volume()-1:0);
					}
					else
					{
						vs_setvolume((vs_getvolume()<=4)?1:(vs_getvolume()-4));
					}
					menu_drawvol();
					break;
        case MODE_MAIN: menu_steps(-1);                                                         break;
        case MODE_SUB:  menu_steps(-MENU_LINES);                                                break;
      }
      break;
    case SW_UP:
      menu_steps(-1);
      break;
    case SW_DOWN:
      menu_steps(+1);
      break;
    case SW_ENTER:
      redraw |= menu_sw();
      break;
    case SW_POWER:
      if(mainmenu[menu_sub].close)
      {
        mainmenu[menu_sub].close();
      }
      standby(0);
      redraw = 1;
      break;
  }
	
	if(redraw == 0)
  {
    menu_drawwnd(0);
    if(u16_draw & DRAWALL)
    {
      redraw = 1;
    }
    else
    {
      if(u16_draw & SEC_CHANGED)
      {
        if(menu_status == MENU_STATE_STOP)
        {
          if(++menu_standbytimer > STANDBY_TIME)
          {
            standby(0);
            redraw = 1;
          }
        }
        menu_drawclock(u16_draw);
      }
      if(u16_draw & DAY_CHANGED)
      {
        menu_drawdate();
      }
    }
  }
	if(redraw)
  {
    menu_standbytimer = 0;
    menu_drawwnd(1);
  }

  if(mainmenu[menu_sub].service)
  {
    mainmenu[menu_sub].service();
  }

  return;
}

void menu_drawvol(void)
{
  u16 x;

  if(menu_mode == MODE_INFO)
  {
		if(fm_tuner_get_status() == FM_TUNER_PLAY)
		{
			x = fm_tuner_get_volume(); //15
			xv_TFT_Rectangle(LCD_WIDTH-1-5-15,   2, LCD_WIDTH-1-5-15+x, 8, fgcolor, 1);
			xv_TFT_Rectangle(LCD_WIDTH-1-5-15+x, 2, LCD_WIDTH-1-5,      8, bgcolor, 1);
		}
			else
		{
			x = (vs_getvolume()/4); //100/4 = 25
			xv_TFT_Rectangle(LCD_WIDTH-1-5-25,   2, LCD_WIDTH-1-5-25+x, 8, fgcolor, 1);
			xv_TFT_Rectangle(LCD_WIDTH-1-5-25+x, 2, LCD_WIDTH-1-5,      8, bgcolor, 1);
		}    
  }

  return;
}

void menu_drawstatus(void)
{
  char c, buf[8];

  switch(menu_status)
  {
    case MENU_STATE_STOP: c = 0x02; break;
    case MENU_STATE_BUF:  c = 0x01; break;
    case MENU_STATE_PLAY: c = 0x00; break;
    default:              c = ' ';  break;
  }
  if(c!=' ')xv_TFT_PutCharSpec(LCD_WIDTH-1-5-38, 1, c, bgcolor, edgecolor);
	else xv_TFT_PutCharSmallfont(LCD_WIDTH-1-5-38, 1, c, bgcolor, edgecolor);

  switch(menu_format)
  {
    case FORMAT_WAV:  strcpy(buf, "WAV"); break;
    case FORMAT_MP3:  strcpy(buf, "MP3"); break;
    case FORMAT_AAC:  strcpy(buf, "AAC"); break;
    case FORMAT_OGG:  strcpy(buf, "OGG"); break;
    case FORMAT_WMA:  strcpy(buf, "WMA"); break;
    case FORMAT_FLAC: strcpy(buf, "FLA"); break;
    default:          strcpy(buf, "   ");  break;
  }
  xv_TFT_Text(LCD_WIDTH-1-5-65, 2, buf, SMALLFONT, bgcolor, edgecolor);

  if(menu_bitrate)
  {
    itoa(menu_bitrate, buf, 10);
  }
  else
  {
    strcpy(buf, "   ");
  }
  xv_TFT_Text(LCD_WIDTH-1-5-93, 2, buf, SMALLFONT, bgcolor, edgecolor);

  return;
}

void menu_alarm(void)
{
  unsigned int i;

  DEBUGOUT("Alarm\r\n");

  //set alarm volume
  i = alarm_getvol();
  if(i)
  {
    vs_setvolume(i);
    menu_drawvol();
  }

  //open alarm file
  if(menu_status == MENU_STATE_STOP)
  {
    menu_drawpopup("Alarm");
    for(i=1; i<=ALARM_FILEITEMS; i++) //open alarm file
    {
      if(alarm_getfile(gbuf.menu.file, i) == 0)
      {
        if(menu_openfile(gbuf.menu.file) == 0)
        {
          break;
        }
      }
    }
  }

  return;
}

#define ITEM_LEFT (0)
#define ITEM_TOP  (11)
void menu_drawwndsub(u16 redraw)
{
  u16 i, x, y;
  static u16 last_first=0;
  char tmp[MAX_NAME];
	
  if(redraw == 0)
  {
    if(menu_sel == menu_lastsel)
    {
      return;
    }
  }

  //clear last selection
  i = ITEM_TOP + ((menu_lastsel-last_first)*MENU_LINEHEIGHT);
  xv_TFT_Rectangle(ITEM_LEFT, i, LCD_WIDTH-5, i+MENU_LINEHEIGHT, bgcolor, 0);

  //draw items
  if((menu_first != last_first) || redraw)
  {
    last_first = menu_first;
    for(i=0; (i < MENU_LINES) && ((menu_first+i) < menu_items); i++)
    {
      mainmenu[menu_sub].get(menu_first+i, tmp);
      x = xv_TFT_Text(ITEM_LEFT+2, ITEM_TOP+2+(i*MENU_LINEHEIGHT), tmp, NORMALFONT, fgcolor, bgcolor);
      if(x < (LCD_WIDTH-5))
      {
        y = ITEM_TOP+2+(i*MENU_LINEHEIGHT);
        xv_TFT_Rectangle(x, y, LCD_WIDTH-5-1, y+12-1, bgcolor, 1); //font height = 12
      }
    }
  }

	//selection
  i = ITEM_TOP + ((menu_sel-menu_first)*MENU_LINEHEIGHT);
  xv_TFT_Rectangle(ITEM_LEFT, i, LCD_WIDTH-5, i+MENU_LINEHEIGHT, selcolor, 0);
	
  //scrollbar
  i = ITEM_TOP+((menu_sel*(LCD_HEIGHT-1-8-ITEM_TOP)) / (menu_items-1));
  xv_TFT_Rectangle(LCD_WIDTH-4, ITEM_TOP, LCD_WIDTH-1, LCD_HEIGHT-1, edgecolor, 1);
  xv_TFT_Rectangle(LCD_WIDTH-4, i, LCD_WIDTH-1, i+8, fgcolor, 1);

  menu_lastsel = menu_sel;

  return;
}

#define IMG_TOP    (45)
#define IMG_LEFT   (0)
#define IMG_MIDDLE ((LCD_WIDTH/2)-(32/2))
#define IMG_RIGHT  (LCD_WIDTH-32)
void menu_drawwndmain(u16 redraw)
{
  s16 x[4], add;
  u16 state[4], item[4];
  u16 i, stop;

  if(redraw == 0)
  {
    if(menu_sel == menu_lastsel)
    {
      return;
    }
  }
  else
  {
    if(menu_sel == menu_lastsel)
    {
      menu_lastsel = (menu_sel==(MAINITEMS-1))?0:(menu_sel+1);
    }
  }

  x[0]     = IMG_LEFT;
  x[1]     = IMG_MIDDLE;
  x[2]     = IMG_RIGHT;
  state[0] = 2;
  state[1] = 0;
  state[2] = 2;
  state[3] = 2;

  item[1] = menu_lastsel;

  if(menu_lastsel == 0)
  {
    item[0] = MAINITEMS-1;
  }
  else
  {
    item[0] = menu_lastsel-1;
  }

  if(menu_lastsel == (MAINITEMS-1))
  {
    item[2] = 0;
  }
  else
  {
    item[2] = menu_lastsel+1;
  }

  if(((menu_sel > menu_lastsel) && ((menu_sel-menu_lastsel) == 1)) ||
     ((menu_sel == 0) && ((menu_lastsel == (MAINITEMS-1))) ))          //move ->
  {
    if(item[2] == (MAINITEMS-1))
    {
      item[3] = 0;
    }
    else
    {
      item[3] = item[2]+1;
    }
    x[3] = IMG_RIGHT+IMG_MIDDLE;
    add  = -2;
  }
  else                                                                 //move <-
  {
    if(item[0] == 0)
    {
      item[3] = MAINITEMS-1;
    }
    else
    {
      item[3] = item[0]-1;
    }
    x[3] = -IMG_MIDDLE;
    add  = +2;
  }

  stop = 0;
  while(stop == 0)
  {
    for(i=0; i<4; i++)
    {
      x[i] += add;
      switch(x[i])
      {
        case IMG_MIDDLE/3:              state[i]=2;         break;
        case IMG_MIDDLE/2:              state[i]=1;         break;
        case IMG_MIDDLE-10:             state[i]=0;         break;
        case IMG_MIDDLE:                state[i]=0; stop=1; break;
        case IMG_MIDDLE+10:             state[i]=0;         break;
        case IMG_RIGHT-(IMG_MIDDLE/2):  state[i]=1;         break;
        case IMG_RIGHT-(IMG_MIDDLE/3):  state[i]=2;         break;
      }
      xv_TFT_DrawIcon(x[i], IMG_TOP, mainmenu[item[i]].img[state[i]], fgcolor, bgcolor);
    }
  }

  i = (LCD_WIDTH/2)-(strlen(mainmenu[menu_sel].name)*NORMALFONT_WIDTH/2);
	xv_TFT_Rectangle(0, IMG_TOP+32+15, 319, IMG_TOP+32+15+NORMALFONT_HEIGHT,bgcolor, 1U);
  xv_TFT_Text(i, IMG_TOP+32+15, mainmenu[menu_sel].name, NORMALFONT, fgcolor, bgcolor);

  menu_lastsel = menu_sel;

  return;
}

void menu_drawclock(u16 draw)
{
  char *clock;

  if(menu_mode == MODE_INFO)
  {
    clock = getclock();
    if(draw & HOUR_CHANGED)
    {
      xv_TFT_Text(((LCD_WIDTH/2)-((8*DIGITFONT_WIDTH)/2))+(0*DIGITFONT_WIDTH), LCD_HEIGHT-24, clock+0, DIGITFONT, bgcolor, fgcolor); //00:00:00
    }
    else if(draw & MIN_CHANGED)
    {
      xv_TFT_Text(((LCD_WIDTH/2)-((8*DIGITFONT_WIDTH)/2))+(3*DIGITFONT_WIDTH), LCD_HEIGHT-24, clock+3, DIGITFONT, bgcolor, fgcolor); //---00:00
    }
    else if(draw & SEC_CHANGED)
    {
      xv_TFT_Text(((LCD_WIDTH/2)-((8*DIGITFONT_WIDTH)/2))+(6*DIGITFONT_WIDTH), LCD_HEIGHT-24, clock+6, DIGITFONT, bgcolor, fgcolor); //------00
    }
  }

  return;
}

void menu_drawdate(void)
{
  if(menu_mode == MODE_INFO)
  {
    xv_TFT_Text(((LCD_WIDTH/2)-((13*NORMALFONT_WIDTH)/2)), LCD_HEIGHT-42, getdate(), NORMALFONT, bgcolor, fgcolor);
  }

  return;
}

void menu_setinfo(const char *info)
{
  DEBUGOUT("Menu: info: %s\r\n", info);

  strncpy(gbuf.menu.info, info, MAX_INFO-1);
  if(menu_mode == MODE_INFO)
  {
    menu_drawwndinfo(1);
  }

  return;
}


void menu_setname(const char *name)
{
  DEBUGOUT("Menu: name: %s\r\n", name);

  strncpy(gbuf.menu.name, name, MAX_NAME-1);
  if(menu_mode == MODE_INFO)
  {
    menu_drawwndinfo(1);
  }

  return;
}

void menu_setbitrate(u16 bitrate)
{
  menu_bitrate = bitrate;
  if(menu_mode == MODE_INFO)
  {
    menu_drawstatus();
  }

  return;
}

void menu_setformat(u16 format)
{
  menu_format = format;
  if(menu_mode == MODE_INFO)
  {
    menu_drawstatus();
  }

  return;
}

void menu_setstatus(u16 status)
{
  menu_status = status;

  if(status == MENU_STATE_STOP)
  {
    menu_format  = FORMAT_UNKNOWN;
    menu_bitrate = 0;
  }

  if(menu_mode == MODE_INFO)
  {
    menu_drawstatus();
  }

  return;
}

void menu_drawwndinfo(u16 redraw)
{
  if(redraw)
  {
    menu_drawstatus();
    menu_drawvol();
    menu_drawdate();
    menu_drawclock(HOUR_CHANGED);
  
    xv_TFT_Text(5, 18, gbuf.menu.name, NORMALFONT, fgcolor, bgcolor);
    xv_TFT_Text(5, 40, gbuf.menu.info, SMALLFONT, fgcolor, bgcolor);
  }
#ifdef FHT_ENABLE
	if(fht_updated())
	{
		menu_draw_fht(fht_sample_buffer_left, fht_sample_buffer_right);	//Draw FHT graph
		fht_clear_update_status();																			//Wait for new samples
	}
#endif
  return;
}

#ifdef FHT_ENABLE
/*
                |
	 |    |       |       |    |
	|||  |||	 | 	|  |   |||  |||
	||| |||||  |  |  |  ||||| |||
L---------------0---------------P
*/
void menu_draw_fht(s16 *fht_val_l, s16 *fht_val_r)
{
	u16 i;
	xv_TFT_Rectangle(LCD_WIDTH - 2*FHT_LEN - 3, LCD_HEIGHT - 53, LCD_WIDTH - 1, LCD_HEIGHT - 51, edgecolor, 1);
	xv_TFT_Rectangle(LCD_WIDTH - FHT_LEN - 3, LCD_HEIGHT - N_DB - 54, LCD_WIDTH - FHT_LEN - 1, LCD_HEIGHT - 54, edgecolor, 1);
	for(i = 0; i < FHT_LEN/2; i++)	// Bins 0 to FHT_LEN/2 for LEFT channel
	{
		xv_TFT_Line(LCD_WIDTH - FHT_LEN - 4 - i*2, LCD_HEIGHT - 54, LCD_WIDTH - FHT_LEN - 4 - i*2, LCD_HEIGHT - 54 - fht_val_l[i]/8, selcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN - 4 - i*2, LCD_HEIGHT - 54 - fht_val_l[i]/8, LCD_WIDTH - FHT_LEN - 4 - i*2, LCD_HEIGHT - 54 - N_DB, bgcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN - 4 - i*2 - 1, LCD_HEIGHT - 54, LCD_WIDTH - FHT_LEN - 4 - i*2 - 1, LCD_HEIGHT - 54 - fht_val_l[i]/8, selcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN - 4 - i*2 - 1, LCD_HEIGHT - 54 - fht_val_l[i]/8, LCD_WIDTH - FHT_LEN - 4 - i*2 -1, LCD_HEIGHT - 54 - N_DB, bgcolor);
	}
	for(i = 0; i < FHT_LEN/2; i++)	// Bins 0 to FHT_LEN/2 for RIGHT channel
	{
		xv_TFT_Line(LCD_WIDTH - FHT_LEN + 1 + i*2, LCD_HEIGHT - 54, LCD_WIDTH - FHT_LEN + 1 + i*2, LCD_HEIGHT - 54 - fht_val_r[i]/8, selcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN + 1 + i*2, LCD_HEIGHT - 54 - fht_val_r[i]/8, LCD_WIDTH - FHT_LEN + 1 + i*2, LCD_HEIGHT - 54 - N_DB, bgcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN + 1 + i*2 -1, LCD_HEIGHT - 54, LCD_WIDTH - FHT_LEN + 1 + i*2 - 1, LCD_HEIGHT - 54 - fht_val_r[i]/8, selcolor);
		xv_TFT_Line(LCD_WIDTH - FHT_LEN + 1 + i*2 -1, LCD_HEIGHT - 54 - fht_val_r[i]/8, LCD_WIDTH - FHT_LEN + 1 + i*2 -1, LCD_HEIGHT - 54 - N_DB, bgcolor);
	}
}
#endif

void menu_drawwnd(u16 redraw)
{
  unsigned int i;

  if(standby_isactive())
  {
    return;
  }

  //draw background
  if(redraw)
  {
    //title bar
    xv_TFT_Rectangle(0, 0, LCD_WIDTH-1, 10, edgecolor, 1);

    //text background
    xv_TFT_Rectangle(0, 11, LCD_WIDTH-1, LCD_HEIGHT-1, bgcolor, 1);

    //text
    switch(menu_mode)
    {
      case MODE_INFO:
        xv_TFT_Text(4, 2, APPNAME, SMALLFONT, bgcolor, edgecolor);
        xv_TFT_Line(0, 35, LCD_WIDTH-1, 35, edgecolor);
        xv_TFT_Rectangle(0, LCD_HEIGHT-46, LCD_WIDTH-1, LCD_HEIGHT-1, fgcolor, 1);
        break;
      case MODE_MAIN:
        xv_TFT_Text( 4, 2, APPNAME, SMALLFONT, bgcolor, edgecolor);
        break;
      case MODE_SUB:
        if(gbuf.menu.file[0] != 0)
        {
          for(i=0; (strlen(gbuf.menu.file+i)*SMALLFONT_WIDTH+5) > (LCD_WIDTH-1); i++);
          xv_TFT_Text(4, 2, gbuf.menu.file+i, SMALLFONT, bgcolor, edgecolor);
        }
        else
        {
          xv_TFT_Text(4, 2, mainmenu[menu_sub].name, SMALLFONT, bgcolor, edgecolor);
        }
        break;
    }
  }

  //draw text, controls...
  switch(menu_mode)
  {
    case MODE_INFO: menu_drawwndinfo(redraw); break;
    case MODE_MAIN: menu_drawwndmain(redraw); break;
    case MODE_SUB:  menu_drawwndsub(redraw);  break;
  }

  return;
}

void menu_drawctrl(CONTROL *ctrl)
{
  unsigned int i, x;
  char *ptr;

  switch(ctrl->type)
  {
    case CTRL_TEXT:
      x = xv_TFT_Text(ctrl->x1+2, ctrl->y1+2, ctrl->val, NORMALFONT, fgcolor, bgcolor);
      xv_TFT_Rectangle(x, ctrl->y1+2, ctrl->x2-2, ctrl->y2-2, bgcolor, 1);
      break;

    case CTRL_BUTTON:
      xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, edgecolor, 1);
      xv_TFT_Text(ctrl->x1+2, ctrl->y1+2, ctrl->val, NORMALFONT, fgcolor, edgecolor);
      if(ctrl->sel)
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, selcolor, 0);
      }
      else
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, edgecolor, 0);
      }
      break;

    case CTRL_CHECKBOX:
      xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, edgecolor, 1);
      if(ctrl->p1) //checked
      {
        xv_TFT_Text(ctrl->x1+2, ctrl->y1+2, ctrl->val, NORMALFONT, selcolor, edgecolor);
      }
      else
      {
        xv_TFT_Text(ctrl->x1+2, ctrl->y1+2, ctrl->val, NORMALFONT, bgcolor, edgecolor);
      }
      if(ctrl->sel)
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, selcolor, 0);
      }
      else
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, edgecolor, 0);
      }
      break;

    case CTRL_INPUT:
      ptr = ctrl->val + ctrl->p1;
      for(i=0, x=ctrl->x1+2; (i<ctrl->len) && *ptr; i++)
      {
        if(ctrl->sel && (ctrl->p2 == 0xFFFF)) //select all
        {
          x = xv_TFT_Text(x, ctrl->y1+2, ptr++, NORMALFONT, fgcolor, edgecolor);
        }
        else if(ctrl->sel && ((ctrl->p1+i) == ctrl->p2))
        {
          x = xv_TFT_Text(x, ctrl->y1+2, ptr++, NORMALFONT, fgcolor, edgecolor);
        }
        else
        {
          x = xv_TFT_Text(x, ctrl->y1+2, ptr++, NORMALFONT, fgcolor, bgcolor);
        }
      }
      xv_TFT_Rectangle(x, ctrl->y1+2, ctrl->x2-2, ctrl->y2-2, bgcolor, 1);
      if(ctrl->sel)
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, selcolor, 0);
      }
      else
      {
        xv_TFT_Rectangle(ctrl->x1, ctrl->y1, ctrl->x2, ctrl->y2, edgecolor, 0);
      }
      break;
  }

  return;
}

void menu_createctrl(CONTROL *ctrl, u16 type, u16 sel, u16 x, u16 y, u16 p, char *value)
{
  unsigned int len=0;

  memset(ctrl, 0, sizeof(CONTROL));

  switch(type)
  {
    case CTRL_TEXT:
      break;
    case CTRL_BUTTON:
      break;
    case CTRL_CHECKBOX:
      ctrl->p1 = (p!=0)?1:0;
      break;
    case CTRL_INPUT:
      len = p;
      break;
  }

  if(len == 0)
  {
    len = strlen(value);
  }

  ctrl->type = type;
  ctrl->x1   = x;
  ctrl->y1   = y;
  ctrl->x2   = x + 3 + (NORMALFONT_WIDTH*len);
  ctrl->y2   = y + 3 + NORMALFONT_HEIGHT;
  ctrl->val  = value;
  ctrl->len  = len;
  ctrl->sel  = sel;

  menu_drawctrl(ctrl);

  return;
}

void menu_drawdlg(const char *title, const char *msg)
{
  xv_TFT_Rectangle(0, 0, LCD_WIDTH-1, NORMALFONT_HEIGHT+2, edgecolor, 1);
  xv_TFT_Text(2, 1, title, NORMALFONT, bgcolor, edgecolor);

  xv_TFT_Rectangle(0, NORMALFONT_HEIGHT+2, LCD_WIDTH-1, LCD_HEIGHT-1, bgcolor, 1);
  xv_TFT_Text(2, 20, msg, NORMALFONT, fgcolor, bgcolor);

  return;
}

void menu_drawpopup(const char *msg)
{
  xv_TFT_Rectangle(4, (LCD_HEIGHT/2)-11, LCD_WIDTH-1-4, (LCD_HEIGHT/2)+11, edgecolor, 0);
  xv_TFT_Rectangle(5, (LCD_HEIGHT/2)-10, LCD_WIDTH-1-5, (LCD_HEIGHT/2)+10, fgcolor, 1);
  xv_TFT_Text(10, (LCD_HEIGHT/2)- 4, msg, SMALLFONT, bgcolor, fgcolor);

  return;
}

void menu_setedgecolor(u16 c) { edgecolor = c; }
void menu_setselcolor(u16 c)  { selcolor  = c; }
void menu_setfgcolor(u16 c)   { fgcolor   = c; }
void menu_setbgcolor(u16 c)   { bgcolor   = c; }


u16 menu_getedgecolor(void)   { return edgecolor; }
u16 menu_getselcolor(void)    { return selcolor;  }
u16 menu_getfgcolor(void)     { return fgcolor;   }
u16 menu_getbgcolor(void)     { return bgcolor;   }

void menu_init(void)
{
  TIME t;

  DEBUGOUT("Menu: init\r\n");

  gbuf.menu.name[0]          = 0;
  gbuf.menu.info[0]          = 0;
  gbuf.menu.file[0]          = 0;
  gbuf.menu.name[MAX_NAME-1] = 0;
  gbuf.menu.info[MAX_INFO-1] = 0;
  gbuf.menu.file[MAX_ADDR-1] = 0;

  menu_setstatus(MENU_STATE_STOP);
  gettime(&t);
  daytime(gbuf.menu.name, &t);
  menu_drawwnd(1);

  //auto start
  if(ini_getentry(SETTINGS_FILE, "AUTOSTART", gbuf.menu.file, MAX_ADDR) == 0)
  {
    menu_openfile(gbuf.menu.file);
  }

  return;
}
