#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "CayenneMQTTClient.h"
#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"
#include "esp32iot_webserver.h"

static char main_tag []="esp32iot"; //general (main) tag

static char* username = "1b98ead0-06af-11e8-ba62-45381d39c6f0";
static char* password = "6d70c659b7caaf0fa1bf03c3c79a846b0c5ae33b";
static char* clientID = "ad42bbb0-16df-11e8-b59c-db84183bf26b";

Network network;
CayenneMQTTClient mqttClient;

// Connect to the Cayenne server.
int connectClient(void);

void loop(void);

#endif