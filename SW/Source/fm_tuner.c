#include "fm_tuner.h"
#include "debug.h"
#include "menu.h"

tuner_registers tuner_buff;
RDS_DATA rds;
u16 fm_tuner_status = FM_TUNER_STOP;
u16 fm_tuner_item	= 0;
u8 refresh_tuner_data = 0;
u8 fm_tuner_volume = 0;

u16 fm_tuner_initialize(void)
{
u16 status = 0;
	
	// Register 0x02
	tuner_buff.Reg02.enable = 1;
  tuner_buff.Reg02.soft_reset = 0;
  tuner_buff.Reg02.new_method = 1;
  tuner_buff.Reg02.rds_en = 1;					// RDS ENABLE
  tuner_buff.Reg02.clk_mode = 0;        // 32.768
  tuner_buff.Reg02.skmode = 0;
  tuner_buff.Reg02.seek = 0;
  tuner_buff.Reg02.seekup = 1;
  tuner_buff.Reg02.rclk_directinput = 0;
  tuner_buff.Reg02.rclk_noncalibmode = 0;
  tuner_buff.Reg02.bass = 1;
  tuner_buff.Reg02.mono = 0;
  tuner_buff.Reg02.dmute = 1;
  tuner_buff.Reg02.dhiz = 0;
	// Register 0x03
	tuner_buff.Reg03.space = 0;
	tuner_buff.Reg03.band = 0;    // 87–108 MHz (US/Europe)
  tuner_buff.Reg03.tune = 1;
  tuner_buff.Reg03.direct_mode = 0;
  tuner_buff.Reg03.chan = 0;
	// Register 0x04
	tuner_buff.Reg04.rsvd1 = 0;
  tuner_buff.Reg04.afcd = 0;
  tuner_buff.Reg04.softmute_en = 1;
  tuner_buff.Reg04.rsvd2 = 0;
  tuner_buff.Reg04.de = 0;
  tuner_buff.Reg04.rsvd3 = 0;
	// Register 0x05
	tuner_buff.Reg05.volume = 15;
	fm_tuner_volume = tuner_buff.Reg05.volume;
  tuner_buff.Reg05.ant_gain = 0;
  tuner_buff.Reg05.ant_type = ANT_TYPE_External;//ANT_TYPE_External;//ANT_TYPE_Headphones;//ANT_TYPE_Both;
  tuner_buff.Reg05.seekth = 8;
  tuner_buff.Reg05.rsvd3 = 0;
  tuner_buff.Reg05.int_mode = 1;
	// Register 0x06
	tuner_buff.Reg06.rsvd1 = 0;
  tuner_buff.Reg06.open_mode = 0;
  tuner_buff.Reg06.rsvd2 = 0;
	// Register 0x07
	tuner_buff.Reg07.freq_mode = 0;
  tuner_buff.Reg07.softblend_en = 1;
  tuner_buff.Reg07.seek_th_old = 0;
  tuner_buff.Reg07.rsvd1 = 0;
  tuner_buff.Reg07.mode_65m_50m = 1;
  tuner_buff.Reg07.th_sofrblend = 16;
  tuner_buff.Reg07.rsvd2 = 0;
	
	DEBUGOUT("FM tuner: initialize\r\n");
	// Write register into RDA5807
	status = fm_tuner_write_seq((u16*)&(tuner_buff.Reg02), 6U);

return(status);
}

void fm_tuner_read_seq(u16 *buffer, u8 len)
{
  if (fm_tuner_i2c_start(RDA5807_SEQ, 1, RDA5807_TIMEOUT))
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }
  
  fm_tuner_i2c_read_buff_and_stop((u8 *)buffer, len << 1, RDA5807_TIMEOUT);
  fm_tuner_bytes_change((u8*)buffer, len << 1);
}

u16 fm_tuner_write_seq(u16 *buffer, u8 len)
{
	u8 error;
	u16 timeout_cnt = RDA5807_TIMEOUT;
	// Start
	error = fm_tuner_i2c_start(RDA5807_SEQ, 0, timeout_cnt);	
	if(error)
	{
		// Stop
		I2C_GenerateSTOP(I2C1, ENABLE);
		timeout_cnt = RDA5807_TIMEOUT;
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) && timeout_cnt);
		if (!timeout_cnt)
			error = RDA5807_HW_ERR;
	}
	else
	{	
		fm_tuner_bytes_change((u8*)buffer, len << 1);
		error = fm_tuner_i2c_write_buf((u8*)buffer, len << 1, RDA5807_TIMEOUT);
		// Stop
		I2C_GenerateSTOP(I2C1, ENABLE);
		timeout_cnt = RDA5807_TIMEOUT;
		while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) && timeout_cnt);
		if (!timeout_cnt)
			error = RDA5807_HW_ERR;

		fm_tuner_bytes_change((u8*)buffer, len << 1);
	}
	
	return(error);	
}

