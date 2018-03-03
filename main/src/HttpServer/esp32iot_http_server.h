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

#include "mongoose.h"

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#include "./web_root/index_html.h"
#include "./web_root/wifi-setup/wifi-setup_html.h"
#include "./web_root/mqtt-setup/mqtt-setup_html.h"
#include "./web_root/notifications/notifications_html.h"

static const char *s_http_port = "8000";
static const char *s_access_key_id = NULL;
static const char *s_secret_access_key = NULL;
static struct mg_serve_http_opts s_http_server_opts;

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

#define MG_LISTEN_ADDR "80"


/* Server handler */
static void mg_ev_handler(struct mg_connection *nc, int ev, void *ev_data);

static esp_err_t mg_init(void );

void mongoose_task(void );

esp_err_t http_server_init(void);


#endif