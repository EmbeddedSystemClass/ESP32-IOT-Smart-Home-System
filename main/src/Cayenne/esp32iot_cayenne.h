#ifndef ESP32IOT_CAYENNE_H
#define ESP32IOT_CAYENNE_H

#include "main.h"

#include "esp_log.h"

#include "CayenneMQTTClient.h"

static char cayenne_tag []="esp32iot-cayenne";

Network network;
CayenneMQTTClient mqttClient;

void CayenneInit(const char* username, const char* password, const char* clientID);

// Connect to the Cayenne server.
int connectClient(void);

void loop(void);

#endif