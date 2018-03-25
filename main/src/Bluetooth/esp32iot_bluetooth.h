#ifndef ESP32IOT_BLUETOOTH_H
#define ESP32IOT_BLUETOOTH_H

#include "main.h"

#include "esp_log.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "controller.h"

#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include <math.h>

#define GATTC_TAG "esp32iot-gattc"

#define PROFILE_NUM      1

#define PROFILE_BA5C_APP_ID 0

#define INVALID_HANDLE 0

static char bluetooth_tag []="esp32iot-bluetooth";

static uint16_t read_interval = 10;

/*static uint16_t BA5C_HTU21D_recived_notification_counter = 0;
static uint16_t BA5C_HTU21D_temperature = 0;
static uint16_t BA5C_HTU21D_humidity = 0;
static uint16_t BA5C_HTU21D_status = 0;

static uint16_t BA5C_MS5637_recived_notification_counter = 0;
static uint16_t BA5C_MS5637_temperature = 0;
static uint16_t BA5C_MS5637_pressure = 0;
static uint16_t BA5C_MS5637_status = 0;

//static uint16_t BA5C_Battery_recived_notification_counter = 0;
static uint16_t BA5C_Battery_level = 0;
static uint16_t BA5C_Battery_status = 0;*/

/* uint16_t BA5C_HTU21D_service_start_handle;
    uint16_t BA5C_HTU21D_service_end_handle;
    uint16_t BA5C_HTU21D_data_char_handle;
    uint16_t BA5C_HTU21D_status_char_handle;
    bool BA5C_HTU21D_get_service;

    uint16_t BA5C_MS5637_service_start_handle;
    uint16_t BA5C_MS5637_service_end_handle;
    uint16_t BA5C_MS5637_data_char_handle;
    uint16_t BA5C_MS5637_calibration_char_handle;
    uint16_t BA5C_MS5637_status_char_handle;
    bool BA5C_MS5637_get_service;

    uint16_t BA5C_Battery_service_start_handle;
    uint16_t BA5C_Battery_service_end_handle;
    uint16_t c;
    bool BA5C_Battery_get_service;*/

typedef struct {
    esp_bt_uuid_t notify_descr_char_uuid;

    esp_bt_uuid_t HTU21D_service_uuid;
    esp_bt_uuid_t HTU21D_data_char_uuid;
    esp_bt_uuid_t HTU21D_status_char_uuid;

    esp_bt_uuid_t MS5637_service_uuid;
    esp_bt_uuid_t MS5637_data_char_uuid;
    esp_bt_uuid_t MS5637_calibration_char_uuid;
    esp_bt_uuid_t MS5637_status_char_uuid;

    esp_bt_uuid_t Battery_service_uuid;
    esp_bt_uuid_t Battery_data_char_uuid;
} BA5C_uuid_t;

typedef struct {
    bool conn_device;

    uint16_t HTU21D_service_start_handle;
    uint16_t HTU21D_service_end_handle;
    uint16_t HTU21D_data_char_handle;
    uint16_t HTU21D_status_char_handle;
    bool HTU21D_get_service;

    uint16_t MS5637_service_start_handle;
    uint16_t MS5637_service_end_handle;
    uint16_t MS5637_data_char_handle;
    uint16_t MS5637_calibration_char_handle;
    uint16_t MS5637_status_char_handle;
    bool MS5637_get_service;

    uint16_t Battery_service_start_handle;
    uint16_t Battery_service_end_handle;
    uint16_t Battery_data_char_handle;
    bool Battery_get_service;
} BA5C_handle_data_t;

typedef struct {
    uint8_t HTU21D_recived_notification_counter;
    int16_t HTU21D_temperature;
    int16_t HTU21D_humidity;
    uint8_t HTU21D_status;

    uint8_t MS5637_recived_notification_counter;
    uint32_t MS5637_D1, MS5637_D2;
    int32_t MS5637_temperature;
    int32_t MS5637_pressure;
    uint8_t MS5637_status;

    uint8_t Battery_level;
    uint8_t Battery_status;
} BA5C_data_t;

BA5C_data_t BA5C_data;


/* eclare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static void gattc_profile_BA5C_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);


static BA5C_uuid_t BA5C_uuid = {
    .notify_descr_char_uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
        //.uuid = {.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION,},
    },


    .HTU21D_service_uuid = {
        //0xF0, 0x00, 0xAA, 0x20, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x20, 0xAA, 0x00, 0xF0},},
    },
    .HTU21D_data_char_uuid = {
        //0xF0, 0x00, 0xAA, 0x21, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x21, 0xAA, 0x00, 0xF0},},
    },
    .HTU21D_status_char_uuid = {
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x2F, 0xAA, 0x00, 0xF0},},
    },


    .MS5637_service_uuid = {
        //0xF0, 0x00, 0xAA, 0x20, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x40, 0xAA, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0x04, 0xAA, 0x00, 0x0F},},   

    },
    .MS5637_data_char_uuid = {
        //0xF0, 0x00, 0xAA, 0x21, 0x04, 0x51, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x41, 0xAA, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0x14, 0xAA, 0x00, 0x0F},},
    },
    .MS5637_calibration_char_uuid = {
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x43, 0xAA, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0x34, 0xAA, 0x00, 0x0F},},
    },
    .MS5637_status_char_uuid = {
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x4F, 0xAA, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0xF4, 0xAA, 0x00, 0x0F},},
    },


    .Battery_service_uuid = {
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x0F, 0x18, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0xF0, 0x81, 0x00, 0x0F},},
    },
    .Battery_data_char_uuid = {
        .len = ESP_UUID_LEN_128,
        .uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x40, 0x51, 0x04, 0x19, 0x2A, 0x00, 0xF0},},
        //.uuid = {.uuid128 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x04, 0x15, 0x40, 0x91, 0xA2, 0x00, 0x0F},},
    },
};



static bool Isconnecting    = false;
static bool stop_scan_done  = false;

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};

static esp_gattc_char_elem_t  *char_elem_result_BA5C   = NULL;
static esp_gattc_descr_elem_t *descr_elem_result_BA5C  = NULL;

static const char remote_device_name[1][20] = {"MEAS Tag BA5C"};


/*ToDo: split gattc_profile_inst data type to be specyfied for particular device*/
struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;

    uint16_t char_handle;

    esp_bd_addr_t remote_bda;
    
    /*BA5C*/
/*    bool conn_device_BA5C;

    uint16_t BA5C_HTU21D_service_start_handle;
    uint16_t BA5C_HTU21D_service_end_handle;
    uint16_t BA5C_HTU21D_data_char_handle;
    uint16_t BA5C_HTU21D_status_char_handle;
    bool BA5C_HTU21D_get_service;

    uint16_t BA5C_MS5637_service_start_handle;
    uint16_t BA5C_MS5637_service_end_handle;
    uint16_t BA5C_MS5637_data_char_handle;
    uint16_t BA5C_MS5637_calibration_char_handle;
    uint16_t BA5C_MS5637_status_char_handle;
    bool BA5C_MS5637_get_service;

    uint16_t BA5C_Battery_service_start_handle;
    uint16_t BA5C_Battery_service_end_handle;
    uint16_t BA5C_Battery_data_char_handle;
    bool BA5C_Battery_get_service;*/

    BA5C_handle_data_t BA5C_handle_data;

};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_BA5C_APP_ID] = {
        .gattc_cb = gattc_profile_BA5C_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

esp_err_t bluetooth_initialize(void);

#endif