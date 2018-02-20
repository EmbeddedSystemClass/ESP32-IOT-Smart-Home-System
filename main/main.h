#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"


static char mqtt_tag []="esp32iot-mqtt";
static char general_tag []="esp32iot";


char* username = "1b98ead0-06af-11e8-ba62-45381d39c6f0";
char* password = "6d70c659b7caaf0fa1bf03c3c79a846b0c5ae33b";
char* clientID = "51984ae0-158c-11e8-b59c-db84183bf26b";


#endif