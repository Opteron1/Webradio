#include "../Include/esp_network.h"

List_of_networks	available_networks = {0};



void scan_networks(void)
{
	int n = WiFi.scanNetworks();
	String buf;

	if(n != 0)
	{
		available_networks = {0};
		available_networks.number_of_networks = n;
		for (int i = 0; i < n; ++i)
    	{
    		buf = WiFi.SSID(i);
    		buf.toCharArray(available_networks.network[i].ssid, buf.length());
    		available_networks.network[i].rssi = WiFi.RSSI(i);

    		switch(WiFi.encryptionType(i))
    		{
    			case ENC_TYPE_NONE:
    				buf = {"    "};
    				buf.toCharArray(available_networks.network[i].ssid, 4);
    			case ENC_TYPE_WEP:	//WEP
    				buf = {"WEP "};
    				buf.toCharArray(available_networks.network[i].ssid, 4);
    			case ENC_TYPE_TKIP:	//WPA
    				buf = {"WPA "};
    				buf.toCharArray(available_networks.network[i].ssid, 4);
    			case ENC_TYPE_CCMP:	//WPA2
    				buf = {"WPA2"};
    				buf.toCharArray(available_networks.network[i].ssid, 4);
    			case ENC_TYPE_AUTO:	//WPA/WPA2
    				buf = {"WPA2"};
    				buf.toCharArray(available_networks.network[i].ssid, 4);
    		}
		}
	}
}

List_of_networks get_networks(void)
{
	return(available_networks);
}

