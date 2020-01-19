#include <Arduino.h>
#include <PubSubClient.h>

#include "settings.h"
#include "WiFi.h"
#include "Backend_MQTT.h"

// **********************************
// * Backend MQTT                   *
// **********************************

// * WiFi client from WiFi.cpp
extern WiFiClient espClient;

// * Setting from EEPROM
extern char mqttIP[16];
extern char mqttPort[6];
extern char mqttUser[32];
extern char mqttPass[32];

// * Initiate MQTT client
PubSubClient mqtt_client(espClient);

// * Funciton protype
void send_metric(String name, double metric);
void send_metric_string(String name, String metric);

void Send_to_MQTT(MyData data, MyData LastData)
{
    if (mqtt_client.connected())
    {
        if (data.power_delivered_present && data.power_delivered != LastData.power_delivered)
        {
            send_metric("power_delivered", data.power_delivered);
        }
        if (data.power_returned_present && data.power_returned != LastData.power_returned)
        {
            send_metric("power_returned", data.power_returned);
        }
        if (data.energy_delivered_tariff1_present && data.energy_delivered_tariff1 != LastData.energy_delivered_tariff1)
        {
            send_metric("energy_delivered_tariff1", data.energy_delivered_tariff1);
        }
        if (data.energy_delivered_tariff2_present && data.energy_delivered_tariff2 != LastData.energy_delivered_tariff2)
        {
            send_metric("energy_delivered_tariff2", data.energy_delivered_tariff2);
        }
        if (data.energy_returned_tariff1_present && data.energy_returned_tariff1 != LastData.energy_returned_tariff1)
        {
            send_metric("energy_returned_tariff1", data.energy_returned_tariff1);
        }
        if (data.energy_returned_tariff2_present && data.energy_returned_tariff2 != LastData.energy_returned_tariff2)
        {
            send_metric("energy_returned_tariff2", data.energy_returned_tariff2);
        }
        if (data.power_delivered_l1_present && data.power_delivered_l1 != LastData.power_delivered_l1)
        {
            send_metric("power_delivered_l1", data.power_delivered_l1);
        }
        if (data.power_delivered_l2_present && data.power_delivered_l2 != LastData.power_delivered_l2)
        {
            send_metric("power_delivered_l2", data.power_delivered_l2);
        }
        if (data.power_delivered_l3_present && data.power_delivered_l3 != LastData.power_delivered_l3)
        {
            send_metric("power_delivered_l3", data.power_delivered_l3);
        }
        if (data.power_returned_l1_present && data.power_returned_l1 != LastData.power_returned_l1)
        {
            send_metric("power_returned_l1", data.power_returned_l1);
        }
        if (data.power_returned_l2_present && data.power_returned_l2 != LastData.power_returned_l2)
        {
            send_metric("power_returned_l2", data.power_returned_l2);
        }
        if (data.power_returned_l3_present && data.power_returned_l3 != LastData.power_returned_l3)
        {
            send_metric("power_returned_l3", data.power_returned_l3);
        }
        if (data.gas_delivered_present && data.gas_delivered != LastData.gas_delivered)
        {
            send_metric("gas_delivered", data.gas_delivered);
        }
        if (data.current_l1_present && data.current_l1 != LastData.current_l1)
        {
            send_metric("current_l1", data.current_l1);
        }
        if (data.current_l2_present && data.current_l2 != LastData.current_l2)
        {
            send_metric("current_l2", data.current_l2);
        }
        if (data.current_l3_present && data.current_l3 != LastData.current_l3)
        {
            send_metric("current_l3", data.current_l3);
        }
        if (data.electricity_tariff_present && data.electricity_tariff != LastData.electricity_tariff)
        {
            send_metric_string("electricity_tariff", data.electricity_tariff);
        }
        if (data.electricity_failures_present && data.electricity_failures != LastData.electricity_failures)
        {
            send_metric("electricity_failures", data.electricity_failures);
        }
        if (data.electricity_long_failures_present && data.electricity_long_failures != LastData.electricity_long_failures)
        {
            send_metric("electricity_long_failures", data.electricity_long_failures);
        }
        if (data.electricity_sags_l1_present && data.electricity_sags_l1 != LastData.electricity_sags_l1)
        {
            send_metric("electricity_sags_l1", data.electricity_sags_l1);
        }
        if (data.electricity_sags_l2_present && data.electricity_sags_l2 != LastData.electricity_sags_l2)
        {
            send_metric("electricity_sags_l2", data.electricity_sags_l2);
        }
        if (data.electricity_sags_l3_present && data.electricity_sags_l3 != LastData.electricity_sags_l3)
        {
            send_metric("electricity_sags_l3", data.electricity_sags_l3);
        }
        if (data.electricity_swells_l1_present && data.electricity_swells_l1 != LastData.electricity_swells_l1)
        {
            send_metric("electricity_swells_l1", data.electricity_swells_l1);
        }
        if (data.electricity_swells_l2_present && data.electricity_swells_l2 != LastData.electricity_swells_l2)
        {
            send_metric("electricity_swells_l2", data.electricity_swells_l2);
        }
        if (data.electricity_swells_l3_present && data.electricity_swells_l3 != LastData.electricity_swells_l3)
        {
            send_metric("electricity_swells_l3", data.electricity_swells_l3);
        }
    }
}

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, const char *payload)
{
    Debug.printf("MQTT Outgoing on %s: ", topic);
    Debug.println(payload);

    bool result = mqtt_client.publish(topic, payload, false);

    if (!result)
    {
        Debug.printf("MQTT publish to topic %s failed\n", topic);
    }
}

