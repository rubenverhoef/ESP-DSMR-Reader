#include <Arduino.h>
#include "WiFi.h"
#include "LED.h"
#include "P1Reader.h"

#include <ESP8266HTTPClient.h>

// * Include settings
#include "settings.h"

// **************************************
// * Send data to backend (DSMR-Reader) *
// **************************************
void send_data_to_backend()
{
    HTTPClient http;
    int httpCode = 0;

    if(http.begin(DSMR_READER_ADDRESS) == true)
    {
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("X-AUTHKEY", DSMR_READER_API);
        httpCode = http.POST("Data to Backend");
        http.writeToStream(&Serial);
    }
    http.end();

    if (httpCode != 201)
    {
        Serial.println(httpCode);
    }
    else
    {
        Serial.println("Send sucessfull to DSMR-Reader backend");
    }
    
}

// **********************************
// * Setup Main                     *
// **********************************

unsigned long last;
P1Reader reader(&Serial, D8);

void setup()
{
    // * Configure Serial
    Serial.begin(BAUD_RATE);

    // * Wifi reset to default
    pinMode(WIFI_DEFAULT_PIN, INPUT_PULLDOWN_16 );
    if (digitalRead(WIFI_DEFAULT_PIN) == HIGH)
    {
        WiFi.disconnect();
    }

    // * Set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // * Start LED blink with 0.6 because we start in AP mode and try to connect
    LED_Blink(0.6);

    // * WiFiManager local initialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // * Reset settings - uncomment for testing
    // wifiManager.resetSettings();

    // * Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    // * Set timeout
    wifiManager.setConfigPortalTimeout(WIFI_TIMEOUT);

    // * Fetches SSID and pass and tries to connect
    // * Reset when no connection after 10 seconds
    if (!wifiManager.autoConnect())
    {
        Serial.println(F("Failed to connect to WIFI and hit timeout"));

        // * Reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(WIFI_TIMEOUT);
    }

    // * If you get here you have connected to the WiFi
    Serial.println(F("Connected to WIFI..."));

    // * Keep LED on
    LED_Blink(0);

    // * Configure OTA
    setup_ota();

    // * Startup MDNS Service
    setup_mdns();

    // * Setup P1 DSMR Reader
    reader.enable(true);
    last = millis();
}

// **********************************
// * Loop                           *
// **********************************

void loop()
{
    // Allow the reader to check the serial buffer regularly
    reader.loop();

    // Every minute, fire off a one-off reading
    unsigned long now = millis();
    if (now - last > 60000)
    {
        reader.enable(true);
        last = now;
    }

    if (reader.available())
    {
        MyData data;
        String err;
        if (reader.parse(&data, &err))
        {
            // Parse succesful, print result
            data.applyEach(Printer());
        }
        else
        {
            // Parser error, print error
            Serial.println(err);
        }
    }
    ArduinoOTA.handle();
}
