#include "esp32iot_cayenne.h"

void CayenneInit(const char* username, const char* password, const char* clientID){
	// Initialize the network.
    NetworkInit(&network);

    // Initialize the Cayenne client.
    CayenneMQTTClientInit(&mqttClient, &network, username, password, clientID, NULL);
}

// Connect to the Cayenne server.
int connectClient(void)
{
    // Connect to the server.
    int error = 0;
    ESP_LOGI(cayenne_tag, "Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
    if ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
        ESP_LOGI(cayenne_tag, "NetworkConnect error: %d\n", error);
        return error;
    }
    if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {

        NetworkDisconnect(&network);
        ESP_LOGI(cayenne_tag, "CayenneMQTTConnect error: %d\n", error);
        return error;
    }
    ESP_LOGI(cayenne_tag, "Connected\n");

    // Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "esp32");

    return CAYENNE_SUCCESS;
}

Timer timer;

void loop(void)
{

    //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");
    print_timer_counter(&timer);
    // Yield to allow MQTT message processing.
/*    CayenneMQTTYield(&mqttClient, 1000);
    //TimerLeftMS(&Timer);
    //delay(2000);

    // Publish some example data every second. This should be changed to send your actual data to Cayenne.
    CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000);*/



}

void cayenne_task(void *param){
    //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");

    
    TimerInit(&timer);
    TimerCountdownMS(&timer, (double)5000);
    while(1){
        loop();


        //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");
    }

        //delay(2000);
    

    abort();
}