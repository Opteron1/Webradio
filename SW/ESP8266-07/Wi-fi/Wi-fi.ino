#include "SPISlave.h"
#include <ESP8266WiFi.h>

extern "C"
{
#include "user_interface.h"
}

// Defines
#define NAME "Webradio SputnikFM"
#define VERSION "Wed, 01 Mar 2017 18:15:00 GMT"
#define DEBUG_BUFFER_SIZE   100U                    //Debug buffer size
#define DEBUG               1U                      //Enable debug messages

// Digital I/O used
#define DATA_AVAIL          16U                     // GPIO16 interrupts STM32Fxxx for data available

#define RINGBUFSIZ          20000U                  // Ringbuffer for data. 20000 bytes is 160 Kbits, about 1.5 seconds at 128kb bitrate.

char* dbgprint(const char* format, ...);

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

}


char* dbgprint(const char* format, ...)
{
  static char sbuf[DEBUG_BUFFER_SIZE];                 // For debug lines
  va_list varArgs;                                     // For variable number of params

  va_start(varArgs, format);                           // Prepare parameters
  vsnprintf(sbuf, sizeof(sbuf), format, varArgs);      // Format the message
  va_end(varArgs) ;                                    // End of using parameters
#ifdef  DEBUG                                          // DEBUG on?
    Serial.print("D: ");                               // Yes, print prefix
    Serial.println(sbuf);                              // and the info
#endif
  return sbuf ;                                        // Return stored string
}

