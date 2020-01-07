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

// * DSMR-Reader Settings
#define DSMR_READER_ADDRESS "http://192.168.0.16:80/api/v2/datalogger/dsmrreading"
#define DSMR_READER_API "8KI4DM8J2I5I1WXUIOOECWQ4ZC3WFA25OX2LT829N3BNCULZNWHV1NLFPXD0YCBA"
