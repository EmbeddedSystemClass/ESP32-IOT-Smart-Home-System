#ifndef ESP32IOT_WEBSERVER_H
#define ESP32IOT_WEBSERVER_H

#include "main.h"

#include <esp_log.h>
#include <esp_event.h>
#include <esp_event_loop.h>

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#include "./web_root/index_html.h"
#include "./web_root/wifi-setup/wifi-setup_html.h"
#include "./web_root/mqtt-setup/mqtt-setup_html.h"

static char webserver_tag []="esp32iot-webserver";

/* Initialize Web Server Manager as sta */
esp_err_t webserver_initialize(void);


#endif