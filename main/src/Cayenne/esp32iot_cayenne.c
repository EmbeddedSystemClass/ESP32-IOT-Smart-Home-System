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


void loop(void)
{

    //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");
    //print_timer_counter(&timer);
    // Yield to allow MQTT message processing.
    //CayenneMQTTYield(&mqttClient, 1000);
    //TimerLeftMS(&Timer);
    //delay(2000);

    // Publish some example data every second. This should be changed to send your actual data to Cayenne.
    //CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5);
    //CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000);



}


void cayenne_task(void *param){
    //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");

    
    // TimerInit(&timer);
    // TimerCountdownMS(&timer, (double)10000);

/*    TimerInit(&timer);
    TimerCountdown(&timer, 10);*/

    Timer timer1;
    timer1.idx = TIMER_1;
    Timer timer;
    timer.idx = TIMER_0;

    TimerInit(&timer1);
    TimerCountdownMS(&timer1, 5000);

    TimerInit(&timer);
    TimerCountdownMS(&timer, 10000);

    while(1){
        //loop();

        ESP_LOGI(cayenne_tag, "TimerX left: timer: %d | timer1: %d\n", TimerLeftMS(&timer), TimerLeftMS(&timer1));
        if(TimerIsExpired(&timer) && TimerIsExpired(&timer1)){
            ESP_LOGI(cayenne_tag, "Timer has expired. Another timer 10s.\n");
            break;
        }

        //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");
    }

        //delay(2000);
    

    abort();
}