#include "TFT.h"

void xv_TFT_Init(void)
{
      TFT_RST_Set;
      v_TFT_delay(50000UL);
      TFT_RST_Reset;
			v_TFT_delay(50000UL);
      TFT_RST_Set;
			v_TFT_delay(50000UL);
      TFT_CS_Reset;

			v_TFT_Write_Command_Data(0x0000,0x0001);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0003,0xA8A4);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x000C,0x0000);  v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x000D,0x080C);  v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x000E,0x2B00);  v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x001E,0x00B7);  v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0001,0x2B3F);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0002,0x0600);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0010,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0011,0x6070);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0005,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0006,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0016,0xEF1C);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0017,0x0003);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0007,0x0233);  v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x000B,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x000F,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0041,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0042,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0048,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0049,0x013F);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x004A,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x004B,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0044,0xEF00);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0045,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0046,0x013F);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0030,0x0707);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0031,0x0204);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0032,0x0204);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0033,0x0502);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0034,0x0507);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0035,0x0204);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0036,0x0204);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0037,0x0502);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x003A,0x0302);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x003B,0x0302);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0023,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0024,0x0000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x0025,0x8000);	v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x004f,0x0000);	//v_TFT_delay(50000UL);
    	v_TFT_Write_Command_Data(0x004e,0x0000);	//v_TFT_delay(50000UL);			
      v_TFT_Write_Command(0x0022);							//v_TFT_delay(50000UL);
      TFT_CS_Set;
			
			v_TFT_delay(50000UL);
			xv_TFT_Clear(0x001f);		v_TFT_delay(50000UL);
			//xv_TFT_Clear(0xD400);
			xv_TFT_SetBacklight(75);
}

void xv_TFT_Clear(u16 u16_dat)
{
  u32	u32_i;

  xv_TFT_SetCursor(0x0000, 0x0000);  
  TFT_CS_Reset; 
  v_TFT_WriteIndex(0x0022); 
  TFT_RS_Set;   
  for(u32_i=0; u32_i<76800; u32_i++)
  {
    v_TFT_Write_16b_Data(u16_dat);
		TFT_WR_Reset;
		TFT_WR_Set;
  }
  TFT_CS_Set;
}

void xv_TFT_SetCursor(u16 u16_x, u16 u16_y)
{
	  v_TFT_Write_Command_Data(0x004e, u16_y);
    v_TFT_Write_Command_Data(0x004f, 0x13f - u16_x);
}

void xv_TFT_SetPoint(u16 u16_x, u16 u16_y, u16 u16_color)
{
  if ((u16_x > 320U)||(u16_y > 240U)) return;
  xv_TFT_SetCursor(u16_x, u16_y);

  TFT_CS_Reset;
  v_TFT_WriteIndex(0x0022);
  TFT_RS_Set;
  v_TFT_Write_16b_Data(u16_color);
  TFT_WR_Reset;
	TFT_WR_Set;
  TFT_CS_Set;
}

u16 xv_TFT_PutCharSpec(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;
  u16 ret;
  u8 u8_tmp_char = 0U;
	
	ret = u16_x+SPECIALFONT_WIDTH;
  if(ret > LCD_WIDTH)
  {
    return LCD_WIDTH+1;
  }

  for (u16_i=0; u16_i<SPECIALFONT_HEIGHT; u16_i++)
  {
    u8_tmp_char = special_char_8x8[((u8_c-SPECIALFONT_START)*(SMALLFONT_HEIGHT*SMALLFONT_WIDTH/8U)) + u16_i];
    for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char >> (7-u16_j)) & 0x01)
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
        }
    }
  }
	return ret;
}

u16 xv_TFT_PutCharSmallfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;
  u16 ret;
  u8 u8_tmp_char = 0U;
	
	ret = u16_x+SMALLFONT_WIDTH;
  if(ret > LCD_WIDTH)
  {
    return LCD_WIDTH+1;
  }

  for (u16_i=0; u16_i<SMALLFONT_HEIGHT; u16_i++)
  {
    u8_tmp_char = ascii_8x8[((u8_c-SMALLFONT_START)*(SMALLFONT_HEIGHT*SMALLFONT_WIDTH/8U)) + u16_i];
    for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char >> (7-u16_j)) & 0x01)
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
        }
    }
  }
	return ret;
}

u16 xv_TFT_PutCharNormalfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;
  u16 ret;
  u8 u8_tmp_char = 0U;
	
	ret = u16_x+NORMALFONT_WIDTH;
  if(ret > LCD_WIDTH)
  {
    return LCD_WIDTH+1;
  }

  for (u16_i=0; u16_i<NORMALFONT_HEIGHT; u16_i++)
  {
    u8_tmp_char = ascii_8x16[((u8_c-NORMALFONT_START)*(NORMALFONT_HEIGHT*NORMALFONT_WIDTH/8U)) + u16_i];
    for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char >> (7-u16_j)) & 0x01)
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
        }
    }
  }
	return ret;
}

