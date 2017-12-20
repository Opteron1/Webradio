#ifndef _ESP_SPI_
#define _ESP_SPI_

#include <stdint.h>

#define NUM_OF_BUFFS	4	//Count of packet buffers
typedef struct
{
	uint8_t		cmd;
	uint8_t		len;
	uint8_t		data[29];
	uint8_t		checksum;
	
}SPI_packet;

void spi_service(void);

void spi_set_packet(uint8_t * data);
uint8_t spi_buf_available(void);
uint8_t spi_packet_available(void);
#endif	//_ESP_SPI_