#include <Arduino.h>

#include "settings.h"
#include "EEPROM_Handler.h"

// **********************************
// * EEPROM helpers                 *
// **********************************

String read_eeprom(int offset, int len)
{
    Debug.print(F("read_eeprom()"));

    String res = "";
    for (int i = 0; i < len; ++i)
    {
        res += char(EEPROM.read(i + offset));
    }
    return res;
}

void write_eeprom(int offset, int len, String value)
{
    Debug.println(F("write_eeprom()"));
    for (int i = 0; i < len; ++i)
    {
        if ((unsigned)i < value.length())
        {
            EEPROM.write(i + offset, value[i]);
        }
        else
        {
            EEPROM.write(i + offset, 0);
        }
    }
}

void commit_eeprom(void)
{
    EEPROM.commit();
}

void begin_eeprom(unsigned int size)
{
    EEPROM.begin(size);
}
