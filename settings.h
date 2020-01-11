#ifndef SETTINGS_H
#define SETTINGS_H

// **********************************
// * DEBUG                          *
// **********************************
#include <RemoteDebug.h>
extern RemoteDebug Debug;

// **********************************
// * WIFI                           *
// **********************************

// * Pin which should get to VCC at boot for Reset Wifi to Default 
// * (when using other GPIO than D0, you should enable the 
// * internal pullup and you have to connect the bin to GND to activate)
#define WIFI_DEFAULT_PIN D0

// * The hostname of our little creature
#define HOSTNAME "ESP_DSMR-Reader"

// * The password used for OTA
#define OTA_PASSWORD "admin"

// * Wifi timeout in milliseconds
#define WIFI_TIMEOUT 30000

// **********************************
// * P1 DSMR Reader                 *
// **********************************

// * Ready to send pin output
#define P1_RTS_PIN D8

// * Time zone of P1 message
#define TIMEZONE 1 // Timezone +1

// * Baud rate for both P1 and debug port
#define BAUD_RATE 115200

#endif /* SETTINGS_H */
