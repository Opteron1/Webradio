#ifndef _IO_H_
#define _IO_H_		1U

#include "stm32f2xx.h"
#include "TFT.h"
#include "debug.h"
#include "stm32_eth.h"
#include "system_init.h"

//Check hardware config
#define STM32F207
#define LCD_SSD1289

#if defined STM32F207                //LM3S Rev
# define STM_NAME "STM32F207VET6"
#elif defined STM32F103RBT6
# define STM_NAME "STM32F103RBT6"
#elif defined STM32F103ZET6
# define STM_NAME "STM32F103ZET6"
#else
# warning "STM32F Rev not defined"
#endif

#if defined LCD_SSD1289
#define LCD_NAME "SSD1289"
# define LCD_WIDTH                     (320)
# define LCD_HEIGHT                    (240)
#elif defined LCD_L2F50                  //LCD
# define LCD_NAME "S65-L2F50"
#elif defined LCD_LPH88
# define LCD_NAME "S65-LPH88"
#elif defined LCD_LS020
# define LCD_NAME "S65-LS020"
#elif defined LCD_MIO283QT1
# define LCD_NAME "MIO283QT1"
#elif defined LCD_MIO283QT2
# define LCD_NAME "MIO283QT2"
#elif defined LCD_ILI9325
# define LCD_NAME "LCD_ILI9325"
#else
# warning "LCD not defined"
#endif


#define LCD_PWMSTANDBY                 (15)    //15 % (1...100%)

//Standby
#define STANDBY_TIME                   (3*60)  //standby after x seconds


//Rotary Encoder switch times
#define SW_SHORTTIME                   (8)   //  8*10ms =   80ms
#define SW_LONGTIME                    (120) //120*10ms = 1200ms

//IR
#define IR_BITTIME                     (1778) //1.778 ms
#define IR_MAXERR                      (150)  //150 us (max. half bit time error)
#define IR_DETECT                      (0)
#define IR_STARTBIT1                   (1)
#define IR_STARTBIT2                   (2)
#define IR_DATABIT                     (3)
#define IR_ALLADDR                     (0x1F)

//IR and Rotary Encoder Commandos
#define SW_PRESSED                     (1)
#define SW_PRESSEDLONG                 (2)
#define SW_VOLP                        (3)
#define SW_VOLM                        (4)
#define SW_UP                          (5)
#define SW_DOWN                        (6)
#define SW_ENTER                       (7)
#define SW_POWER                       (8)


void ethernet_setmac(uint64_t mac);
u16 ethernet_link(void);
void ethernet_put(u8 *pkt, u16 len);


void keys_timerservice(void);
s16 keys_sw(void);
s16 keys_steps(void);

s16 ir_cmd(void);
s16 ir_rawdata(void);
u16 ir_getaddr(void);
void ir_setaddr(u16 addr);
void ir_init(void);

u16 ir_getkeyvolm(void);
u16 ir_getkeyvolp(void);
u16 ir_getkeyok(void);
u16 ir_getkeydown(void);
u16 ir_getkeyup(void);
u16 ir_getkeypower(void);
void ir_setkeyvolm(u16 key);
void ir_setkeyvolp(u16 key);
void ir_setkeyok(u16 key);
void ir_setkeydown(u16 key);
void ir_setkeyup(u16 key);
void ir_setkeypower(u16 key);

void cpu_speed(u16 low_speed);
void cpu_reset(void);
void IntMasterDisable(void);
void IntMasterEnable(void);
void SysTickIntDisable(void);
void SysTickIntEnable(void);
void SysTickDisable(void);
void SysTickEnable(void);
void delay_ms(u32 ms);

#endif //_IO_H_
