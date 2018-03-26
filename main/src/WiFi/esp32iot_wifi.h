#ifndef ESP32IOT_WIFI_H
#define ESP32IOT_WIFI_H

#include "main.h"

#include <esp_wifi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include <esp_log.h>
#include <esp_event.h>
#include <esp_event_loop.h>

#include "esp32iot_storage.h"
#include "esp32iot_cayenne.h"
#include "esp32iot_bluetooth.h"

#define DEFAULT_AP_SSID "S/M/H_Beacon"

#define DEFAULT_AP_PASSWORD "smh_322aq"
#define DEFAULT_AP_AUTHMODE WIFI_AUTH_WPA2_PSK
// #define DEFAULT_AP_PASSWORD ""
// #define DEFAULT_AP_AUTHMODE WIFI_AUTH_OPEN

//#define DEFAULT_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN

#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN
#define DEFAULT_RSSI -127
#define DEFAULT_SCAN_AUTHMODE WIFI_AUTH_OPEN

#define MAX_WIFI_CONNECTION_ATTEMPTS	3

static char wifi_tag []="esp32iot-wifi";

static EventGroupHandle_t wifi_event_group;

static uint8_t connection_failure_counter = 0; //should be local in file *.c

typedef struct wifi_evidence_t{
	char ssid[33];      /**< SSID of target AP*/
    char password[64];  /**< password of target AP*/	
};

struct wifi_evidence_t actual_wifi;

/* Convert a WiFi event type to a string. */
char *wifi_err_to_string(int ev);

/* Initialize Wi-Fi as sta */
esp_err_t wifi_initialize(void);

/* Start AP or STA Wi-Fi */
esp_err_t wifi_start(void);

/*  WiFi event handler. */
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event);


/* Initialize Wi-Fi as ap with scan */
esp_err_t wifi_ap_scan_create(void);

/* Start Wi-Fi scan */
esp_err_t wifi_scan_start(void);

esp_err_t wifi_ap_configure(void );

esp_err_t wifi_sta_configure(const char ssid[], const char password[]);

esp_err_t wifi_apsta_configure(const char ssid[], const char password[]);

#endif