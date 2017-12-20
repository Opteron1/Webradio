#include "../Include/esp_spi.h"
#include "../Include/esp_debug.h"

SPI_packet packet_buffer[NUM_OF_BUFFS] = {0};
uint8_t packet_buffer_head = 0U;
uint8_t packet_buffer_tail = 0U;

void spi_service(void)
{
	if(spi_packet_available() == 0)
	{
		return;
	}
	
}

void spi_set_packet(uint8_t * data)
{
	uint8_t i;
	if(spi_buf_available())
	{
		if (packet_buffer_tail < NUM_OF_BUFFS-1)
		{
			packet_buffer[packet_buffer_tail].cmd = data[0];
			packet_buffer[packet_buffer_tail].len = data[1];
			for(i = 0; i < 30; i++)
			{
				packet_buffer[packet_buffer_tail].data[i+2] = data[i+2];
			}
			packet_buffer[packet_buffer_tail].checksum = data[31];
			packet_buffer_tail++;
		}
		else
		{
			packet_buffer[packet_buffer_tail].cmd = data[0];
			packet_buffer[packet_buffer_tail].len = data[1];
			for(i = 0; i < 30; i++)
			{
				packet_buffer[packet_buffer_tail].data[i+2] = data[i+2];
			}
			packet_buffer[packet_buffer_tail].checksum = data[31];
			packet_buffer_tail = 0;
		}
	}
	else	//No buffer available
	{
		dbgprint("All buffers are full.");
	}
}

uint8_t spi_buf_available(void)
{
	uint8_t head, tail;
	
	head = packet_buffer_head;
	tail = packet_buffer_tail;

	if(head > tail)
	{
		return (NUM_OF_BUFFS-(head-tail))-1;
	}
	else if(head < tail)
	{
		return (tail-head)-1;
	}

	return (NUM_OF_BUFFS-1);
}

uint8_t spi_packet_available(void)
{
	return (packet_buffer_head!=packet_buffer_tail ? 1 : 0);
}