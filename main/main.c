/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "main.h"

void app_main()
{
    esp_err_t err;

    printf("\n\nESP32-IOT: Smart Home System ==> Start!\n\n");

/*    err = ;
    if (err != ESP_OK){
        ESP_LOGW(mqtt_main_tag, "%d", err);
        ESP_ERROR_CHECK( err );
    }

    ESP_LOGI(mqtt_main_tag, "");*/

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "", (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    printf("silicon revision %d, ", chip_info.revision);
    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");


    err = storage_init();
    if (err != ESP_OK){
        ESP_LOGW(main_tag, "Error ( %d )", err);
        ESP_ERROR_CHECK( err );
    }

    err = get_restart_counter();
    if (err != ESP_OK){
        ESP_LOGW(main_tag, "Error ( %d )", err);
        ESP_ERROR_CHECK( err );
    }

    err = save_restart_counter();
    if (err != ESP_OK){
        ESP_LOGW(main_tag, "Error ( %d )", err);
        ESP_ERROR_CHECK( err );
    }
    
    err = wifi_start();
    if (err != ESP_OK){
        ESP_LOGW(main_tag, "Error ( %d )", err);
        ESP_ERROR_CHECK( err );
    }else{
       http_server_init(); 
    }

    

/*    TimerInit(&timer);
    printf("Timer: TimerIsExpired: %i \n\n", TimerIsExpired(&timer));
    TimerCountdown(&timer, 1);

    printf("Timer: TimerIsExpired: %i \n\n", TimerIsExpired(&timer));

    int prev_timeout =  TimerLeftMS(&timer);

    while(1){
        if(prev_timeout != TimerLeftMS(&timer)){
            printf("Timer: TimerLeftMS: %d \n", TimerLeftMS(&timer));
            prev_timeout =  TimerLeftMS(&timer);
        }else if(prev_timeout == 0){
            break;
        }
    }

    printf("TimerMS: TimerIsExpired: %i \n\n", TimerIsExpired(&timer));
    TimerCountdownMS(&timer, 50);

    printf("TimerMS: TimerIsExpired: %i \n\n", TimerIsExpired(&timer));

    prev_timeout =  TimerLeftMS(&timer);

    while(1){
        if(prev_timeout != TimerLeftMS(&timer)){
            printf("TimerMS: TimerLeftMS: %d \n", TimerLeftMS(&timer));
            prev_timeout =  TimerLeftMS(&timer);
        }else if(prev_timeout == 0){
            break;
        }
    }

    printf("TimerMS: TimerIsExpired: %i \n\n", TimerIsExpired(&timer));*/

    // Initialize the network.

    printf("\nESP32-IOT End!.\n");
    //fflush(stdout);
    //esp_restart();
}
