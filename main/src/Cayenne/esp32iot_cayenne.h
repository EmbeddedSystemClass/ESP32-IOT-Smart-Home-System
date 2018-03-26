#ifndef ESP32IOT_CAYENNE_H
#define ESP32IOT_CAYENNE_H

#include "main.h"

#include "esp_log.h"

#include "CayenneMQTTClient.h"

#include "esp32iot_bluetooth.h"
#include "Timer.h"

#include "freertos/event_groups.h"

static char cayenne_tag []="esp32iot-cayenne";

Network network;
CayenneMQTTClient mqttClient;

static EventGroupHandle_t cayenne_event_group;

void CayenneInit(const char* username, const char* password, const char* clientID);

// Connect to the Cayenne server.
int connectClient(void);

void cayenne_task(void);

#endif