void fm_tuner_bytes_change(u8 *buffer, u8 len)
{
	while (len > 1)     // Если осталась хотя бы пара байт
  {
    u8 tmp = *(buffer+1);
    *(buffer+1) = *buffer;
    *buffer = tmp;
    buffer += 2;
    len -= 2;
  }
}

void fm_tuner_read(u8 addr, u16 *buffer, u8 len)
{
	//u8 error;
	//u16 timeout_cnt = RDA5807_TIMEOUT;
	
  if (fm_tuner_i2c_start(RDA5807_RAND, 0, RDA5807_TIMEOUT))
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }
  
  if (fm_tuner_i2c_write_buf(&addr, 1, RDA5807_TIMEOUT))
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }
  
  if (fm_tuner_i2c_start(RDA5807_RAND, 1, RDA5807_TIMEOUT))
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }

  fm_tuner_i2c_read_buff_and_stop((uint8_t *)buffer, len << 1, RDA5807_TIMEOUT);
  fm_tuner_bytes_change((uint8_t *)buffer, len << 1);
}

void fm_tuner_write(u8 addr, u16 *buffer, u8 len)
{
  int8_t err;
  
  err = fm_tuner_i2c_start(RDA5807_RAND, 0, RDA5807_TIMEOUT);
  if (err)
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }
 
  if (fm_tuner_i2c_write_buf(&addr, 1, RDA5807_TIMEOUT))
  {
    fm_tuner_i2c_stop(RDA5807_TIMEOUT);
    //rda5807_bus_init(I2Cx);
    return;
  }

  fm_tuner_bytes_change((uint8_t *)buffer, len << 1);

  err = fm_tuner_i2c_write_buf((uint8_t *)buffer, len << 1, RDA5807_TIMEOUT);
  fm_tuner_i2c_stop(RDA5807_TIMEOUT);
  
  fm_tuner_bytes_change((uint8_t *)buffer, len << 1);
}

void fm_tuner_set_volume(u8 volume)
{
  reg02h Reg02;
  reg05h Reg05;
  u8 mute = volume ? 0 : 1;
  
  if (volume > 15)
    volume = 15;
  
  //volume--;
	fm_tuner_volume = volume;

  if (!mute)
  {
    fm_tuner_read(0x05, (u16*) &Reg05, 1);
    Reg05.volume = volume;
    fm_tuner_write(0x05, (u16*) &Reg05, 1);
  }
    
  fm_tuner_read(0x02, (u16*) &Reg02, 1);
  Reg02.dmute = mute ? 0 : 1;
  fm_tuner_write(0x02, (u16*) &Reg02, 1);
}

u8 fm_tuner_get_volume(void)
{
	return fm_tuner_volume;
}

void fm_tuner_set_bass_boost(u8 value)
{
  reg02h Reg02;

  fm_tuner_read(0x02, (u16*) &Reg02, 1);
  Reg02.bass = (value) ? 1 : 0;
  fm_tuner_write(0x02, (u16*) &Reg02, 1);
}

void fm_tuner_mute(u8 value)
{
	reg02h Reg02;

  fm_tuner_read(0x02, (u16*) &Reg02, 1);
  Reg02.dhiz = (value) ? 0 : 1;
  fm_tuner_write(0x02, (u16*) &Reg02, 1);
}

void fm_tuner_set_freq_in_100Khz(u16 Freq100kHz)
{
  reg03h Reg03;
  
  if (Freq100kHz < 870)
    Freq100kHz = 870;
  if (Freq100kHz > 1080)
    Freq100kHz = 1080;
  
  Freq100kHz -= 870;

  fm_tuner_read(0x03, (u16*) &Reg03, 1);
  Reg03.chan = Freq100kHz;
  Reg03.tune = 1;
  fm_tuner_write(0x03, (u16*) &Reg03, 1);
}

u16 fm_tuner_get_freq_in_100Khz(void)
{
  reg0Ah Reg0A;
	u16 Freq100kHz = Reg0A.readchan;
  
  fm_tuner_read(0x0A, (u16*) &Reg0A, 1);
  
  if (Freq100kHz == 319)
    return 0;
  
  Freq100kHz += 870;
  
  return Freq100kHz;
}

void fm_tuner_start_seek(u8 up)
{
  reg02h Reg02;

  fm_tuner_read(0x02, (u16*) &Reg02, 1);
  
  Reg02.skmode = 1;          // 07 Seek Mode (0 = wrap at the upper or lower band limit and continue seeking; 1 = stop seeking at the upper or lower band limit)
  Reg02.seek = 1;            // 08 Seek (0 = Disable stop seek; 1 = Enable)   
  Reg02.seekup = up ? 1 : 0; // 09 Seek Up (0 = Seek down; 1 = Seek up)

  fm_tuner_write(0x02, (u16*) &Reg02, 1);
}

