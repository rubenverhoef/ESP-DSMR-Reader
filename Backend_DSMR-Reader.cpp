#include <Arduino.h>
#include <ESP8266HTTPClient.h>

#include "settings.h"
#include "Backend_DSMR-Reader.h"

// **********************************
// * Backend DSMR-Reader            *
// **********************************

void Send_to_DSMR_Reader()
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
