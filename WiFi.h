// **********************************
// * WIFI                           *
// **********************************

#include <WiFiManager.h>
#include <ArduinoOTA.h>

void configModeCallback(WiFiManager *myWiFiManager);
void setup_ota();
void setup_mdns();
