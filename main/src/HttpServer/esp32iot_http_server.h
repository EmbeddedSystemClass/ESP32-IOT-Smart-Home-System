#ifndef ESP32IOT_HTTP_SERVER_H
#define ESP32IOT_HTTP_SERVER_H

#include "main.h"

#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "mongoose.h"

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#include "./web_root/index_html.h"
#include "./web_root/wifi-setup/wifi-setup_html.h"
#include "./web_root/mqtt-setup/mqtt-setup_html.h"
#include "./web_root/notifications/notifications_html.h"

#include "esp32iot_cayenne.h"

#define HTTP_SERVER_ESP32IOT_TASK_NAME	"ESP32IOT Manager"

static char http_server_tag []="esp32iot-http_server";

#define MG_LISTEN_ADDR "80"

const static int CONNECTED_BIT = BIT0;

extern struct wifi_evidence_t actual_wifi;

extern uint8_t wifi_manager_state;

static struct mg_mgr mgr;
static struct mg_connection *nc;

static char* username = ""; //cayenne_mqtt
static char* password = "";
static char* clientID = "";

char *mg_ev_to_string(int ev);

static void handle_sum_call(struct mg_connection *nc, struct http_message *hm);

static void mg_ev_handler(struct mg_connection *nc, int ev, void *ev_data);

void http_server_init(void *data);

#endif