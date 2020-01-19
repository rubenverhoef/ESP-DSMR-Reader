#include <Arduino.h>
#include <ESP8266HTTPClient.h>

#include "settings.h"
#include "Backend_DSMR-Reader.h"

// **********************************
// * Backend DSMR-Reader            *
// **********************************

String Format_HTTP_data(MyData data, MyData LastData)
{
    String httpData = ("");

    if (data.timestamp_present) // Always Required
    {
        httpData += "&timestamp=";
        httpData += formatDate(data.timestamp);
    }
    if (data.power_delivered_present) // Always Required
    {
        httpData += "&electricity_currently_delivered=";
        httpData += data.power_delivered;
    }
    if (data.power_returned_present) // Always Required
    {
        httpData += "&electricity_currently_returned=";
        httpData += data.power_returned;
    }
    if (data.energy_delivered_tariff1_present) // Always Required
    {
        httpData += "&electricity_delivered_1=";
        httpData += data.energy_delivered_tariff1;
    }
    if (data.energy_delivered_tariff2_present) // Always Required
    {
        httpData += "&electricity_delivered_2=";
        httpData += data.energy_delivered_tariff2;
    }
    if (data.energy_returned_tariff1_present) // Always Required
    {
        httpData += "&electricity_returned_1=";
        httpData += data.energy_returned_tariff1;
    }
    if (data.energy_returned_tariff2_present) // Always Required
    {
        httpData += "&electricity_returned_2=";
        httpData += data.energy_returned_tariff2;
    }
    if (data.power_delivered_l1_present && data.power_delivered_l1 != LastData.power_delivered_l1)
    {
        httpData += "&phase_currently_delivered_l1=";
        httpData += data.power_delivered_l1;
    }
    if (data.power_delivered_l2_present && data.power_delivered_l2 != LastData.power_delivered_l2)
    {
        httpData += "&phase_currently_delivered_l2=";
        httpData += data.power_delivered_l2;
    }
    if (data.power_delivered_l3_present && data.power_delivered_l3 != LastData.power_delivered_l3)
    {
        httpData += "&phase_currently_delivered_l3=";
        httpData += data.power_delivered_l3;
    }
    if (data.power_returned_l1_present && data.power_returned_l1 != LastData.power_returned_l1)
    {
        httpData += "&phase_currently_returned_l1=";
        httpData += data.power_returned_l1;
    }
    if (data.power_returned_l2_present && data.power_returned_l2 != LastData.power_returned_l2)
    {
        httpData += "&phase_currently_returned_l2=";
        httpData += data.power_returned_l2;
    }
    if (data.power_returned_l3_present && data.power_returned_l3 != LastData.power_returned_l3)
    {
        httpData += "&phase_currently_returned_l3=";
        httpData += data.power_returned_l3;
    }
    if (data.gas_delivered_present && data.gas_delivered != LastData.gas_delivered)
    {
        httpData += "&extra_device_timestamp=";
        httpData += formatDate(data.gas_delivered.timestamp);
        httpData += "&extra_device_delivered=";
        httpData += data.gas_delivered;
    }

    /* Remove first char of string (the &) */
    httpData = httpData.substring(1, httpData.length());
    Debug.println(httpData);
    return httpData;
}

extern char dsmrAPI[70]; // API key for DSMR-Reader backend
extern String dsmrHost; // api http address to DSMR-Reader backend
void Send_to_DSMR_Reader(MyData data, MyData LastData)
{
    HTTPClient http;
    int httpCode = 0;
    String payload;
    if(http.begin(dsmrHost) == true)
    {
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("X-AUTHKEY", dsmrAPI);
        httpCode = http.POST(Format_HTTP_data(data, LastData));
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
