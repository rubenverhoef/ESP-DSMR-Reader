#include <Arduino.h>
#include "EEPROM_Handler.h"
#include "WiFi.h"
#include "LED.h"
#include "P1Reader.h"
#include "Backend_DSMR-Reader.h"
#include "Backend_MQTT.h"

// * Include settings
#include "settings.h"

unsigned long last;
P1Reader reader(&Serial, P1_RTS_PIN);
RemoteDebug Debug;

bool MQTT_enabled = false;
bool DSMR_Reader_enabled = false;
unsigned int scanInterval_INT = 0;

char scanInterval[3] = ""; // Scan interval of readings
char dsmrIP[16] = ""; // IP to DSMR-Reader backend
char dsmrPort[6] = ""; // Port to DSMR-Reader backend
char dsmrAPI[70] = ""; // API key for DSMR-Reader backend
String dsmrHost;

char mqttIP[16] = "";
char mqttPort[6]  = "";
char mqttUser[32] = "";
char mqttPass[32] = "";

WiFiManagerParameter CUSTOM_scanInterval("scanInterval", "Scan Interval in Seconds", scanInterval, 3);
WiFiManagerParameter CUSTOM_DSMR_IP("dsmrIP", "DSMR IP (leave blank if unused)", dsmrIP, 16);
WiFiManagerParameter CUSTOM_DSMR_PORT("dsmrPort", "DSMR Port", dsmrPort, 6);
WiFiManagerParameter CUSTOM_DSMR_API("dsmrAPI", "DSMR API", dsmrAPI, 70);
WiFiManagerParameter CUSTOM_MQTT_IP("mqttIP", "MQTT IP (leave blank if unused)", mqttIP, 16);
WiFiManagerParameter CUSTOM_MQTT_PORT("mqttPort", "MQTT Port", mqttPort, 6);
WiFiManagerParameter CUSTOM_MQTT_USER("mqttUser", "MQTT User", mqttUser, 32);
WiFiManagerParameter CUSTOM_MQTT_PASS("mqttPass", "MQTT Password", mqttPass, 32);

// ******************************************
// * Callback for saving WIFI config        *
// ******************************************

// * Callback notifying us of the need to save config
void save_wifi_config_callback()
{
    strcpy(scanInterval, CUSTOM_scanInterval.getValue());
    strcpy(dsmrIP, CUSTOM_DSMR_IP.getValue());
    strcpy(dsmrPort, CUSTOM_DSMR_PORT.getValue());
    strcpy(dsmrAPI, CUSTOM_DSMR_API.getValue());
    strcpy(mqttIP, CUSTOM_MQTT_IP.getValue());
    strcpy(mqttPort, CUSTOM_MQTT_PORT.getValue());
    strcpy(mqttUser, CUSTOM_MQTT_USER.getValue());
    strcpy(mqttPass, CUSTOM_MQTT_PASS.getValue());

    Debug.println(F("Saving WiFiManager config"));

    write_eeprom(0, 5, scanInterval);
    write_eeprom(3, 18, dsmrIP);
    write_eeprom(19, 24, dsmrPort);
    write_eeprom(25, 94, dsmrAPI);
    write_eeprom(95, 110, mqttIP);
    write_eeprom(111, 116, mqttPort);
    write_eeprom(117, 148, mqttUser);
    write_eeprom(149, 180, mqttPass);
    write_eeprom(181, 1, "1");
    commit_eeprom();
}

