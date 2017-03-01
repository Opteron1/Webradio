#ifndef _VS_H_
#define _VS_H_	1U

#include "stm32f2xx.h"
#include "vs_patch.h"
#include "menu.h"

//----- DEFINES -----
#define DEFAULT_VOLUME                 (40)     //   0 -   100 %
#define DEFAULT_BASSAMP                (5)      //   0 -    15 dB
#define DEFAULT_BASSFREQ               (100)    //  20 -   150 Hz
#define DEFAULT_TREBLEAMP              (0)      //  -8 -     7 dB
#define DEFAULT_TREBLEFREQ             (15000)  //1000 - 15000 Hz

//VS FiFo
#define VS_BUFSIZE                     (42*1024) //42 kBytes

//VS Type
#define VS_UNKNOWN                     (0)
#define VS1033C                        (1)
#define VS1033D                        (2)
#define VS1053B                        (3)

//Audio Format
#define FORMAT_UNKNOWN                 (0)
#define FORMAT_WAV                     (1)
#define FORMAT_MP3                     (2)
#define FORMAT_AAC                     (3)
#define FORMAT_OGG                     (4)
#define FORMAT_WMA                     (5)
#define FORMAT_FLAC                    (6)

//Clock
#define VS_XTAL                        (12288000UL)
//Opcode
#define VS_READ                        (0x03)
#define VS_WRITE                       (0x02)
//Register
#define VS_MODE                        (0x00)   //Mode control
#define SM_RESET                       (1<< 2)  //Soft Reset
#define SM_CANCEL                      (1<< 3)  //Cancel Decoding
#define SM_STREAM                      (1<< 6)  //Stream Mode
#define SM_SDINEW                      (1<<11)  //VS1002 native SPI modes
#define VS_STATUS                      (0x01)   //Status
#define VS_BASS                        (0x02)   //Built-in bass/treble enhancer
#define VS_CLOCKF                      (0x03)   //Clock freq + multiplier
#define VS1033_SC_MUL_2X               (0x4000)
#define VS1033_SC_MUL_3X               (0x8000)
#define VS1033_SC_MUL_4X               (0xC000)
#define VS1053_SC_MUL_2X               (0x2000)
#define VS1053_SC_MUL_3X               (0x6000)
#define VS1053_SC_MUL_4X               (0xA000)
#define VS_DECODETIME                  (0x04)   //Decode time in seconds
#define VS_AUDATA                      (0x05)   //Misc. audio data
#define VS_WRAM                        (0x06)   //RAM write/read
#define VS_WRAMADDR                    (0x07)   //Base address for RAM write/read
#define VS_HDAT0                       (0x08)   //Stream header data 0
#define VS_HDAT1                       (0x09)   //Stream header data 1
#define VS_AIADDR                      (0x0A)   //Start address of application
#define VS_VOL                         (0x0B)   //Volume control
//RAM Data
#define VS_RAM_ENDFILLBYTE             (0x1E06)  //End fill byte

typedef union
{
  uint8_t  b8[VS_BUFSIZE];
  uint16_t b16[VS_BUFSIZE/2];
  uint32_t b32[VS_BUFSIZE/4];
} VSBUFFER;

//----- GLOBALS -----
extern VSBUFFER vs_buf;

#define VS_DREQ_READ()                 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);
#define VS_RST_DISABLE()               GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);
#define VS_RST_ENABLE()                GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_RESET);
#define VS_CS_DISABLE()                GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);
#define VS_CS_ENABLE()                 GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
#define VS_DCS_DISABLE()               GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_SET);
#define VS_DCS_ENABLE()                GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET);


void vs_handler(void);
u8 vs_bufgetc(void);
void vs_bufputs(const u8 *s, u16 len);
u16 vs_buffree(void);
u16 vs_buflen(void);
void vs_bufsethead(u16 head);
void vs_bufreset(void);
s16 vs_gettreblefreq(void);
void vs_settreblefreq(s16 freq);
s16 vs_gettrebleamp(void);
void vs_settrebleamp(s16 amp);
void vs_setbassfreq(s16 freq);
s16 vs_getbassfreq(void);
s16 vs_getbassamp(void);
void vs_setbassamp(s16 amp);
s16 vs_getvolume(void);
void vs_setvolume(s16 vol);
void vs_write_bass(void);
void vs_write_volume(void);
void vs_write_plugin(const u16 *plugin, u16 len);
u16 vs_read_ram(u16 addr);
void vs_write_reg(u16 reg, u16 data);
u16 vs_request(void);
void vs_ssi_wait(void);
u8 vs_ssi_readwrite(u8 c);
void vs_ssi_write(u8 c);
u16 vs_read_reg(u16 reg);
void vs_pause(void);
void vs_play(void);
void vs_stopstream(void);
void vs_stop(void);
void vs_start(void);
void vs_reset(void);
void vs_init(void);

#endif	/* _VS_H_ */
