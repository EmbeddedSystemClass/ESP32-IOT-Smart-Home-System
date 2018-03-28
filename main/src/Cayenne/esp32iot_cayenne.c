#include "esp32iot_cayenne.h"

void CayenneInit(const char* username, const char* password, const char* clientID){
    static char* LOCAL_TAG = "CayenneInit";

	// Initialize the network.
    NetworkInit(&network);

    // Initialize the Cayenne client.
    CayenneMQTTClientInit(&mqttClient, &network, username, password, clientID, NULL);

    TimerInit(&timer);
    //Start the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
    TimerCountdown(&timer, 5);
}

// Connect to the Cayenne server.
int connectClient(void)
{
    static char* LOCAL_TAG = "connectClient";

    int error = 0;
    // Connect to the server.
    ESP_LOGI(LOCAL_TAG, "Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
    while ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
        if (finished)
            return error;
        ESP_LOGE(LOCAL_TAG, "TCP connect failed, error: %d\n", error);
        delay(2000);
    }

    if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {
        ESP_LOGE(LOCAL_TAG, "MQTT connect failed, error: %d\n", error);
        return error;
    }
    ESP_LOGI(LOCAL_TAG, "Connected\n");

    // Subscribe to required topics.
    if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
        ESP_LOGE(LOCAL_TAG, "Subscription to Command topic failed, error: %d\n", error);
    }
    if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
        ESP_LOGE(LOCAL_TAG, "Subscription to Config topic failed, error:%d\n", error);
    }

    // Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "smh001");
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "smh_cpu");
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "240000000");

    return CAYENNE_SUCCESS;
}

/**
* Print the message info.
* @param[in] message The message received from the Cayenne server.
*/
void outputMessage(CayenneMessageData* message)
{
    static char* LOCAL_TAG = "outputMessage";

    int i;
    switch (message->topic) {
    case COMMAND_TOPIC:
        ESP_LOGI(LOCAL_TAG, "topic=Command");
        break;
    case CONFIG_TOPIC:
        ESP_LOGI(LOCAL_TAG, "topic=Config");
        break;
    default:
        ESP_LOGI(LOCAL_TAG, "topic=%d", message->topic);
        break;
    }
    ESP_LOGI(LOCAL_TAG, " channel=%d", message->channel);
    if (message->clientID) {
        ESP_LOGI(LOCAL_TAG, " clientID=%s", message->clientID);
    }
    if (message->type) {
        ESP_LOGI(LOCAL_TAG, " type=%s", message->type);
    }
    for (i = 0; i < message->valueCount; ++i) {
        if (message->values[i].value) {
            ESP_LOGI(LOCAL_TAG, " value=%s", message->values[i].value);
        }
        if (message->values[i].unit) {
            ESP_LOGI(LOCAL_TAG, " unit=%s", message->values[i].unit);
        }
    }
    if (message->id) {
        ESP_LOGI(LOCAL_TAG, " id=%s", message->id);
    }
    ESP_LOGI(LOCAL_TAG, "\n");
}

/**
* Handle messages received from the Cayenne server.
* @param[in] message The message received from the Cayenne server.
*/
void messageArrived(CayenneMessageData* message)
{
    static char* LOCAL_TAG = "messageArrived";

    int error = 0;
    // Add code to process the message. Here we just ouput the message data.
    outputMessage(message);

    if (message->topic == COMMAND_TOPIC) {
        // If this is a command message we publish a response to show we recieved it. Here we are just sending a default 'OK' response.
        // An error response should be sent if there are issues processing the message.
        if ((error = CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, NULL)) != CAYENNE_SUCCESS) {
            ESP_LOGE(LOCAL_TAG, "Response failure, error: %d\n", error);
        }

        // Send the updated state for the channel so it is reflected in the Cayenne dashboard. If a command is successfully processed
        // the updated state will usually just be the value received in the command message.
        if ((error = CayenneMQTTPublishData(&mqttClient, message->clientID, DATA_TOPIC, message->channel, NULL, NULL, message->values[0].value)) != CAYENNE_SUCCESS) {
            ESP_LOGE(LOCAL_TAG, "Publish state failure, error: %d\n", error);
        }
    }
}


