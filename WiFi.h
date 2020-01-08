// **********************************
// * WIFI                           *
// **********************************

#ifndef WIFI_H
#define WIFI_H

#include <WiFiManager.h>
#include <ArduinoOTA.h>

void configModeCallback(WiFiManager *myWiFiManager);
void setup_ota();
void setup_mdns();

#endif /* WIFI_H */
