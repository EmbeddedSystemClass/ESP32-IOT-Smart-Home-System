#ifndef ESP32IOT_WIFI_H
#define ESP32IOT_WIFI_H

#include "main.h"

#include <esp_wifi.h>

#include <esp_log.h>
#include <esp_event.h>
#include <esp_event_loop.h>

#include "esp32iot_storage.h"

#define DEFAULT_AP_SSID "esp32-iot-smart-home-system"
#define DEFAULT_AP_PASSWORD ""
#define DEFAULT_AP_AUTHMODE WIFI_AUTH_OPEN

//#define DEFAULT_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN


#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN
#define DEFAULT_RSSI -127
#define DEFAULT_SCAN_AUTHMODE WIFI_AUTH_OPEN

#define WIFI_MANAGER_WAITING_FOR_ACTION         1
#define WIFI_MANAGER_MODE_AP                    2
#define WIFI_MANAGER_MODE_STA                   3
#define WIFI_MANAGER_CONNECTION_ATTEMPT_STA     4
#define WIFI_MANAGER_CONNECTED_STA              5
#define WIFI_MANAGER_SCANNING_STA               6
#define WIFI_MANAGER_CONNECTING_AP              7
#define WIFI_MANAGER_CONNECTED_AP               8

static char wifi_tag []="esp32iot-wifi";

static uint8_t connection_failure_counter = 0; //should be local in file *.c

typedef struct {
	char *ssid;      /**< SSID of target AP*/
    char *password;  /**< password of target AP*/	
} wifi_evidence_t;

static wifi_evidence_t actual_wifi;

static uint8_t wifi_manager_state = 0;

/* Convert a WiFi event type to a string. */
char *wifi_err_to_string(int ev);

void copy_string(uint8_t d[], uint8_t s[]);

/* Initialize Wi-Fi as ap */
esp_err_t wifi_ap_create(void);

/* Initialize Wi-Fi as ap with scan */
esp_err_t wifi_ap_scan_create(void);

/* Initialize Wi-Fi as sta */
esp_err_t wifi_initialize(void);

/* Start Wi-Fi scan */
esp_err_t wifi_scan_start(void);

/* Connect to sta Wi-Fi */
esp_err_t wifi_sta_start(const char *ssid, const char *password);

/*  WiFi event handler. */
esp_err_t wifi_event_handler(void *ctx, system_event_t *event);

#endif