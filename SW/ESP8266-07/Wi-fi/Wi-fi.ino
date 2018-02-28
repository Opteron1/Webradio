#include "SPISlave.h"
#include "Include/esp_debug.h"
#include "Include/esp_spi.h"
#include "Include/esp_network.h"
#include <ESP8266WiFi.h>

extern "C"
{
#include "user_interface.h"
}

// Defines
#define NAME "Webradio SputnikFM"
#define VERSION "Wed, 01 Mar 2017 18:15:00 GMT"


// Digital I/O used
#define DATA_AVAIL          16U                     // GPIO16 interrupts STM32Fxxx for data available

#define RINGBUFSIZ          20000U                  // Ringbuffer for data. 20000 bytes is 160 Kbits, about 1.5 seconds at 128kb bitrate.

// Global variables
uint8_t*         ringbuf;                           // Ringbuffer for data


void setup()
{
  pinMode(DATA_AVAIL, OUTPUT);                         // This pin will interrupt the STM32Fxxx to get received data
  Serial.begin(115200UL);                              // For debug
  Serial.println();
  system_update_cpu_freq(160U);                        // Set to 80/160 MHz
  ringbuf = (uint8_t *) malloc(RINGBUFSIZ);            // Create ring buffer

  WiFi.persistent(false);                              // Do not save SSID and password
  WiFi.disconnect();                                   // After restart the router could still keep the old connection
  WiFi.mode(WIFI_STA);                                 // This ESP is a station
  wifi_station_set_hostname((char*)NAME);

  SPISlave.onData([](uint8_t * data, size_t len) {
#warning declare RX function here
    });
  
  SPISlave.begin();

  // Print some memory and sketch info
  dbgprint("Starting ESP Version %s...  Free memory %d",
             VERSION,
             system_get_free_heap_size());
  dbgprint("Sketch size %d, free size %d",
             ESP.getSketchSize(),
             ESP.getFreeSketchSpace());
}

void loop()
{
  spi_service();
}

