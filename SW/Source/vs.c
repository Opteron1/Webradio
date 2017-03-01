#include "vs.h"
u16 vs_playing = 0U;
s16 vs_vol = 0, vs_sbamp=0, vs_sbfreq=0, vs_stamp=0, vs_stfreq=0;
VSBUFFER vs_buf;
volatile u16 vs_bufhead=0, vs_buftail=0;

void vs_handler(void)
{
	u16 len, tail;
	if(vs_request() == 0)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
		return;
	}
		
	len = vs_buflen();
	if(len != 0)
	{
		if(len > 32)
		{
			len = 32;
		}
		while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET); //ssi transmit fifo full?
		VS_DCS_DISABLE();
		tail = vs_buftail;
		VS_DCS_ENABLE();
		while(len--)
		{
			vs_ssi_write(vs_buf.b8[tail]);
			if(++tail == VS_BUFSIZE)
			{
				tail = 0;
			}
		}
		vs_buftail = tail;
	}
	else
	{
		vs_pause();
	}

  //if(!vs_request())			
	//EXTI_ClearITPendingBit(EXTI_Line5);
	return;
}

u8 vs_bufgetc(void)
{
  unsigned char c;
  unsigned int head, tail;

  head = vs_bufhead;
  tail = vs_buftail;

  if(head != tail)
  {
    c = vs_buf.b8[tail++];
    if(tail == VS_BUFSIZE)
    {
      tail = 0;
    }
    vs_buftail = tail;
  }
  else
  {
    c = 0;
  }

  return c;
}

void vs_bufputs(const u8 *s, u16 len)
{
  u16 head;

  head = vs_bufhead;
  while(len--)
  {
    vs_buf.b8[head++] = *s++;
    if(head == VS_BUFSIZE)
    {
      head = 0;
    }
  }
  vs_bufhead = head;

  return;
}

u16 vs_buffree(void)
{
  u16 head, tail;

  head = vs_bufhead;
  tail = vs_buftail;

  if(head > tail)
  {
    return (VS_BUFSIZE-(head-tail))-1;
  }
  else if(head < tail)
  {
    return (tail-head)-1;
  }

  return (VS_BUFSIZE-1);
}

u16 vs_buflen(void)
{
  u16 head, tail;

  head = vs_bufhead;
  tail = vs_buftail;

  if(head > tail)
  {
    return (head-tail);
  }
  else if(head < tail)
  {
    return (VS_BUFSIZE-(tail-head));
  }

  return 0;
}

void vs_bufsethead(u16 head)
{
  vs_bufhead = head;

  return;
}

void vs_bufreset(void)
{
  vs_bufhead = 0;
  vs_buftail = 0;

  return;
}

s16 vs_gettreblefreq(void)
{
  return vs_stfreq*1000;
}

void vs_settreblefreq(s16 freq) //1000 - 15000Hz
{
  freq /= 1000;

  if(freq < 1) //< 1
  {
    freq = 1;
  }
  else if(freq > 15) //> 15
  {
    freq = 15;
  }
  vs_stfreq = freq;
  vs_write_bass();

  return;
}

s16 vs_gettrebleamp(void)
{
  return vs_stamp;
}

void vs_settrebleamp(s16 amp) //-8 - 7dB
{
  if(amp < -8) //< -8
  {
    amp = -8;
  }
  else if(amp > 7) //> 7
  {
    amp = 7;
  }
  vs_stamp = amp;
  vs_write_bass();

  return;
}

s16 vs_getbassamp(void)
{
  return vs_sbamp;
}

void vs_setbassfreq(s16 freq) //20 - 150Hz
{
  freq /= 10;

  if(freq < 2) //< 2
  {
    freq = 2;
  }
  else if(freq > 15) //> 15
  {
    freq = 15;
  }
  vs_sbfreq = freq;
  vs_write_bass();

  return;
}

s16 vs_getbassfreq(void)
{
  return vs_sbfreq*10;
}

void vs_setbassamp(s16 amp) //0 - 15dB
{
  if(amp < 0) //< 0
  {
    amp = 0;
  }
  else if(amp > 15) //> 15
  {
    amp = 15;
  }
  vs_sbamp = amp;
  vs_write_bass();

  return;
}

s16 vs_getvolume(void)
{
  return vs_vol;
}

void vs_setvolume(s16 vol) //0 - 100%
{
  if(vol <= 0) //<= 0
  {
    vs_write_reg(VS_VOL, 0xFFFF); //analog power off
  }
  else if(vol > 100) //> 100
  {
    vs_vol = 100;
    if(vs_playing)
    {
      vs_write_volume();
    }
  }
  else //1 - 99
  {
    vs_vol = vol;
    if(vs_playing)
    {
      vs_write_volume();
    }
  }

  menu_drawvol();

  return;
}

