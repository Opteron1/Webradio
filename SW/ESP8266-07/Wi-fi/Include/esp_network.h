#ifndef _ESP_NETWORK_
#define _ESP_NETWORK_

#include <stdint.h>
#include <string.h>
#include <ESP8266WiFi.h>


typedef struct
{
	char		ssid[32];		//Maximum characters in SSID is 32
	long		rssi;			//Signal strength
	char		encryption[4];	//WEP, WPA, WPA2, NONE, AUTO
}record;

typedef struct 
{
	uint8_t			number_of_networks;
	record 			network[64];
}List_of_networks;



void scan_networks(void);
List_of_networks get_networks(void);

#endif	//_ESP_NETWORK_
