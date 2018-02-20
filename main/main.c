/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "main.h"

#include "CayenneMQTTClient.h"

Network network;
CayenneMQTTClient mqttClient;

void app_main()
{
    esp_err_t err;

    printf("\n\nESP32-IOT Start!\n\n");

/*    err = ;
    if (err != ESP_OK){
        ESP_LOGW(mqtt_tag, "%d", err);
        ESP_ERROR_CHECK( err );
    }

    ESP_LOGI(mqtt_tag, "");*/

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "", (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    printf("silicon revision %d, ", chip_info.revision);
    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("\nESP32-IOT End!.\n");
    fflush(stdout);
    //esp_restart();
}
