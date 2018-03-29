#include "esp32iot_cayenne.h"


static void copy_string(char *target, char *source)
{
    while(*source)
    {
        *target = *source;        
        source++;        
        target++;
    }    
    *target = '\0';
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
        printf("topic=Command");
        break;
    case CONFIG_TOPIC:
        printf("topic=Config");
        break;
    default:
        printf("topic=%d", message->topic);
        break;
    }
    printf(" channel=%d", message->channel);
    if (message->clientID) {
        printf(" clientID=%s", message->clientID);
    }
    if (message->type) {
        printf(" type=%s", message->type);
    }
    for (i = 0; i < message->valueCount; ++i) {
        if (message->values[i].value) {
            printf(" value=%s", message->values[i].value);
        }
        if (message->values[i].unit) {
            printf(" unit=%s", message->values[i].unit);
        }
    }
    if (message->id) {
        printf(" id=%s", message->id);
    }
    printf("\n");
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
            printf("Response failure, error: %d\n", error);
        }

        // Send the updated state for the channel so it is reflected in the Cayenne dashboard. If a command is successfully processed
        // the updated state will usually just be the value received in the command message.
        if ((error = CayenneMQTTPublishData(&mqttClient, message->clientID, DATA_TOPIC, message->channel, NULL, NULL, message->values[0].value)) != CAYENNE_SUCCESS) {
            printf("Publish state failure, error: %d\n", error);
        }
    }
}

void CayenneInit(const char* username_arg, const char* password_arg, const char* clientID_arg){
    static char* LOCAL_TAG = "CayenneInit";

    // copy_string(username, username_arg);
    // copy_string(password, password_arg);
    // copy_string(clientID, clientID_arg);
    // username = "MQTT_USERNAME";
    // password = "MQTT_PASSWORD";
    // clientID = "CLIENT_ID";

	// Initialize the network.
    NetworkInit(&network);

    // Initialize the Cayenne client.
    CayenneMQTTClientInit(&mqttClient, &network, username_arg, password_arg, clientID_arg, messageArrived);

    //TimerInit(&timer);
    //Start the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
    //TimerCountdown(&timer, 5);
}



/**
* Connect to the Cayenne server.
* @return Returns CAYENNE_SUCCESS if the connection succeeds, or an error code otherwise.
*/
int connectClient(void)
{
    static char* LOCAL_TAG = "connectClient";

    int error = 0;
    // Connect to the server.
    printf("Connecting to %s:%d\n", CAYENNE_DOMAIN, CAYENNE_PORT);
    while ((error = NetworkConnect(&network, CAYENNE_DOMAIN, CAYENNE_PORT)) != 0) {
        printf("TCP connect failed, error: %d\n", error);
        delay(2000);
    }

    if ((error = CayenneMQTTConnect(&mqttClient)) != MQTT_SUCCESS) {
        printf("MQTT connect failed, error: %d\n", error);
        return error;
    }
    printf("Connected\n");

    // Subscribe to required topics.
    if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
        printf("Subscription to Command topic failed, error: %d\n", error);
    }
    if ((error = CayenneMQTTSubscribe(&mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL)) != CAYENNE_SUCCESS) {
        printf("Subscription to Config topic failed, error:%d\n", error);
    }

    // Send device info. Here we just send some example values for the system info. These should be changed to use actual system data, or removed if not needed.
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_VERSION_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, CAYENNE_VERSION);
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "SMH_v2.0");
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_CPU_MODEL_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "smh_b_one");
    CayenneMQTTPublishData(&mqttClient, NULL, SYS_CPU_SPEED_TOPIC, CAYENNE_NO_CHANNEL, NULL, NULL, "240000000");

    return CAYENNE_SUCCESS;
}



int i = 0;
void cayenne_task(void)
{
    static char* LOCAL_TAG = "cayenne_task";
    
    Timer timer;
    TimerInit(&timer);
    //Start the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
    TimerCountdown(&timer, 5);
    
    while(1){
        i++;

        // Yield to allow MQTT message processing.
        int error = MQTTYield(&mqttClient, 1000);
        if(error != MQTT_SUCCESS){
            ESP_LOGE(LOCAL_TAG, "CayenneMQTTYield error: %d", error);
        }


        // Check that we are still connected, if not, reconnect.
        if (!NetworkConnected(&network) || !CayenneMQTTConnected(&mqttClient)) {
            NetworkDisconnect(&network);
            CayenneMQTTDisconnect(&mqttClient);
            printf("Reconnecting\n");
            while (connectClient() != CAYENNE_SUCCESS) {
                delay(2000);
                printf("Reconnect failed, retrying\n");
            }
        }

        // Publish some example data every few seconds. This should be changed to send your actual data to Cayenne.
        if (TimerIsExpired(&timer)) {
            int error = 0;
            if ((error = CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5 + i)) != CAYENNE_SUCCESS) {
                printf("Publish temperature failed, error: %d\n", error);
            }
            if ((error = CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000 + i)) != CAYENNE_SUCCESS) {
                printf("Publish luminosity failed, error: %d\n", error);
            }
            if ((error = CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 2, TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL, 800 + i)) != CAYENNE_SUCCESS) {
                printf("Publish barometric pressure failed, error: %d\n", error);
            }
            //Restart the countdown timer for publishing data every 5 seconds. Change the timeout parameter to publish at a different interval.
            TimerCountdown(&timer, 5);
        }

        printf(".\n");

        if(i >1000){
            i=0;
        }

    }

    // // Disconnect from Cayenne.
    // if(CayenneMQTTConnected(&mqttClient))
    //     CayenneMQTTDisconnect(&mqttClient);

    // // Disconnect network client.
    // if(NetworkConnected(&network))
    //     NetworkDisconnect(&network);
}