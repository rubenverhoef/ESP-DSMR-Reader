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
void send_metric(String name, long metric);

void Send_to_MQTT(MyData data)
{
    send_metric("consumption_low_tarif", data.energy_delivered_tariff1);
    send_metric("consumption_high_tarif", data.energy_delivered_tariff2);
    send_metric("actual_consumption", data.power_delivered);
    send_metric("instant_power_usage", data.power_delivered);
    send_metric("gas_meter_m3", data.gas_delivered);
}

//* Connecto to MQTT broker
void MQTT_connect(void)
{
    Debug.printf("MQTT connecting to: %s:%s\n", mqttIP, mqttPort);

    mqtt_client.setServer(atoi(mqttIP), atoi(mqttPort));
}

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, char *payload)
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
    // * Loop until we're reconnected
    int MQTT_RECONNECT_RETRIES = 0;

    while (!mqtt_client.connected() && MQTT_RECONNECT_RETRIES < MQTT_MAX_RECONNECT_TRIES)
    {
        MQTT_RECONNECT_RETRIES++;
        Debug.printf("MQTT connection attempt %d / %d ...\n", MQTT_RECONNECT_RETRIES, MQTT_MAX_RECONNECT_TRIES);

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
            Debug.println(F(" Retrying in 5 seconds"));
            Debug.println("");

            // * Wait 5 seconds before retrying
            delay(5000);
        }
    }

    if (MQTT_RECONNECT_RETRIES >= MQTT_MAX_RECONNECT_TRIES)
    {
        Debug.printf("*** MQTT connection failed, giving up after %d tries ...\n", MQTT_RECONNECT_RETRIES);
        return false;
    }

    return true;
}

void send_metric(String name, long metric)
{
    Debug.print(F("Sending metric to broker: "));
    Debug.print(name);
    Debug.print(F("="));
    Debug.println(metric);

    char output[10];
    ltoa(metric, output, sizeof(output));

    String topic = String(MQTT_ROOT_TOPIC) + "/" + name;
    send_mqtt_message(topic.c_str(), output);
}

// * MQTT Last reconnection counter
long LAST_RECONNECT_ATTEMPT = 0;

// * MQTT handle 
void MQTT_handle(void)
{
    if (!mqtt_client.connected())
    {
        long now = millis();

        if (now - LAST_RECONNECT_ATTEMPT > 5000)
        {
            LAST_RECONNECT_ATTEMPT = now;

            if (mqtt_reconnect())
            {
                LAST_RECONNECT_ATTEMPT = 0;
            }
        }
    }
    else
    {
        mqtt_client.loop();
    }
}
