// **********************************
// * Backend MQTT                   *
// **********************************

#ifndef BACKEND_MQTT_H
#define BACKEND_MQTT_H

#include "P1Reader.h"

void Send_to_MQTT(MyData data, MyData LastData);
void MQTT_handle(void);
void MQTT_connect(void);

#endif /* BACKEND_MQTT_H */