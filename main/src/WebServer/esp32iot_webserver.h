#ifndef ESP32IOT_WEBSERVER_H
#define ESP32IOT_WEBSERVER_H

#include "main.h"

#include <string.h>

#include "openssl/ssl.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"


#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "esp32iot_wifi.h"
#include "esp32iot_storage.h"

#include "./web_root/index_html.h"
#include "./web_root/wifi-setup/wifi-setup_html.h"
#include "./web_root/mqtt-setup/mqtt-setup_html.h"

#define OPENSSL_ESP32IOT_SERVER_ACK "HTTP/1.1 200 OK\r\n" \
                                "Content-Type: text/html\r\n" \
                                "Content-Length: 98\r\n\r\n" \
                                "<html>\r\n" \
                                "<head>\r\n" \
                                "<title>OpenSSL example</title></head><body>\r\n" \
                                "OpenSSL server example!\r\n" \
                                "</body>\r\n" \
                                "</html>\r\n" \
                                "\r\n"

#define OPENSSL_ESP32IOT_TASK_NAME        "ESP32IOT Manager"
#define OPENSSL_ESP32IOT_TASK_STACK_WORDS 10240
#define OPENSSL_ESP32IOT_TASK_PRIORITY    8

#define OPENSSL_ESP32IOT_RECV_BUF_LEN       1024

#define OPENSSL_ESP32IOT_LOCAL_TCP_PORT     443

const static int CONNECTED_BIT = BIT0;

static char webserver_tag []="esp32iot-webserver";



/* Initialize Web Server Manager as sta */
esp_err_t webserver_initialize(void);

void openssl_esp32iot_task(void *p);

void openssl_server_init(void);

#endif