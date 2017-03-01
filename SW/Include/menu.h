#ifndef _MENU_H_
#define _MENU_H_	1U

#include "stm32f2xx.h"
#include "io.h"
#include "vs.h"
#include "TFT.h"
//#include "main.h"
#include "buffer.h"
#include "settings.h"
#include "card.h"
#include "station.h"

//----- DEFINES -----
#define DEFAULT_BGCOLOR                RGB(255,255,255) //background
#define DEFAULT_FGCOLOR                RGB(  0,  0,  0) //foreground
#define DEFAULT_SELCOLOR               RGB(255,  0,  0) //selection
#define DEFAULT_EDGECOLOR              RGB(  0,144,240) //edges

#define MENU_LINES                     (14) //lines
#define MENU_LINEHEIGHT                (15) //pixel

#define MENU_NOP                       (0)
#define MENU_PLAY                      (1)
#define MENU_UPDATE                    (2)
#define MENU_ERROR                     (3)
#define MENU_BACK                      (4)
#define MENU_BACKTXT                   "<< back <<"
#define MENU_SEARCHTXT								 "Search/tune"

#define MENU_STATE_STOP                (0)
#define MENU_STATE_BUF                 (1)
#define MENU_STATE_PLAY                (2)

#define CTRL_TEXT                      (0)
#define CTRL_BUTTON                    (1)
#define CTRL_CHECKBOX                  (2)
#define CTRL_INPUT                     (3)

typedef struct
{
  const char *name;
  const u32 *img[3];
  void(*init)(void);                      //init routine
  u16(*items)(void);             //get item count
  void(*get)(u16 item, char *s); //get item name
  u16(*open)(u16 item); //open item (ret: 0=play, 1=update, 2=error, 3=back)
  void(*close)(void);                     //close item
  void(*service)(void);                   //service routine
} MAINMENUITEM;

typedef struct
{
  u16 type; //text, button, checkbox, input
  u16 x1;
  u16 y1;
  u16 x2;
  u16 y2;
  char *val;        //value
  u16 len; //chars
  u16 sel; //control active/selected
  u16 p1;  //text: - | button: - | checkbox: checked | input: first
  u16 p2;  //text: - | button: - | checkbox: -       | input: sel (0xffff = select all)
} CONTROL;

u16 menu_openfile(char *file);
void menu_service(u16 u16_draw);
void menu_drawvol(void);
void menu_up(void);
void menu_down(void);
void menu_stopfile(void);
unsigned int menu_sw(void);
unsigned int menu_swlong(void);
void menu_alarm(void);
void menu_drawwndsub(u16 redraw);
void menu_drawwndmain(u16 redraw);
void menu_drawclock(u16 draw);
void menu_drawdate(void);
void menu_setinfo(const char *info);
void menu_setname(const char *name);
void menu_setbitrate(u16 bitrate);
void menu_setformat(u16 format);
void menu_setstatus(u16 status);
void menu_drawwndinfo(u16 redraw);
void menu_draw_fht(s16 *fht_val_l, s16 *fht_val_r);
void menu_drawwnd(u16 redraw);


void     menu_setedgecolor(u16 color);
void     menu_setselcolor(u16 color);
void     menu_setfgcolor(u16 color);
void     menu_setbgcolor(u16 color);
u16      menu_getedgecolor(void);
u16      menu_getselcolor(void);
u16      menu_getfgcolor(void);
u16      menu_getbgcolor(void);

void menu_drawctrl(CONTROL *ctrl);
void menu_createctrl(CONTROL *ctrl, u16 type, u16 sel, u16 x, u16 y, u16 p, char *value);
void menu_drawdlg(const char *title, const char *msg);
void menu_drawpopup(const char *msg);
void menu_init(void);


#endif /* _MENU_H_ */
