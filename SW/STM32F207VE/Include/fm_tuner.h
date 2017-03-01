#ifndef _FM_TUNER_H_
#define _FM_TUNER_H_	1U

#include "stm32f2xx.h"


//----- DEFINES -----
#define FM_STATION_FILE                   "FM_STATION.INI"


#define ADRW 	        0x20
#define ADRR 	        0x21

#define RDA5807_SEQ   0x10
#define RDA5807_RAND	0x11

#define RDA5807_TIMEOUT 1000U

#define RDA5807_OK_ERR						0
#define RDA5807_NOT_CONNECT_ERR		1
#define RDA5807_BAD_CHKSUM_ERR		2
#define RDA5807_HW_ERR						3

#define FM_TUNER_MUTE						0
#define FM_TUNER_STOP						1
#define FM_TUNER_PLAY						2
#define FM_TUNER_ERROR					3

#define ANT_TYPE_Off            0
#define ANT_TYPE_Headphones     1
#define ANT_TYPE_External       2
#define ANT_TYPE_Both           3


#define RDS_GROUP_A0	(0)
#define RDS_GROUP_A1	(2)
#define RDS_GROUP_A2	(4)
#define RDS_GROUP_A3	(6)
#define RDS_GROUP_A4	(8)
#define RDS_GROUP_A5	(10)
#define RDS_GROUP_A6	(12)
#define RDS_GROUP_A7	(14)
#define RDS_GROUP_A8	(16)
#define RDS_GROUP_A9	(18)
#define RDS_GROUP_A10	(20)
#define RDS_GROUP_A11	(22)
#define RDS_GROUP_A12	(24)
#define RDS_GROUP_A13	(26)
#define RDS_GROUP_A14	(28)
#define RDS_GROUP_A15	(30)

#define RDS_GROUP_B0	(RDS_GROUP_A0 | 1)
#define RDS_GROUP_B1	(RDS_GROUP_A1 | 1)
#define RDS_GROUP_B2	(RDS_GROUP_A2 | 1)
#define RDS_GROUP_B3	(RDS_GROUP_A3 | 1)
#define RDS_GROUP_B4	(RDS_GROUP_A4 | 1)
#define RDS_GROUP_B5	(RDS_GROUP_A5 | 1)
#define RDS_GROUP_B6	(RDS_GROUP_A6 | 1)
#define RDS_GROUP_B7	(RDS_GROUP_A7 | 1)
#define RDS_GROUP_B8	(RDS_GROUP_A8 | 1)
#define RDS_GROUP_B9	(RDS_GROUP_A9 | 1)
#define RDS_GROUP_B10	(RDS_GROUP_A10 | 1)
#define RDS_GROUP_B11	(RDS_GROUP_A11 | 1)
#define RDS_GROUP_B12	(RDS_GROUP_A12 | 1)
#define RDS_GROUP_B13	(RDS_GROUP_A13 | 1)
#define RDS_GROUP_B14	(RDS_GROUP_A14 | 1)
#define RDS_GROUP_B15	(RDS_GROUP_A15 | 1)