void cayenne_task(void)
{
    static char* LOCAL_TAG = "cayenne_task";

    //while (!finished) {
        // Yield to allow MQTT message processing.
        //CayenneMQTTYield(&mqttClient, 1000);
        //delay(1000);
        CayenneMQTTYield(&mqttClient, 1000);
        // Check that we are still connected, if not, reconnect.
        if (!NetworkConnected(&network) || !CayenneMQTTConnected(&mqttClient)) {
            NetworkDisconnect(&network);
            CayenneMQTTDisconnect(&mqttClient);
            ESP_LOGI(LOCAL_TAG, "Reconnecting\n");
            while (connectClient() != CAYENNE_SUCCESS) {
                if (finished)
                    return;
                delay(2000);
                ESP_LOGE(LOCAL_TAG, "Reconnect failed, retrying\n");
            }
        }

        // Publish some example data every few seconds. This should be changed to send your actual data to Cayenne.
        if (TimerIsExpired(&timer)) {
            int error = 0;
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, BA5C_data.HTU21D_temperature)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.HTU21D_temperature failed, error: %d\n", error);
            }
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT, BA5C_data.HTU21D_humidity)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.HTU21D_humidity failed, error: %d\n", error);
            }
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 2, NULL, UNIT_UNDEFINED, BA5C_data.HTU21D_status)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.HTU21D_status failed, error: %d\n", error);
            }

            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 3, TYPE_TEMPERATURE, UNIT_CELSIUS, BA5C_data.MS5637_temperature)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.MS5637_temperature failed, error: %d\n", error);
            }
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 4, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, BA5C_data.MS5637_pressure)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.MS5637_pressure failed, error: %d\n", error);
            }
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 5, NULL, UNIT_UNDEFINED, BA5C_data.MS5637_status)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.MS5637_status failed, error: %d\n", error);
            }

            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 6, TYPE_BATTERY, UNIT_PERCENT, BA5C_data.Battery_level)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.Battery_level failed, error: %d\n", error);
            }
            if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 7, NULL, UNIT_UNDEFINED, BA5C_data.Battery_status)) != CAYENNE_SUCCESS) {
                ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.Battery_status failed, error: %d\n", error);
            }

           /* ESP_LOGE(LOCAL_TAG, "BA5C_data.MS5637_status: %d\n", BA5C_data.MS5637_status);
            ESP_LOGE(LOCAL_TAG, "BA5C_data.Battery_status: %d\n", BA5C_data.Battery_status);*/
            // if ((CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 8, NULL, UNIT_UNDEFINED, BA5C_data.registered_notify_services)) != CAYENNE_SUCCESS) {
            //     ESP_LOGE(LOCAL_TAG, "Publish BA5C_data.registered_notify_services failed, error: %d\n", error);
            // }

            //Restart the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
            TimerCountdown(&timer, 5);
     //   }
    }


/*    static char* LOCAL_TAG = "cayenne_task";

    //ESP_LOGI(LOCAL_TAG, "Pozdro 600 na rejonie\n");
    //CayenneMQTTYield(&mqttClient, 1);
   // delay(1000);
    CayenneMQTTYield(&mqttClient, 1000);

    // CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5+i);
    // CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000+i);

    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, BA5C_data.HTU21D_temperature);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT, BA5C_data.HTU21D_humidity);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 2, TYPE_STATUS, UNIT_DIGITAL, BA5C_data.HTU21D_status);

    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 3, TYPE_TEMPERATURE, UNIT_CELSIUS, BA5C_data.MS5637_temperature);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 4, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, BA5C_data.MS5637_pressure);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 5, TYPE_STATUS, UNIT_DIGITAL, BA5C_data.MS5637_status);

    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 6, TYPE_BATTERY, UNIT_PERCENT, BA5C_data.Battery_level);
    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 7, TYPE_STATUS, UNIT_DIGITAL, BA5C_data.Battery_status);

    CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 8, TYPE_STATUS, UNIT_DIGITAL, BA5C_data.registered_notify_services);

*/



    //i++;


}