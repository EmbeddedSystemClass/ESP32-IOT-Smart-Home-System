#ifndef ESP32IOT_CAYENNE_H
#define ESP32IOT_CAYENNE_H

#include "main.h"

static char cayenne_tag []="esp32iot-cayenne";

Network network;
CayenneMQTTClient mqttClient;

void CayenneInit(void);

// Connect to the Cayenne server.
int connectClient(void);

void loop(void);

#endif