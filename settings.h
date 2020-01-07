// **********************************
// * Ledstrip Settings              *
// **********************************

// * Pin which should get to VCC at boot for Reset Wifi to Default 
// * (when using other GPIO than D0, you should enable the 
// * internal pullup and you have to connect the bin to GND to activate)
#define WIFI_DEFAULT_PIN D0

// * Baud rate for both hardware and software serial
#define BAUD_RATE 115200

// * The hostname of our little creature
#define HOSTNAME "ESP_DSMR-Reader"

// * The password used for OTA
#define OTA_PASSWORD "admin"

// * Wifi timeout in milliseconds
#define WIFI_TIMEOUT 30000