u8 fm_tuner_get_seek_tune_ready_flag(void)
{
  reg0Ah Reg0A;
  
  fm_tuner_read(0x0A, (u16*) &Reg0A, 1);

  return Reg0A.stc;
}

u8 fm_tuner_get_rssi(void)
{
	return tuner_buff.Reg0B.rssi;
}
	

void fm_tuner_rds_decode(tuner_registers *registers)
{
	u16 rds_group = (registers->Reg0D.rdsb >> 11U) & 0x1F;
	if((registers->Reg0B.blera != 0x0) | (registers->Reg0B.blerb != 0x0))	//If error in RDS occurs - return
		return;
	
	switch(rds_group) 
	{
		case RDS_GROUP_A0:
			rds.A0.AF = registers->Reg0E.rdsc;
			rds.A0.PSN[registers->Reg0D.rdsb & 0x03] = registers->Reg0F.rdsd >> 8;
			rds.A0.PSN[registers->Reg0D.rdsb & 0x03 + 1] = registers->Reg0F.rdsd & 0xFF;
			break;
		case RDS_GROUP_B0:
			rds.B0.PI = registers->Reg0E.rdsc;
			rds.B0.PSN[registers->Reg0D.rdsb & 0x03] = registers->Reg0F.rdsd >> 8;
			rds.B0.PSN[registers->Reg0D.rdsb & 0x03 + 1] = registers->Reg0F.rdsd & 0xFF;
			break;
		case RDS_GROUP_A2:
			rds.A2.radiotext[registers->Reg0D.rdsb & 0x0F] = registers->Reg0E.rdsc >> 8;
			rds.A2.radiotext[registers->Reg0D.rdsb & 0x0F + 1] = registers->Reg0E.rdsc & 0xFF;
			rds.A2.radiotext[registers->Reg0D.rdsb & 0x0F + 2] = registers->Reg0F.rdsd >> 8;
			rds.A2.radiotext[registers->Reg0D.rdsb & 0x0F + 3] = registers->Reg0F.rdsd & 0xFF;
			break;
		default:
			break;
	}
}

void fm_tuner_timerservice(void)		// 10Hz
{
		if(fm_tuner_status == FM_TUNER_PLAY)
			refresh_tuner_data = 1;
		else
			refresh_tuner_data = 0;
}

void fm_tuner_service(void)
{	
	if(refresh_tuner_data)
	{
		refresh_tuner_data = 0;
		fm_tuner_read_seq((u16*)&(tuner_buff.Reg0A), 6U);
		if(tuner_buff.Reg0A.rdsr)							//New RDS data received
		{
			fm_tuner_rds_decode(&tuner_buff);		//Decode RDS data
		}
	}	
}

u16 fm_tuner_items(void)
{
  char value[16];

  if(ini_getentry(FM_STATION_FILE, "NUMBEROFENTRIES", value, 16) == 0)
  {
    return atoi(value)+2; //add first entry ("<< back <<") and second entry SEARCH/TUNE
  }

  return 2;
}

void fm_tuner_getitem(u16 item, char *name)
{
  char entry[16];

  if(item == 0) //back
  {
    strcpy(name, MENU_BACKTXT);
  }
	else if(item == 1)	//Search/Tune
	{
		strcpy(name, MENU_SEARCHTXT);
	}
  else
  {
    sprintf(entry, "TITLE%i", item-1);	// item-1 -> search/tune is "item=1"
    if(ini_getentry(FM_STATION_FILE, entry, name, MAX_NAME) != 0)
    {
      sprintf(entry, "FILE%i", item-1);	// item-1 -> search/tune is "item=1"
      ini_getentry(FM_STATION_FILE, entry, name, MAX_NAME);
    }
  }

  return;
}

u16 fm_tuner_getitemfreq(u16 item, char *freq)
{
  char entry[16];

  *freq = 0;

  if(item)
  {
    sprintf(entry, "FILE%i", item);
    if(ini_getentry(FM_STATION_FILE, entry, freq, MAX_ADDR) == 0)
    {
      return 0;
    }
  }

  return 1;
}

u16 fm_tuner_openitem(u16 item)
{
  if(item == 0) //back
  {
    return MENU_BACK;
  }
	else if(item == 1)	//Search/tune
	{
		fm_tuner_start_seek(1);
#warning COMMENT - zde by se dalo definovat nove menu pro ladeni
#ifdef FHT_ENABLE
		fht_start();
#endif
		return MENU_PLAY;
	}
  else //play item
  {
		if(fm_tuner_open(item)!= FM_TUNER_ERROR)
		{
			fm_tuner_mute(0);
			DEBUGOUT("FM tuner: on\r\n");
#ifdef FHT_ENABLE
			fht_start();
#endif
			return MENU_PLAY;
		}
  }
  return MENU_ERROR;
}