void vs_write_bass(void)
{
  vs_write_reg(VS_BASS, ((vs_stamp&0x0f)<<12)|((vs_stfreq&0x0f)<<8)|((vs_sbamp&0x0f)<<4)|((vs_sbfreq&0x0f)<<0));

  return;
}

void vs_write_volume(void)
{
  s16 vol;

  vol = 100-vs_vol;
  vs_write_reg(VS_VOL, (vol<<8)|(vol<<0));

  return;
}

void vs_write_plugin(const u16 *plugin, u16 len)
{
  u16 i;
  u16 addr, n, val;

  for(i=0; i<len;)
  {
    addr = plugin[i++];
    n    = plugin[i++];
    if(n & 0x8000U) //RLE run, replicate n samples
    {
      n  &= 0x7FFF;
      val = plugin[i++];
      while(n--) 
      {
        vs_write_reg(addr, val);
      }
    }
    else //copy run, copy n sample
    {
      while(n--)
      {
        val = plugin[i++];
        vs_write_reg(addr, val);
      }
    }
  }

  return;
}

u16 vs_read_ram(u16 addr)
{
  u32 ret;

  vs_write_reg(VS_WRAMADDR, addr);
  ret = vs_read_reg(VS_WRAM);

  return ret;
}

void vs_write_reg(u16 reg, u16 data)
{
  u16 timeout;

  IntMasterDisable();

  vs_ssi_wait(); //wait for transfer complete
  VS_DCS_DISABLE();
  VS_CS_ENABLE();
  vs_ssi_readwrite(VS_WRITE);
  vs_ssi_readwrite(reg);
  vs_ssi_readwrite(data>>8);
  vs_ssi_readwrite(data);
  VS_CS_DISABLE();

  //execution -> DREQ low
  switch(reg)
  {
    case VS_MODE:     timeout = 48000; break;
    case VS_STATUS:   timeout =   240; break;
    case VS_BASS:     timeout =  2400; break;
    case VS_CLOCKF:   timeout = 48000; break;
    case VS_WRAM:     timeout =   240; break;
    case VS_WRAMADDR: timeout =   240; break;
    case VS_VOL:      timeout =  2400; break;
    default:          timeout =  2400; break;
  }
  //for(; timeout!=0; timeout--)
	while(timeout--)
  {
    if(vs_request())
    {
      break;
    }
  }
	IntMasterEnable();
	
  return;
}

void vs_ssi_wait(void)
{
  /* Loop while SPI3 busy */
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);

  SPI_I2S_ReceiveData(SPI3);

  return;
}

u8 vs_ssi_readwrite(u8 c)
{
	u8 i = 0;
  /* Loop while DR register in not empty */
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
  /* Send byte through the SPI3 peripheral */
  SPI_I2S_SendData(SPI3, c);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
  i = SPI_I2S_ReceiveData(SPI3);

  /* Return the byte read from the SPI bus */
  return i;
}

void vs_ssi_write(u8 c)
{
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
  SPI_I2S_SendData(SPI3, c);

  return;
}

u16 vs_request(void)
{
  return VS_DREQ_READ(); //1=ready, 0=buf full
}

u16 vs_read_reg(u16 reg)
{
  u16 ret, timeout;

  IntMasterDisable();

  vs_ssi_wait(); //wait for transfer complete
  VS_DCS_DISABLE();
  VS_CS_ENABLE();
  vs_ssi_readwrite(VS_READ);
  vs_ssi_readwrite(reg);
  ret  = vs_ssi_readwrite(0xff)<<8;
  ret |= vs_ssi_readwrite(0xff);
  VS_CS_DISABLE();

  //execution -> DREQ low
  switch(reg)
  {
    case VS_MODE:     timeout = 48000; break;
    case VS_STATUS:   timeout =   240; break;
    case VS_BASS:     timeout =  2400; break;
    case VS_CLOCKF:   timeout = 48000; break;
    case VS_WRAM:     timeout =   240; break;
    case VS_WRAMADDR: timeout =   240; break;
    case VS_VOL:      timeout =  2400; break;
    default:          timeout =  2400; break;
  }
  //for(; timeout!=0; timeout--)
	while(timeout--)
  {
    if(vs_request())
    {
      break;
    }
  }

  IntMasterEnable();

  return ret;
}

void vs_pause(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;											//disable dreq irq
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line5);															//clear flag

  return;
}

void vs_play(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
  if(vs_request() && vs_playing)
  {
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;											//enable dreq irq
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		EXTI_ClearITPendingBit(EXTI_Line5);															//clear flag
		EXTI_GenerateSWInterrupt(EXTI_Line5);
  }

  return;
}