//00H
typedef struct __attribute__((packed))
{
	unsigned int	CHIP_ID			:16;		
}reg00h;
//02H
typedef struct __attribute__((packed))
{
  u16 enable            :1;    // 00 Power Up Enable (0 = Disabled; 1 = Enabled)    
  u16 soft_reset        :1;    // 01 Soft reset (0 = not reset; 1 = reset)
  u16 new_method        :1;    // 02 New Demodulate Method Enable, can improve the receive sensitivity about 1dB.   
  u16 rds_en            :1;    // 03 RDS/RBDS enable (1 = rds/rbds enable)    
  u16 clk_mode          :3;    // 4-6 „астота внешнего резонатора
  u16 skmode            :1;    // 07 Seek Mode (0 = wrap at the upper or lower band limit and continue seeking; 1 = stop seeking at the upper or lower band limit)
  u16 seek              :1;    // 08 Seek (0 = Disable stop seek; 1 = Enable)   
  u16 seekup            :1;    // 09 Seek Up (0 = Seek down; 1 = Seek up)
  u16 rclk_directinput  :1;    // 10 RCLK DIRECT INPUT MODE (1 = RCLK clock use the directly input mode)
  u16 rclk_noncalibmode :1;    // 11 RCLK NON-CALIBRATE MODE
  u16 bass              :1;    // 12 Bass Boost (0 = Disabled; 1 = Bass boost enabled)
  u16 mono              :1;    // 13 Mono Select (0 = Stereo; 1 = Force mono)
  u16 dmute             :1;    // 14 Mute Disable (0 = Mute; 1 = Normal operation)
  u16 dhiz              :1;    // 15 Audio Output High-Z Disable (0 = High impedance; 1 = Normal operation)	
}reg02h;
//03H
typedef struct __attribute__((packed))
{
  u16 space             :2;    // 0-1 Channel Spacing.
  // 00 = 100 kHz
  // 01 = 200 kHz
  // 10 = 50kHz
  // 11 = 25KHz
  u16 band              :2;    // 2-3 Band Select
  // 00 = 87Ц108 MHz (US/Europe)
  // 01 = 76Ц91 MHz (Japan)
  // 10 = 76Ц108 MHz (world wide)
  // 11 = 65 Ц76 MHz (East Europe) or 50-65MHz 
  u16 tune              :1;    // 04 Tune (0 = Disable; 1 = Enable)
  u16 direct_mode       :1;    // 05 Directly Control Mode, Only used when test.
  u16 chan              :10;   // 6-15 Channel Select 
  // BAND = 0          Frequency = Channel Spacing (kHz) x CHAN + 87.0 MHz
  // BAND = 1 or 2     Frequency = Channel Spacing (kHz) x CHAN + 76.0 MHz
  // BAND = 3          Frequency = Channel Spacing (kHz) x CHAN + 65.0 MHz
}reg03h;
//04H
typedef struct __attribute__((packed))
{
  u16 rsvd1             :8;    // 0-7 Reserved
  u16 afcd              :1;    // 08 AFC disable (0 = afc work; 1 = afc disabled)
  u16 softmute_en       :1;    // 09 1 = softmute enable
  u16 rsvd2             :1;    // 10 Reserved    
  u16 de                :1;    // 11 De-emphasis (0 = 75 µs; 1 = 50 µs)
  u16 rsvd3             :4;    // 12-15 Reserved  		
}reg04h;
//05H
typedef struct __attribute__((packed))
{
	u16 volume            :4;    // 0-3 Volume - DAC Gain Control Bits (0000=min; 1111=max)
  u16 ant_gain          :2;    // 4-5  оэффициент усилени€ антены (предположительно)
  u16 ant_type          :2;    // 6-7 “ип антены (см. константы ANT_TYPE)
  u16 seekth            :4;    // 8-11 Seek SNR threshold value
  u16 rsvd3             :3;    // 12-14 Reserved   
  u16 int_mode          :1;    // 15 INT MODE (0 = generate 5ms interrupt; 1 = interrupt last until read reg0CH action occurs)		
}reg05h;
//06H
typedef struct __attribute__((packed))
{
	u16 rsvd1             :13;   // 0-12 Resvered
  u16 open_mode         :2;    // 13-14 Open reserved register mode (11 = open behind registers writing function others: only open behind registers reading function
  u16 rsvd2             :1;    // 15 Reserved 		
}reg06h;
//07H
typedef struct __attribute__((packed))
{
	u16 freq_mode         :1;     // 00 If 1, then freq setting changed (Freq = 76000(or 87000) kHz + freq_direct (08H) kHz)
  u16 softblend_en      :1;     // 01 Softblend enable
  u16 seek_th_old       :6;     // 2-7 Seek threshold for old seek mode, Valid when Seek_Mode=001   
  u16 rsvd1             :1;     // 08 Resvered
  u16 mode_65m_50m      :1;     // 09 Valid when band[1:0] = 2Тb11 (0x03H_bit<3:2>) 
  // 1 = 65~76 MHz;
  // 0 = 50~76 MHz.
  u16 th_sofrblend      :5;     // 10-14 Threshold for noise soft blend setting, unit 2dB
  u16 rsvd2             :1;     // 15 Reserved   	
}reg07h;
//0AH
typedef struct __attribute__((packed))
{
  u16 readchan          :10;    // 0-9 Read Channel.
  // BAND = 0         Frequency = Channel Spacing (kHz) x READCHAN[9:0]+ 87.0 MHz
  // BAND = 1 or 2    Frequency = Channel Spacing (kHz) x READCHAN[9:0]+ 76.0 MHz
  // BAND = 3         Frequency = Channel Spacing (kHz) x READCHAN[9:0]+ 65.0 MHz
  u16 st                :1;     // 10 Stereo Indicator (0 = Mono; 1 = Stereo)
  u16 blk_e             :1;     // 11 When RDS enable (1 = Block E has been found; 0 = no Block E has been found)
  u16 rdss              :1;     // 12 RDS Synchronization (0 = RDS decoder not synchronized(default); 1 = RDS decoder synchronized)
  u16 sf                :1;     // 13 Seek Fail (0 = Seek successful; 1 = Seek failure)
  u16 stc               :1;     // 14 Seek/Tune Complete (0 = Not complete; 1 = Complete)
  u16 rdsr              :1;     // 15 RDS ready (0 = No RDS/RBDS group ready(default); 1 = New RDS/RBDS group ready)	
}reg0Ah;
//0BH
typedef struct __attribute__((packed))
{
  u16 blerb             :2;     // 0-1 Block Errors Level of RDS_DATA_1, and is always read as Errors Level of RDS BLOCK B (in RDS mode ) or E (in RBDS mode when ABCD_E flag is 1)
  u16 blera             :2;     // 2-3 Block Errors Level of RDS_DATA_0, and is always read as Errors Level of RDS BLOCK A (in RDS mode) or BLOCK E (in RBDS mode when ABCD_E flag is 1)
  u16 abcd_e            :1;     // 04 (1 = the block id of register 0cH,0dH,0eH,0fH is E; 0 = the block id of register 0cH, 0dH, 0eH,0fH is A, B, C, D)
  u16 rsvd1             :2;     // 5-6 Resvered
  u16 fm_ready          :1;     // 07 1 = ready; 0 = not ready
  u16 fm_true           :1;     // 08 1 = the current channel is a station; 0 = the current channel is not a station
  u16 rssi              :7;     // 9-15 RSSI (000000 = min; 111111 = max) RSSI scale is logarithmic
}reg0Bh;
//0CH
typedef struct __attribute__((packed))
{
	u16	rdsa   						:16;		// RDS data block A
}reg0Ch;
//0DH
typedef struct __attribute__((packed))
{
	u16	rdsb   						:16;		// RDS data block B
}reg0Dh;
//0EH
typedef struct __attribute__((packed))
{
	u16	rdsc   						:16;		// RDS data block C
}reg0Eh;
//0FH
typedef struct __attribute__((packed))
{
	u16	rdsd   						:16;		// RDS data block D
}reg0Fh;

