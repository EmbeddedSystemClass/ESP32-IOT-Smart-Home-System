#ifndef ESP32IOT_CAYENNE_H
#define ESP32IOT_CAYENNE_H

#include "main.h"

#include "esp_log.h"

#include "CayenneMQTTClient.h"

#include "esp32iot_bluetooth.h"
#include "Timer.h"

#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

static char cayenne_tag []="esp32iot-cayenne";

Network network;
CayenneMQTTClient mqttClient;
static bool finished = false;

//int i = 0;
Timer timer;
    

static EventGroupHandle_t cayenne_event_group;

void CayenneInit(const char* username, const char* password, const char* clientID);

// Connect to the Cayenne server.
int connectClient(void);

/**
* Print the message info.
* @param[in] message The message received from the Cayenne server.
*/
void outputMessage(CayenneMessageData* message);

/**
* Handle messages received from the Cayenne server.
* @param[in] message The message received from the Cayenne server.
*/
void messageArrived(CayenneMessageData* message);

void cayenne_task(void);

void intHandler(int signum);

#endif