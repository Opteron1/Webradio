#include "wifi.h"
#include "mmc_io.h"

u8 wifi_esp8266_status = ESP8266_NOT_INSTALLED;
u32 wifi_esp8266_sw_version = 0UL;


u32 wifi_esp8266_get_fw_version(void)
{
	ESP8266_CS_ENABLE();
	ssi_write(ESP8266_CMD6);
	wifi_esp8266_sw_version = ssi_read() << 24;
	wifi_esp8266_sw_version |= ssi_read() << 16;
	wifi_esp8266_sw_version |= ssi_read() << 8;
	wifi_esp8266_sw_version |= ssi_read();
	ESP8266_CS_DISABLE();
	
	return wifi_esp8266_sw_version;
}

u8 wifi_esp8266_init(void)
{
	wifi_esp8266_hw_reset();
	wifi_esp8266_sw_reset();
	
	return (wifi_esp8266_status);
}

static u8 wifi_esp8266_sw_reset(void)
{
	ESP8266_CS_ENABLE();
	ssi_write(ESP8266_CMD0);
	ESP8266_CS_DISABLE();
	
	return 1;
}

static void wifi_esp8266_hw_reset(void)
{
u32 i = 100U;
	
	ESP8266_RESET_ENABLE();
	while(i--)
	{
		ESP8266_CS_TOGGLE();	// Needed for start booting of ESP8266
	}
	ESP8266_RESET_DISABLE();
	
	i = 5000000UL;
	while(i--);							// Wait some time while ESP8266 boots on
	ESP8266_CS_DISABLE();
}
