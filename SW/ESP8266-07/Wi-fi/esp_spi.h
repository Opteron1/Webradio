#ifndef _ESP_SPI_
#define _ESP_SPI_

#include <stdint.h>
#include <ESP8266WiFi.h>

/*
 * !!!! NOTE: This commands must be same(except comments) as in the ../../../STM32F207VE/Include/wifi.h !!!!
 */
// Commands for ESP8266
#define ESP8266_CMD0					0	//Software reset ESP8266
#define ESP8266_CMD1					1	//Send list of networks to the ARM
#define ESP8266_CMD2					2	//Connect to SSID
#define ESP8266_CMD3					3	//Send data
#define ESP8266_CMD4					4	//Get data
#define ESP8266_CMD5					5	//Disconnet from network
#define ESP8266_CMD6					6	//Get firmware version of ESP8266


#define NUM_OF_BUFFS	32	//Count of packet buffers
#define DATA_PART		28	//Count of data bytes in SPI packet
typedef struct
{
	uint8_t		cmd;
	uint8_t		packet_count;
	uint8_t		num_of_packet;
	uint8_t		data[DATA_PART];
	uint8_t		checksum;
	
}SPI_packet;


void spi_service(void);
uint8_t check_checksum(SPI_packet packet);
void spi_set_packet(uint8_t * data);
SPI_packet spi_get_packet(void);
uint8_t spi_buf_available(void);
uint8_t spi_packet_available(void);
#endif	//_ESP_SPI_