u16 xv_TFT_PutCharLargefont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;
	u16 ret; 
  u8 u8_tmp_char_1 = 0U;
	u8 u8_tmp_char_2 = 0U;
	
	ret = u16_x+LARGEFONT_WIDTH;
  if(ret > LCD_WIDTH)
  {
    return LCD_WIDTH+1;
  }

  for (u16_i=0; u16_i<LARGEFONT_HEIGHT; u16_i++)
  {
    u8_tmp_char_1 = ascii_16x16[((u8_c-LARGEFONT_START)*(LARGEFONT_HEIGHT*LARGEFONT_WIDTH/8U)) + u16_i*2];
		u8_tmp_char_2 = ascii_16x16[((u8_c-LARGEFONT_START)*(LARGEFONT_HEIGHT*LARGEFONT_WIDTH/8U)) + u16_i*2+1];
    for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char_1 >> (7-u16_j)) & 0x1)
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
        }
    }
		for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char_2 >> (7-u16_j)) & 0x1)
        {
          xv_TFT_SetPoint(u16_x+u16_j+8, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j+8, u16_y+u16_i, u16_bkColor);
        }
    }
  }
	return ret;
}

u16 xv_TFT_PutCharDigitfont(u16 u16_x, u16 u16_y, u8 u8_c, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;
	u16 ret;
  u8 u8_tmp_char_1 = 0U;
	u8 u8_tmp_char_2 = 0U;
	
	ret = u16_x+DIGITFONT_WIDTH;
  if(ret > LCD_WIDTH)
  {
    return LCD_WIDTH+1;
  }

  for (u16_i=0; u16_i<20U; u16_i++)
  {
    u8_tmp_char_1 = font_digit[((u8_c-0x30)*40) + u16_i*2];
		u8_tmp_char_2 = font_digit[((u8_c-0x30)*40) + u16_i*2+1];
    for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char_1 >> (7-u16_j)) & 0x1)
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
        }
    }
		for (u16_j=0; u16_j<8; u16_j++)
    {
      if ((u8_tmp_char_2 >> (7-u16_j)) & 0x1)
        {
          xv_TFT_SetPoint(u16_x+u16_j+8, u16_y+u16_i, u16_charColor);
        }
        else
        {
          xv_TFT_SetPoint(u16_x+u16_j+8, u16_y+u16_i, u16_bkColor);
        }
    }
  }
	return ret;
}

u16 xv_TFT_Text(u16 x, u16 y, const char *str, u8 u8_font, u16 Color, u16 bkColor)
{
  u8 i;
	u16 len;
	u16 xdot;
	
	len = strlen(str);
	
	switch(u8_font)	{
		case SMALLFONT:
			for (i=0;i<len;i++)
			{
				xdot = xv_TFT_PutCharSmallfont((x+SMALLFONT_WIDTH*i), y, *str++, Color, bkColor);
				if(xdot > LCD_WIDTH)
				{
					break;
				}
			}
			break;
		case NORMALFONT: 
			for (i=0;i<len;i++)
			{
				xdot = xv_TFT_PutCharNormalfont((x+NORMALFONT_WIDTH*i), y, *str++, Color, bkColor);
				if(xdot > LCD_WIDTH)
				{
					break;
				}
			}
			break;
		case LARGEFONT:
			for (i=0;i<len;i++)
			{
				xdot = xv_TFT_PutCharLargefont((x+LARGEFONT_WIDTH*i), y, *str++, Color, bkColor);
				if(xdot > LCD_WIDTH)
				{
					break;
				}
			}
			break;
		case DIGITFONT:
			for (i=0;i<len;i++)
			{
				xdot = xv_TFT_PutCharDigitfont((x+DIGITFONT_WIDTH*i), y, *str++, Color, bkColor);
				if(xdot > LCD_WIDTH)
				{
					break;
				}
			}
			break;
		default:
			for (i=0;i<len;i++)
			{
				xdot = xv_TFT_PutCharNormalfont((x+NORMALFONT_WIDTH*i), y, *str++, Color, bkColor);
				if(xdot > LCD_WIDTH)
				{
					break;
				}
			}
			break;
		}
	return xdot;
}

void xv_TFT_DrawIcon(u16 u16_x, u16 u16_y, const u32 *u32_icon, u16 u16_charColor, u16 u16_bkColor)
{
  u16 u16_i = 0U;
  u16 u16_j = 0U;  
  u32 u32_tmp = 0U;

  for (u16_i=0; u16_i<32U; u16_i++)
  {
		u32_tmp = *(u32_icon + u16_i);
    for (u16_j=0; u16_j<32; u16_j++)
    {
      if ((u32_tmp >> (31-u16_j)) & 0x1)
      {
        xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_charColor);
      }
      else
      {
        xv_TFT_SetPoint(u16_x+u16_j, u16_y+u16_i, u16_bkColor);
      }
    }
  }
}

