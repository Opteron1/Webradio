#ifndef _TFT_H_
#define _TFT_H_		1U

#include "stm32f2xx.h"
#include <string.h>
#include "fonts.h"
#include "icons.h"
#include "io.h"

#define		TFT_RD_Set			GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_SET)
#define		TFT_RD_Reset		GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET)
#define		TFT_RS_Set			GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET)
#define		TFT_RS_Reset		GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET)
#define		TFT_WR_Set			GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_SET)
#define		TFT_WR_Reset		GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_RESET)
#define		TFT_CS_Set			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET)
#define		TFT_CS_Reset		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET)
#define		TFT_RST_Set			GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET)
#define		TFT_RST_Reset		GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET)
#define		TFT_DP_Hi(x)		((GPIOD->ODR&0x00ff)|(x&0xff00))
#define		TFT_DP_Lo(x)		((GPIOD->ODR&0xff00)|(x&0x00ff))
/*#define		TFT_DP_Hi(x)		GPIO_Write(GPIOB, x)*/
/*#define		TFT_DP_Lo(x)		GPIO_Write(GPIOC, x)*/

#define RGB(r,g,b)                     (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue
#define GET_RED(x)                     ((x>>8)&0xF8) //5 red
#define GET_GREEN(x)                   ((x>>3)&0xFC) //6 green
#define GET_BLUE(x)                    ((x<<3)&0xF8) //5 blue


void xv_TFT_Init(void);
void xv_TFT_Clear(u16 u16_dat);
void xv_TFT_SetCursor(u16 u16_x, u16 u16_y);
u16 xv_TFT_PutCharSpec(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor);
u16 xv_TFT_PutCharSmallfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor);
u16 xv_TFT_PutCharNormalfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor);
u16 xv_TFT_PutCharLargefont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor);
u16 xv_TFT_PutCharDigitfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor);
u16 xv_TFT_Text(u16 x, u16 y, const char *str, u8 u8_font, u16 Color, u16 bkColor);
void xv_TFT_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill);
void xv_TFT_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill);
void xv_TFT_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill);
void xv_TFT_SetPoint(u16 u16_x, u16 u16_y, u16 u16_color);
void xv_TFT_Line(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
void xv_TFT_DrawIcon(u16 u16_x, u16 u16_y, const u32 *u32_icon, u16 u16_charColor, u16 u16_bkColor);
void xv_TFT_SetBacklight(u8 percent);


static void v_TFT_Write_16b_Data(u16 data);
static void v_TFT_Write_Command_Data(u16 index, u16 dat);
static void v_TFT_Write_Command(unsigned int wcommand);
static void v_TFT_delay(u16 Delay);
static void v_TFT_WriteIndex(u16 idx);




#endif //_TFT_H_
