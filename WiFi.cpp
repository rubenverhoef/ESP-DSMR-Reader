#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "settings.h"
#include "WiFi.h"
#include "LED.h"

// * Initiate WIFI client
WiFiClient espClient;

// **********************************
// * WIFI                           *
// **********************************

// * Gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
    Debug.println(F("Entered config mode"));
    Debug.println(WiFi.softAPIP());

    // * If you used auto generated SSID, print it
    Debug.println(myWiFiManager->getConfigPortalSSID());

    // * Entered config mode, make led toggle faster
    LED_Blink(0.2);
}

// **********************************
// * Setup OTA                      *
// **********************************

void setup_ota()
{
    Debug.println(F("Arduino OTA activated."));

    // * Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // * Set hostname for OTA
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]()
    {
        Debug.println(F("Arduino OTA: Start"));
    });

    ArduinoOTA.onEnd([]()
    {
        Debug.println(F("Arduino OTA: End (Running reboot)"));
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
        Debug.printf("Arduino OTA Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
        Debug.printf("Arduino OTA Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Debug.println(F("Arduino OTA: Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            Debug.println(F("Arduino OTA: Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            Debug.println(F("Arduino OTA: Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            Debug.println(F("Arduino OTA: Receive Failed"));
        else if (error == OTA_END_ERROR)
            Debug.println(F("Arduino OTA: End Failed"));
    });

    ArduinoOTA.begin();
    Debug.println(F("Arduino OTA finished"));
}

// **********************************
// * Setup MDNS discovery service   *
// **********************************

void setup_mdns()
{
    Debug.println(F("Starting MDNS responder service"));

    bool mdns_result = MDNS.begin(HOSTNAME);
    if (mdns_result)
    {
        MDNS.addService("http", "tcp", 80);
    }
}