void xv_TFT_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color)
{
 	u16 x,y;
 	u16 dx;// = abs(x1 - x0);
 	u16 dy;// = abs(y1 - y0);

	if(y0==y1)
	{
		if(x0<=x1)
		{
			x=x0;
		}
		else
		{
			x=x1;
			x1=x0;
		}
  		while(x <= x1)
  		{
   			xv_TFT_SetPoint(x,y0,color);
   			x++;
  		}
  		return;
	}
	else if(y0>y1)
	{
		dy=y0-y1;
	}
	else
	{
		dy=y1-y0;
	}
 
 	if(x0==x1)
	{
		if(y0<=y1)
		{
			y=y0;
		}
		else
		{
			y=y1;
			y1=y0;
		}
  		while(y <= y1)
  		{
   			xv_TFT_SetPoint(x0,y,color);
   			y++;
  		}
  		return;
	}
	else if(x0 > x1)
 	{
		dx=x0-x1;
  		x = x1;
  		x1 = x0;
  		y = y1;
  		y1 = y0;
 	}
 	else
 	{
		dx=x1-x0;
  		x = x0;
  		y = y0;
 	}

 	if(dx == dy)
 	{
  		while(x <= x1)
  		{

   			x++;
			if(y>y1)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			xv_TFT_SetPoint(x,y,color);
  		}
 	}
 	else
 	{
 		xv_TFT_SetPoint(x, y, color);
  		if(y < y1)
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
    			s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				xv_TFT_SetPoint(x, y,color);
    			}
   			}
   			else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
    			s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y < y1)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				xv_TFT_SetPoint(x, y, color);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
	    		s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				xv_TFT_SetPoint(x, y,color);
    			}
   			}
	   		else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
	    		s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y1 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				xv_TFT_SetPoint(x, y,color);
    			}
   			}
  		}
 	}
}

void xv_TFT_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
{
	u16 x,y;
	s16 delta,tmp;
	x=0;
	y=r;
	delta=3-(r<<1);

	while(y>x)
	{
		if(fill)
		{
			xv_TFT_Line(cx+x,cy+y,cx-x,cy+y,color);
			xv_TFT_Line(cx+x,cy-y,cx-x,cy-y,color);
			xv_TFT_Line(cx+y,cy+x,cx-y,cy+x,color);
			xv_TFT_Line(cx+y,cy-x,cx-y,cy-x,color);
		}
		else
		{
			xv_TFT_SetPoint(cx+x,cy+y,color);
			xv_TFT_SetPoint(cx-x,cy+y,color);
			xv_TFT_SetPoint(cx+x,cy-y,color);
			xv_TFT_SetPoint(cx-x,cy-y,color);
			xv_TFT_SetPoint(cx+y,cy+x,color);
			xv_TFT_SetPoint(cx-y,cy+x,color);
			xv_TFT_SetPoint(cx+y,cy-x,color);
			xv_TFT_SetPoint(cx-y,cy-x,color);
		}
		x++;
		if(delta>=0)
		{
			y--;
			tmp=(x<<2);
			tmp-=(y<<2);
			delta+=(tmp+10);
		}
		else
		{
			delta+=((x<<2)+6);
		}
	}
}

void xv_TFT_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill)
{
	if(fill)
	{
		u16 i;
		if(x0>x1)
		{
			i=x1;
			x1=x0;
		}
		else
		{
			i=x0;
		}
		for(;i<=x1;i++)
		{
			xv_TFT_Line(i,y0,i,y1,color);
		}
		return;
	}
	xv_TFT_Line(x0,y0,x0,y1,color);
	xv_TFT_Line(x0,y1,x1,y1,color);
	xv_TFT_Line(x1,y1,x1,y0,color);
	xv_TFT_Line(x1,y0,x0,y0,color);
}

void xv_TFT_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill)
{
	xv_TFT_Rectangle(x0, y0, x0+with, y0+with, color,fill);
}

void xv_TFT_SetBacklight(u8 percent)
{
	TIM_SetCompare1(TIM11, (u32)percent);
}
	
static void v_TFT_Write_16b_Data(u16 data)
{
  //GPIOE->ODR = (GPIOE->ODR&0xff00)|(data&0x00ff);
  //GPIOE->ODR = (GPIOE->ODR&0x00ff)|(data&0xff00);
  GPIOD->ODR = data;
  __ASM volatile ("nop");   /* Delay */
}

static void v_TFT_Write_Command_Data(u16 index, u16 dat)
{
  TFT_CS_Reset;
  TFT_RS_Reset;
  TFT_RD_Set;
  v_TFT_Write_16b_Data(index);

  TFT_WR_Reset;
  TFT_WR_Set;
  TFT_RS_Set;       
  v_TFT_Write_16b_Data(dat);  
    
  TFT_WR_Reset;
  TFT_WR_Set;
  TFT_CS_Set;
}

static void v_TFT_Write_Command(unsigned int wcommand)
{
  TFT_RD_Set;
  TFT_RS_Reset;
  v_TFT_Write_16b_Data(wcommand);
  TFT_WR_Reset;
  TFT_WR_Set;
}

static void v_TFT_WriteIndex(u16 idx)
{
  TFT_RS_Reset;
  TFT_RD_Set;
  v_TFT_Write_16b_Data(idx);
  TFT_WR_Reset;
  TFT_WR_Set;
  TFT_RS_Set;
}

static void v_TFT_delay(u16 Delay)
{
  while(Delay--);
}
