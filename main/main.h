#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "freertos/portmacro.h"

#include "CayenneMQTTClient.h"
#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"
#include "esp32iot_http_server.h"
#include "esp32iot_cayenne.h"
#include "mongoose.h"

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

static char main_tag []="esp32iot"; //general (main) tag

static char* username = "1b98ead0-06af-11e8-ba62-45381d39c6f0";
static char* password = "6d70c659b7caaf0fa1bf03c3c79a846b0c5ae33b";
static char* clientID = "b06385e0-16ee-11e8-9f38-9fae3d42ebf0";


#endif