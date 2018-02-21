#ifndef ESP32IOT_STORAGE_H
#define ESP32IOT_STORAGE_H

#include "main.h"

#include <esp_log.h>
#include <esp_event.h>
#include <esp_event_loop.h>

#include <nvs_flash.h>
#include <nvs.h>

#include <stdio.h>
#include <string.h>

#define GENERAL_STORAGE_NAMESPACE "general_storage"
#define WIFI_SETTINGS_STORAGE_NAMESPACE "wifi_storage"
#define MQTT_SETTINGS_STORAGE_NAMESPACE "mqtt_storage"

static char storage_tag []="esp32iot-storage";

esp_err_t storage_init(void);
esp_err_t erase_storage(void);

esp_err_t save_wifi(const char* ssid, const char* password);
esp_err_t save_last_connected_wifi(const char* ssid, const char* password);

esp_err_t get_wifi(const char* ssid, char* password[]);
esp_err_t get_last_connected_wifi(const char* ssid[], char* password[]);

esp_err_t save_mqtt(void);
esp_err_t get_mqtt(void);

esp_err_t save_restart_counter(void);
esp_err_t get_restart_counter(void);

#endif