// * Reconnect to MQTT server and subscribe to in and out topics
bool mqtt_reconnect()
{
    if (!mqtt_client.connected())
    {
        // * Attempt to connect
        if (mqtt_client.connect(HOSTNAME, mqttUser, mqttPass))
        {
            Debug.println(F("MQTT connected!"));

            // * Once connected, publish an announcement...
            char *message = new char[16 + strlen(HOSTNAME) + 1];
            strcpy(message, "p1 meter alive: ");
            strcat(message, HOSTNAME);
            mqtt_client.publish("hass/status", message);

            Debug.printf("MQTT root topic: %s\n", MQTT_ROOT_TOPIC);
        }
        else
        {
            Debug.print(F("MQTT Connection failed: rc="));
            Debug.println(mqtt_client.state());
            Debug.println(F("Retrying in 30 seconds"));
            return false;
        }
    }

    return true;
}

//* Connecto to MQTT broker
void MQTT_connect(void)
{
    Debug.printf("MQTT connecting to: %s:%s\n", mqttIP, mqttPort);

    mqtt_client.setServer(mqttIP, atoi(mqttPort));
    mqtt_reconnect();
}


void send_metric(String name, double metric)
{
    char output[64+1];
    snprintf(output, sizeof(output), "%g", metric);
    send_metric_string(name, output);
}

void send_metric_string(String name, String metric)
{
    Debug.print(F("Sending metric to broker: "));
    Debug.print(name);
    Debug.print(F("="));
    Debug.println(metric);

    String topic = String(MQTT_ROOT_TOPIC) + "/" + name;
    send_mqtt_message(topic.c_str(), metric.c_str());
}

// * MQTT Last reconnection counter
long LAST_RECONNECT_ATTEMPT = 0;

// * MQTT handle 
void MQTT_handle(void)
{
    if (!mqtt_client.connected())
    {
        long now = millis();

        if (now - LAST_RECONNECT_ATTEMPT >= (MQTT_RECONNECT_TIMEOUT * 1000))
        {
            if (mqtt_reconnect() == true)
            {
                LAST_RECONNECT_ATTEMPT = 0;
            }
            else
            {
                LAST_RECONNECT_ATTEMPT = now;
            }
            
        }
    }
    else
    {
        mqtt_client.loop();
    }
}
