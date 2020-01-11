#include <Arduino.h>
#include <ESP8266HTTPClient.h>

#include "settings.h"
#include "Backend_DSMR-Reader.h"

// **********************************
// * Backend DSMR-Reader            *
// **********************************

String Format_HTTP_data(MyData data)
{
    String httpData = ("");

    if (data.timestamp_present)
    {
        httpData += "&timestamp=";
        httpData += formatDate(data.timestamp);
    }
    if (data.power_delivered_present)
    {
        httpData += "&electricity_currently_delivered=";
        httpData += data.power_delivered;
    }
    if (data.energy_delivered_tariff1_present)
    {
        httpData += "&electricity_delivered_1=";
        httpData += data.energy_delivered_tariff1;
    }
    if (data.energy_delivered_tariff2_present)
    {
        httpData += "&electricity_delivered_2=";
        httpData += data.energy_delivered_tariff2;
    }
    if (data.energy_returned_tariff1_present)
    {
        httpData += "&electricity_returned_1=";
        httpData += data.energy_returned_tariff1;
    }
    if (data.energy_returned_tariff2_present)
    {
        httpData += "&electricity_returned_2=";
        httpData += data.energy_returned_tariff2;
    }
    if (data.power_delivered_l1_present)
    {
        httpData += "&phase_currently_delivered_l1=";
        httpData += data.power_delivered_l1;
    }
    if (data.power_delivered_l2_present)
    {
        httpData += "&phase_currently_delivered_l2=";
        httpData += data.power_delivered_l2;
    }
    if (data.power_delivered_l3_present)
    {
        httpData += "&phase_currently_delivered_l3=";
        httpData += data.power_delivered_l3;
    }
    if (data.power_returned_l1_present)
    {
        httpData += "&phase_currently_returned_l1=";
        httpData += data.power_returned_l1;
    }
    if (data.power_returned_l2_present)
    {
        httpData += "&phase_currently_returned_l2=";
        httpData += data.power_returned_l2;
    }
    if (data.power_returned_l3_present)
    {
        httpData += "&phase_currently_returned_l3=";
        httpData += data.power_returned_l3;
    }
    if (data.gas_delivered_present)
    {
        httpData += "&extra_device_timestamp=";
        httpData += formatDate(data.gas_delivered.timestamp);
        httpData += "&extra_device_delivered=";
        httpData += data.gas_delivered;
    }

    /* Remove first char of string (the &) */
    httpData = httpData.substring(1, httpData.length());
    return httpData;
}

extern char dsmrAPI[70]; // API key for DSMR-Reader backend
extern String dsmrHost; // api http address to DSMR-Reader backend
void Send_to_DSMR_Reader(MyData data)
{
    HTTPClient http;
    int httpCode = 0;
    String payload;
    if(http.begin(dsmrHost) == true)
    {
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("X-AUTHKEY", dsmrAPI);
        httpCode = http.POST(Format_HTTP_data(data));
        payload = http.getString();
    }
    http.end();

    if (httpCode != 201)
    {
        Debug.printf("HTTP response: httpCode: %d\n\r", httpCode);
        Debug.printf("%s\n\r", payload.c_str());
    }
    else
    {
        Debug.printf("Send sucessfull to DSMR-Reader backend\n\r");
    }
    
}
