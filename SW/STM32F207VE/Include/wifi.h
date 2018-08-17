#ifndef _WIFI_H_
#define _WIFI_H_	1U

#include "stm32f2xx.h"

// ESP8266 states
#define ESP8266_NOT_INSTALLED		(0)
#define ESP8266_INSTALLED				(1)



// Commands for ESP8266
#define ESP8266_CMD0					0	//Software reset ESP8266
#define ESP8266_CMD1					1	//Get list of networks
#define ESP8266_CMD2					2	//Connect to SSID
#define ESP8266_CMD3					3	//Send data
#define ESP8266_CMD4					4	//Get data
#define ESP8266_CMD5					5	//Disconnet from network
#define ESP8266_CMD6					6	//Get firmware version of ESP8266


#define ESP8266_IRQ_READ()                 GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
#define ESP8266_CS_ENABLE()                GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET)
#define ESP8266_CS_DISABLE()               GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET)
#define ESP8266_CS_TOGGLE()								 GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
#define ESP8266_RESET_ENABLE()             GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET)
#define ESP8266_RESET_DISABLE()            GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET)




u32 wifi_esp8266_get_fw_version(void);
u8 wifi_esp8266_init(void);

static u8 wifi_esp8266_sw_reset(void);
static void wifi_esp8266_hw_reset(void);

#endif	/* _WIFI_H_ */
