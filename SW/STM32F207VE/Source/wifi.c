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
	wifi_esp8266_ss_pulse();
	return (wifi_esp8266_status);
}

static void wifi_esp8266_ss_pulse(void)
{
	u8 i = 10U;
	ESP8266_CS_DISABLE();
	while(i--);					//Delay for generate short pulse for ESP8266 booting
	ESP8266_CS_ENABLE();
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
u8 i = 100U;
	
	ESP8266_RESET_ENABLE();
	while(i--);
	ESP8266_RESET_DISABLE();
}
