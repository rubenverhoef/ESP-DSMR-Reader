#include <Arduino.h>
#include <RemoteDebug.h>
#include "EEPROM_Handler.h"
#include "WiFi.h"
#include "LED.h"
#include "P1Reader.h"
#include "Backend_DSMR-Reader.h"

// * Include settings
#include "settings.h"

unsigned long last;
P1Reader reader(&Serial, P1_RTS_PIN);
RemoteDebug Debug;

char scanInterval[6] = ""; // Scan interval of readings
char dsmrIP[16] = ""; // IP to DSMR-Reader backend
char dsmrPort[6] = ""; // Port to DSMR-Reader backend
char dsmrAPI[70] = ""; // API key for DSMR-Reader backend
String dsmrHost;

WiFiManagerParameter CUSTOM_scanInterval("scanInterval", "Scan Interval", scanInterval, 6);
WiFiManagerParameter CUSTOM_DSMR_IP("ip", "DSMR IP", dsmrIP, 16);
WiFiManagerParameter CUSTOM_DSMR_PORT("port", "DSMR port", dsmrPort, 6);
WiFiManagerParameter CUSTOM_DSMR_API("api", "DSMR API", dsmrAPI, 70);

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

    Serial.println(F("Should save config"));
    Serial.println(F("Saving WiFiManager config"));

    write_eeprom(0, 5, scanInterval);
    write_eeprom(6, 21, dsmrIP);
    write_eeprom(22, 27, dsmrPort);
    write_eeprom(28, 97, dsmrAPI);
    write_eeprom(98, 1, "1");
    commit_eeprom();
}

void setup()
{
    // * Configure Serial
    Serial.begin(BAUD_RATE);
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
        read_eeprom(0, 5).toCharArray(scanInterval, 6);
        read_eeprom(6, 21).toCharArray(dsmrIP, 16);
        read_eeprom(22, 27).toCharArray(dsmrPort, 6);
        read_eeprom(28, 97).toCharArray(dsmrAPI, 70);
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

    Serial.println("scanInterval:");
    Serial.println(scanInterval);
    Serial.println("dsmrIP:");
    Serial.println(dsmrIP);
    Serial.println("dsmrPort:");
    Serial.println(dsmrPort);
    Serial.println("dsmrAPI:");
    Serial.println(dsmrAPI);

    dsmrHost = "http://";
    dsmrHost += dsmrIP;
    dsmrHost += ":";
    dsmrHost += dsmrPort;
    dsmrHost += "/api/v2/datalogger/dsmrreading";

    // DEBUG:
    Debug.begin(HOSTNAME);
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showColors(true); // Colors
    Debug.setSerialEnabled(true); // All messages too send to serial too, and can be see in serial monitor
}

void loop()
{
    // Allow the reader to check the serial buffer regularly
    reader.loop();

    // Every x, fire off a one-off reading
    unsigned long now = millis();
    if (now - last > (unsigned)atoi(scanInterval))
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
            Debug.printf("/**BEGIN**/\n\r\n\r");
            Send_to_DSMR_Reader(data);
            Debug.printf("/***END***/\n\r\n\r");
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