void setup()
{
    // * Configure Serial
    Serial.begin(BAUD_RATE);

    // DEBUG:
    Debug.begin(HOSTNAME);
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showColors(true); // Colors
    Debug.setSerialEnabled(true); // All messages too send to serial too, and can be see in serial monitor

    begin_eeprom(512);

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

    // * Custom values
    String settings_available = read_eeprom(98, 1);

    if (settings_available == "1")
    { 
        read_eeprom(0, 5).toCharArray(scanInterval, 3);
        read_eeprom(6, 21).toCharArray(dsmrIP, 16);
        read_eeprom(22, 27).toCharArray(dsmrPort, 6);
        read_eeprom(28, 97).toCharArray(dsmrAPI, 70);
        read_eeprom(98, 113).toCharArray(mqttIP, 16);
        read_eeprom(114, 119).toCharArray(mqttPort, 6);
        read_eeprom(120, 151).toCharArray(mqttUser, 32);
        read_eeprom(152, 183).toCharArray(mqttPass, 32);
    }

    // * WiFiManager local initialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // * Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);

    // * Set timeout
    wifiManager.setConfigPortalTimeout(WIFI_TIMEOUT);

    // * Set save config callback
    wifiManager.setSaveConfigCallback(save_wifi_config_callback);

    wifiManager.addParameter(&CUSTOM_scanInterval);
    wifiManager.addParameter(&CUSTOM_DSMR_IP);
    wifiManager.addParameter(&CUSTOM_DSMR_PORT);
    wifiManager.addParameter(&CUSTOM_DSMR_API);
    wifiManager.addParameter(&CUSTOM_MQTT_IP);
    wifiManager.addParameter(&CUSTOM_MQTT_PORT);
    wifiManager.addParameter(&CUSTOM_MQTT_USER);
    wifiManager.addParameter(&CUSTOM_MQTT_PASS);

    // * Fetches SSID and pass and tries to connect
    // * Reset when no connection after 10 seconds
    if (!wifiManager.autoConnect())
    {
        Debug.println(F("Failed to connect to WIFI and hit timeout"));

        // * Reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(WIFI_TIMEOUT);
    }
    
    // * If you get here you have connected to the WiFi
    Debug.println(F("Connected to WIFI..."));

    // * Keep LED on
    LED_Blink(0);

    // * Configure OTA
    setup_ota();

    // * Startup MDNS Service
    setup_mdns();

    // * Setup P1 DSMR Reader
    reader.enable(true);
    last = millis();

    if (strlen(dsmrIP) == 0)
        DSMR_Reader_enabled = false;
    else
        DSMR_Reader_enabled = true;

    if (strlen(mqttIP) == 0)
        MQTT_enabled = false;
    else
        MQTT_enabled = true;

    scanInterval_INT = (unsigned)atoi(scanInterval);
    if (scanInterval_INT < MINIMAL_SCAN_INTERVAL)
    {
        scanInterval_INT = MINIMAL_SCAN_INTERVAL;
    }

    Debug.println("scanInterval:");
    Debug.println(scanInterval_INT);
    if (DSMR_Reader_enabled == true)
    {
        Debug.println("dsmrIP:");
        Debug.println(dsmrIP);
        Debug.println("dsmrPort:");
        Debug.println(dsmrPort);
        Debug.println("dsmrAPI:");
        Debug.println(dsmrAPI);
        Debug.println("mqttIP:");
    }
    if (MQTT_enabled == true)
    {
        Debug.println(mqttIP);
        Debug.println("mqttPort:");
        Debug.println(mqttPort);
        Debug.println("mqttUser:");
        Debug.println(mqttUser);
        Debug.println("mqttPass:");
        Debug.println(mqttPass);
    }

    dsmrHost = "http://";
    dsmrHost += dsmrIP;
    dsmrHost += ":";
    dsmrHost += dsmrPort;
    dsmrHost += "/api/v2/datalogger/dsmrreading";

    // * Setup MQTT
    if (MQTT_enabled)
    {
        MQTT_connect();
    }
    
}

void loop()
{
    if (MQTT_enabled)
    {
        MQTT_handle();
    }

    // Allow the reader to check the serial buffer regularly
    reader.loop();

    // Every x, fire off a one-off reading
    unsigned long now = millis();
    if (now - last > (scanInterval_INT * 1000))
    {
        reader.enable(true);
        last = now;
        Debug.println("/***GO!***/");
    }

    if (reader.available())
    {
        MyData data;
        String err;
        if (reader.parse(&data, &err))
        {
            Debug.println("/**BEGIN**/");
            if (DSMR_Reader_enabled)
            {
                Send_to_DSMR_Reader(data);
            }
            if (MQTT_enabled)
            {
                Send_to_MQTT(data);
            }
            if (DSMR_Reader_enabled != true && MQTT_enabled != true)
            {
                Debug.println("!!! No backend enabled !!");
            }
            Debug.println("/***END***/");
            Debug.println("");
        }
        else
        {
            // Parser error, print error
            Debug.printf("Parser error: %s\n\r", err.c_str());
        }
    }
    // OTA handler
    ArduinoOTA.handle();
    // Remote debug over WiFi
    Debug.handle();
}
