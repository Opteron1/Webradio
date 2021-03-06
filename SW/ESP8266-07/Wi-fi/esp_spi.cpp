#include "esp_spi.h"
#include "esp_debug.h"
#include "esp_network.h"

// SPI packet buffers
static SPI_packet packet_buffer[NUM_OF_BUFFS] = {0};
static uint8_t packet_buffer_head = 0U;
static uint8_t packet_buffer_tail = 0U;


void spi_service(void)
{
	SPI_packet rx_packet;

	if(spi_packet_available() == 0)	// None packet received.
	{
		return;
	}
	// Process packet
	rx_packet = spi_get_packet();
	if	(check_checksum(rx_packet))	// At least one packet received and checksum is OK process it.
	{
		// Process packet
		switch(rx_packet.cmd)
		{
			case ESP8266_CMD0:	// Software reset ESP8266
				dbgprint("Soft reset CMD.");
				ESP.reset();
			case ESP8266_CMD1:	// Send list of networks to the ARM
				dbgprint("CMD1.");
				break;
			case ESP8266_CMD2:	// Connect to SSID
				//decode_cmd2();
				break;
			case ESP8266_CMD3:	// Send data

				break;
			case ESP8266_CMD4:	// Get data

				break;
			case ESP8266_CMD5:	// Disconnect from network

				break;
			case ESP8266_CMD6:	// Get firmware version of ESP8266

				break;
			default:
				dbgprint("Unknown command recieved.");
				break;
		}
	}
	else	// Packet corrupted - CHECKSUM isn't valid.
	{
		dbgprint("Corrupted packet.");
		// Send to the ARM information about corrupted packet.
	}
	dbgprint("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", rx_packet.cmd,
	rx_packet.packet_count, rx_packet.num_of_packet, rx_packet.data[0], rx_packet.data[1], rx_packet.data[2], rx_packet.data[3],
	rx_packet.data[4], rx_packet.data[5], rx_packet.data[6], rx_packet.data[7], rx_packet.data[8], rx_packet.data[9], rx_packet.data[10],
	rx_packet.data[11], rx_packet.data[12], rx_packet.data[13], rx_packet.data[14], rx_packet.data[15], rx_packet.data[16],
	rx_packet.data[17], rx_packet.data[18], rx_packet.data[19], rx_packet.data[20], rx_packet.data[21], rx_packet.data[22],
	rx_packet.data[23], rx_packet.data[24], rx_packet.data[25], rx_packet.data[26], rx_packet.data[27], rx_packet.data[28], rx_packet.checksum);
}

/*
 * Check checksum of received packet
 */
uint8_t check_checksum(SPI_packet packet)
{
	uint8_t i, chsum = 0;

	chsum = packet_buffer[packet_buffer_head].cmd;
	chsum += packet_buffer[packet_buffer_head].packet_count;
	chsum += packet_buffer[packet_buffer_head].num_of_packet;
	for(i = 0; i < DATA_PART; i++)
	{
		chsum += packet_buffer[packet_buffer_head].data[i];
	}
	if (chsum = packet_buffer[packet_buffer_head].checksum)
		return 1;
	else
		return 0;
}

/*
 * Packet was received from ARM over SPI, store it into the buffer.
 */
void spi_set_packet(uint8_t * data)
{
	uint8_t i;
	if(spi_buf_available())	// Space in buffer available, put packet into buffer.
	{
		if (packet_buffer_tail < NUM_OF_BUFFS-1)
		{
			packet_buffer[packet_buffer_tail].cmd = data[0];
			packet_buffer[packet_buffer_tail].packet_count = data[1];
			packet_buffer[packet_buffer_tail].num_of_packet = data[2];
			for(i = 0; i < DATA_PART; i++)
			{
				packet_buffer[packet_buffer_tail].data[i+3] = data[i+3];
			}
			packet_buffer[packet_buffer_tail].checksum = data[31];
			packet_buffer_tail++;
		}
		else
		{
			packet_buffer[packet_buffer_tail].cmd = data[0];
			packet_buffer[packet_buffer_tail].packet_count = data[1];
			packet_buffer[packet_buffer_tail].num_of_packet = data[2];
			for(i = 0; i < DATA_PART; i++)
			{
				packet_buffer[packet_buffer_tail].data[i+3] = data[i+3];
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

/*
 * Get packet from buffer.
 */
SPI_packet spi_get_packet(void)
{
	SPI_packet pac = packet_buffer[packet_buffer_head];

	if(packet_buffer_head < NUM_OF_BUFFS-1)
		packet_buffer_head++;
	else
		packet_buffer_head = 0;
	return pac;
}

/*
 * Check if any space in buffer are available.
 */
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

/*
 * New packet are available in buffer.
 */
uint8_t spi_packet_available(void)
{
	return ((packet_buffer_head!=packet_buffer_tail) ? 1 : 0);
}