void vs_stopstream(void)
{
  u16 i, timeout;

  vs_pause();

  vs_ssi_wait(); //wait for transfer complete
  VS_DCS_DISABLE();
  VS_CS_DISABLE();

  //cancel playback
  vs_write_reg(VS_MODE, SM_SDINEW | SM_CANCEL);
  for(timeout=100; (timeout!=0) && (vs_read_reg(VS_MODE) & SM_CANCEL); timeout--)
  {
    VS_DCS_ENABLE();
    for(i=32; i!=0; i--)
    {
      vs_ssi_write(vs_bufgetc());
    }
    vs_ssi_wait();
    VS_DCS_DISABLE();
  }

  //flush buffer
  if(vs_read_reg(VS_HDAT0) || vs_read_reg(VS_HDAT1))
  {
    for(timeout=12288/32; timeout!=0; timeout--) //for FLAC 12288 otherwise 2052
    {
      VS_DCS_ENABLE();
      for(i=32; i!=0; i--)
      {
        vs_ssi_write(0x00);
      }
      vs_ssi_wait();
      VS_DCS_DISABLE();
    }
  }

  //reset
  if(vs_read_reg(VS_HDAT0) || vs_read_reg(VS_HDAT1))
  {
    vs_write_reg(VS_MODE, SM_SDINEW | SM_RESET); //soft-reset
    delay_ms(10);
    if(vs_read_reg(VS_HDAT0) || vs_read_reg(VS_HDAT1))
    {
      vs_setvolume(0); //0 -> analog power off
      vs_reset(); //hard-reset
    }
  }

  return;
}

void vs_stop(void)
{
  DEBUGOUT("VS: stop\r\n");

  if(vs_playing)
  {
    vs_stopstream();
  }

  buf_reset();

  vs_playing = 0;

  return;
}

void vs_start(void)
{
  DEBUGOUT("VS: start\r\n");

  vs_playing = 1;

  vs_pause();
  vs_setvolume(vs_vol);

  buf_reset();

  return;
}

void vs_reset(void)
{
  u32 i;
	SPI_InitTypeDef	SPI_InitStructure;

  DEBUGOUT("VS: reset\r\n");

  //ssi speed down
  vs_ssi_wait(); //wait for transfer complete
  //vs_ssi_speed(2000000); //2 MHz

  //hard reset
  VS_CS_DISABLE();
  VS_DCS_DISABLE();
  VS_RST_ENABLE();
  delay_ms(10);
  VS_RST_DISABLE();
  delay_ms(20);
	
	//software reset
  vs_write_reg(VS_MODE, SM_RESET);
	delay_ms(10);

  //set registers
  vs_write_reg(VS_MODE, SM_SDINEW);

  //get VS version, set clock multiplier and load patch
  i = (vs_read_reg(VS_STATUS)&0xF0)>>4;
  if(i == 4)                                                         //VS1053
  {
    DEBUGOUT("VS: VS1053\r\n");
    vs_write_reg(VS_CLOCKF, 0x1800|VS1053_SC_MUL_4X);
#warning COMMENT FOR CHINESE CLONES
    DEBUGOUT("VS: load VS1053B patch\r\n");                            //VS1053B
    vs_write_plugin(vs1053b_patch, VS1053B_PATCHLEN);
  }
  else if(i == 5)                                                    //VS1033
  {
    DEBUGOUT("VS: VS1033\r\n");
    vs_write_reg(VS_CLOCKF, 0x1800|VS1033_SC_MUL_4X);
    i = vs_read_ram(0x1942); //extra parameter (0x1940) -> version (0x1942)
    if(i < 3)                                                        //VS1033C
    {
      DEBUGOUT("VS: load VS1033C patch\r\n");
      vs_write_plugin(vs1033c_patch, VS1033C_PATCHLEN);
    }
    else                                                             //VS1033D
    {
      DEBUGOUT("VS: load VS1033D patch\r\n");
      vs_write_plugin(vs1033d_patch, VS1033D_PATCHLEN);
    }
  }

  //ssi speed up
  //vs_ssi_speed(0); //0 = default speed
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) { ; }
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3, &SPI_InitStructure);

  return;
}

void vs_init(void)
{
  DEBUGOUT("VS: init\r\n");

  vs_playing = 0;

  //reset vs buffer
  vs_bufreset();

  //reset vs
  vs_reset();

  //set volume, bass, treble
  vs_setvolume(DEFAULT_VOLUME);
  vs_setbassfreq(DEFAULT_BASSFREQ);
  vs_setbassamp(DEFAULT_BASSAMP);
  vs_settreblefreq(DEFAULT_TREBLEFREQ); 
  vs_settrebleamp(DEFAULT_TREBLEAMP);

  return;
}
