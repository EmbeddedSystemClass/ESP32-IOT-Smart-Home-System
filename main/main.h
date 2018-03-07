#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "freertos/portmacro.h"

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

static char main_tag []="esp32iot"; //general (main) tag

#endif