typedef struct
{
	reg02h     Reg02;
	reg03h     Reg03;
	reg04h     Reg04;
	reg05h     Reg05;
	reg06h     Reg06;
	reg07h     Reg07;
	reg0Ah     Reg0A;
	reg0Bh     Reg0B;
	reg0Ch     Reg0C;
	reg0Dh     Reg0D;
	reg0Eh     Reg0E;
	reg0Fh     Reg0F;
} tuner_registers;

// RDS structures
typedef struct __attribute__((packed))
{
	u16 AF		:16;	//Alternative frequency of current station
	u8 	PSN[8];			//Program Service Name of listed station 8 chars
}rds_group_A0;

typedef struct __attribute__((packed))
{
	u16 PI		:16;	//Program Identification code
	u8 PSN[8];			//Program Service Name of listed station
}rds_group_B0;

typedef struct __attribute__((packed))
{
	u8 radiotext[64];	// Radiotext 64 chars
}rds_group_A2;

typedef struct __attribute__((packed))
{
	u16 PI			 :16;	//Program Identification code
	u8 radiotext[32];	// Radiotext
}rds_group_B2;

typedef struct
{
	rds_group_A0	A0;
	rds_group_B0	B0;
	rds_group_A2	A2;
	rds_group_B2	B2;
}RDS_DATA;


u16 fm_tuner_initialize(void);
void fm_tuner_read_seq(u16 *buffer, u8 len);
u16 fm_tuner_write_seq(u16 *buffer, u8 len);
void fm_tuner_bytes_change(u8 *buffer, u8 len);
void fm_tuner_read(u8 addr, u16 *buffer, u8 len);
void fm_tuner_write(u8 addr, u16 *buffer, u8 len);
void fm_tuner_set_volume(u8 volume);
u8 fm_tuner_get_volume(void);
void fm_tuner_set_bass_boost(u8 value);
void fm_tuner_mute(u8 value);
void fm_tuner_set_freq_in_100Khz(u16 Freq100kHz);
u16 fm_tuner_get_freq_in_100Khz(void);
void fm_tuner_start_seek(u8 up);
u8 fm_tuner_get_seek_tune_ready_flag(void);
u8 fm_tuner_get_rssi(void);
void fm_tuner_rds_decode(tuner_registers *registers);
void fm_tuner_timerservice(void);
void fm_tuner_service(void);
u16 fm_tuner_items(void);
void fm_tuner_getitem(u16 item, char *name);
u16 fm_tuner_getitemfreq(u16 item, char *freq);
u16 fm_tuner_openitem(u16 item);
void fm_tuner_closeitem(void);
void fm_tuner_init(void);
u16 fm_tuner_open(u16 item);
u16 fm_tuner_get_status(void);

static u8 fm_tuner_i2c_write_buf(u8 *buffer, u16 len, u16 timeout);
static s8 fm_tuner_i2c_start(u8 address, u8 read, u16 timeout);
static s8 fm_tuner_i2c_stop(u16 timeout);
static s8 fm_tuner_i2c_read_buff_and_stop(u8 *buffer, u16 len, u16 timeout);

#endif	/* _FM_TUNER_H_ */
