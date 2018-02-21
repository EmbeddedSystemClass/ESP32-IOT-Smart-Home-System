/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "main.h"


// Connect to the Cayenne server.
int connectClient(void)
{
    // Connect to the server.
    int error = 0;
    printf("Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
    if ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
        return error;
    }

    if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {
        NetworkDisconnect(&network);
        return error;
    }
    printf("Connected\n");

    // Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "esp32");

    return CAYENNE_SUCCESS;
}

void loop(void)
{
    while (1) {
        // Yield to allow MQTT message processing.
        CayenneMQTTYield(&mqttClient, 1000);

        // Publish some example data every second. This should be changed to send your actual data to Cayenne.
        CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5);
        CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000);
    }
}

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

    err = webserver_initialize();
    if (err != ESP_OK){
        ESP_LOGW(main_tag, "Error ( %d )", err);
        ESP_ERROR_CHECK( err );
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
    NetworkInit(&network);

    // Initialize the Cayenne client.
    CayenneMQTTClientInit(&mqttClient, &network, username, password, clientID, NULL);
    

    printf("\nESP32-IOT End!.\n");
    fflush(stdout);
    //esp_restart();
}
