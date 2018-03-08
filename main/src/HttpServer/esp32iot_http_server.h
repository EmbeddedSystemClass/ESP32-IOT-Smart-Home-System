#ifndef ESP32IOT_HTTP_SERVER_H
#define ESP32IOT_HTTP_SERVER_H

#include "main.h"

#include <string.h>

#include "openssl/ssl.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"


#include "nvs_flash.h"

#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/err.h"

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#include "./web_root/index_html.h"
#include "./web_root/wifi-setup/wifi-setup_html.h"
#include "./web_root/mqtt-setup/mqtt-setup_html.h"
#include "./web_root/notifications/notifications_html.h"

#include "esp32iot_cayenne.h"

#define HTTP_SERVER_ESP32IOT_TASK_NAME        "ESP32IOT Manager"

#define HTTP_SERVER_ESP32IOT_TASK_STACK_WORDS 10240
#define HTTP_SERVER_ESP32IOT_TASK_PRIORITY    8

#define HTTP_SERVER_ESP32IOT_RECV_BUF_LEN       1024

#define HTTP_SERVER_ESP32IOT_LOCAL_TCP_PORT     443

const static int CONNECTED_BIT = BIT0;

static char http_server_tag []="esp32iot-http_server";

char* json_unformatted;

extern struct wifi_evidence_t actual_wifi;

extern uint8_t wifi_manager_state;

struct netconn *conn, *newconn;

struct http_request_str http_request_to_str(const char *s);

static void http_server_netconn_serve(struct netconn *conn);

static void http_server(void *pvParameters);

void http_server_init(void);

static err_t parse_http_request(const char* request, const char key[], char value[]);

#endif