// **********************************
// * EEPROM                         *
// **********************************

#ifndef EEPROM_H
#define EEPROM_H

#include <EEPROM.h>

/* Function prototypes */
String read_eeprom(int offset, int len);
void write_eeprom(int offset, int len, String value);
void save_wifi_config_callback();
void commit_eeprom(void);
void begin_eeprom(unsigned int size);

#endif /* EEPROM_H */
