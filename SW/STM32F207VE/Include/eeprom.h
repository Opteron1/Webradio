#ifndef _EEPROM_H_
#define _EEPROM_H_	1U

#include "stm32f2xx.h"


/* Used EEPROM 24LC128 */

#define EEPROM_SIZE			16384	/* EEPROM size 16kB (128kb) */

#define EEPROM_ADDR_W		0x90
#define EEPROM_ADDR_R		0x91

#endif	/* _EEPROM_H_ */