void fm_tuner_closeitem(void)
{
	if(fm_tuner_status != FM_TUNER_STOP)
  {
#ifdef FHT_ENABLE
		fht_stop();
#endif
		fm_tuner_mute(1);
		fm_tuner_init();
		fm_tuner_status = FM_TUNER_STOP;
		menu_setstatus(MENU_STATE_STOP);
		DEBUGOUT("FM tuner: off\r\n");
	
		gbuf.tuner.name[0] = 0;
		gbuf.tuner.frequency[0] = 0;
		gbuf.tuner.name[MAX_NAME-1] = 0;
		gbuf.tuner.frequency[MAX_FREQ-1] = 0;
	}
  return;
}

void fm_tuner_init(void)
{
	DEBUGOUT("FM_tuner: init\r\n");
	fm_tuner_status = FM_TUNER_MUTE;
	fm_tuner_item   = 0;
	
	tuner_buff.Reg0C.rdsa = 0x0000;
	tuner_buff.Reg0D.rdsb = 0x0000;
	tuner_buff.Reg0E.rdsc = 0x0000;
	tuner_buff.Reg0F.rdsd = 0x0000;
	
	menu_setstatus(MENU_STATE_STOP);
}

u16 fm_tuner_open(u16 item)
{
	u16 freq;
	fm_tuner_getitem(item, gbuf.tuner.name);
	fm_tuner_getitemfreq(item-1, gbuf.tuner.frequency);		// item-1 -> search/tune is "item=1"
	freq = atou(gbuf.tuner.frequency);
	
	if((freq > 1080) | (freq < 870))
	{
		fm_tuner_status = FM_TUNER_ERROR;
		DEBUGOUT("FM tuner: error\r\n");
		return FM_TUNER_ERROR;
	}
	
	fm_tuner_set_freq_in_100Khz(freq);
	menu_setstatus(MENU_STATE_PLAY);
	freqtostr(freq, gbuf.tuner.frequency);
	fm_tuner_status = FM_TUNER_PLAY;
	
	return FM_TUNER_PLAY;
}

u16 fm_tuner_get_status(void)
{
	return (fm_tuner_status);
}

static u8 fm_tuner_i2c_write_buf(u8 *buffer, u16 len, u16 timeout)
{
	u16 timeout_cnt;
  
  while (len--)
  {
    I2C_SendData(I2C1, *(buffer++));
    timeout_cnt = timeout;
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && timeout_cnt) {timeout_cnt--;}
    if (!timeout_cnt)
      return RDA5807_NOT_CONNECT_ERR;
  }
  
  return RDA5807_OK_ERR;
}

static s8 fm_tuner_i2c_start(u8 address, u8 read, u16 timeout)
{
	u16 timeout_cnt;
  
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout_cnt = RDA5807_TIMEOUT;
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && timeout_cnt) {timeout_cnt--;}
  if (!timeout_cnt)
    return RDA5807_HW_ERR;
  
  if (read)
  {
    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Receiver);
    timeout_cnt = RDA5807_TIMEOUT;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && timeout_cnt) {timeout_cnt--;}
  }
  else
  {
    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Transmitter);
    timeout_cnt = RDA5807_TIMEOUT;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && timeout_cnt) {timeout_cnt--;}
  }
  
  if (!timeout_cnt)
      return RDA5807_NOT_CONNECT_ERR;
  
  return RDA5807_OK_ERR;
}

static s8 fm_tuner_i2c_stop(u16 timeout)
{
	u16 timeout_cnt = timeout;
	
	I2C_GenerateSTOP(I2C1, ENABLE);
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) && timeout_cnt);
  if (!timeout_cnt)
    return RDA5807_HW_ERR;
  
  return RDA5807_OK_ERR;
}

static s8 fm_tuner_i2c_read_buff_and_stop(u8 *buffer, u16 len, u16 timeout)
{
	u16 timeout_cnt;
  
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  while (len-- != 1)
  {
    timeout_cnt = timeout;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && timeout_cnt) {timeout_cnt--;}
    *buffer++ = I2C_ReceiveData(I2C1);
  }

  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1,ENABLE);

  timeout_cnt = timeout;
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && timeout_cnt) {timeout_cnt--;}
  *buffer++ = I2C_ReceiveData(I2C1);

  fm_tuner_i2c_stop(timeout);
  
  return RDA5807_OK_ERR;
}

