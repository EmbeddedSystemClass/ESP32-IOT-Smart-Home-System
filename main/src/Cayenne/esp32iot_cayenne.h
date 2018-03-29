#ifndef ESP32IOT_CAYENNE_H
#define ESP32IOT_CAYENNE_H


#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "CayenneMQTTClient.h"

#include "MQTTesp32.h"

#include "esp_log.h"
#include "esp_system.h"

#include "CayenneMQTTClient.h"

#include "Timer.h"

#include "freertos/event_groups.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#include "esp32iot_wifi.h"
#include "esp32iot_bluetooth.h"

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))


#define BA5C_RESTART_BUTTON_CHANNEL 8

#define BUTTON_ON "1"
#define BUTTON_OFF "0"

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
static char* username = "";
static char* password = "";
static char* clientID = "";

Network network;
CayenneMQTTClient mqttClient;
//static bool finished = false;

//int i = 0;
//Timer timer;
    



//static EventGroupHandle_t cayenne_event_group;

void CayenneInit(const char* username_arg, const char* password_arg, const char* clientID_arg);

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

#endif