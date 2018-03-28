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

// Handle messages received from the Cayenne server.
void messageArrived(CayenneMessageData* message)
{
    static char* LOCAL_TAG = "messageArrived";
    ESP_LOGW(LOCAL_TAG, "Message received on channel %d\n", message->channel);
    
    // Add code to process the message here.
    if (message->topic == COMMAND_TOPIC) {
        // If this is a command message we publish a response to show we recieved it. Here we are just sending a default 'OK' response.
        // An error response should be sent if there are issues processing the message.
        CayenneMQTTPublishResponse(&mqttClient, message->clientID, message->id, NULL);

        // Send the updated state for the channel so it is reflected in the Cayenne dashboard. If a command is successfully processed
        // the updated state will usually just be the value received in the command message.
        CayenneMQTTPublishData(&mqttClient, message->clientID, DATA_TOPIC, message->channel, NULL, NULL, message->values[0].value);
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



// Connect to the Cayenne server.
int connectClient(void)
{
    static char* LOCAL_TAG = "connectClient";

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

    // Subscribe to required topics. Here we subscribe to the Command and Config topics.
    CayenneMQTTSubscribe(&mqttClient, NULL, COMMAND_TOPIC, CAYENNE_ALL_CHANNELS, NULL);
    CayenneMQTTSubscribe(&mqttClient, NULL, CONFIG_TOPIC, CAYENNE_ALL_CHANNELS, NULL);

    return CAYENNE_SUCCESS;
}



int i = 0;
void cayenne_task(void)
{
    static char* LOCAL_TAG = "cayenne_task";
    //if(CayenneMQTTConnected(&mqttClient)){
        // Yield to allow MQTT message processing.
        i++;
        int error = 0;
        error = MQTTYield(&mqttClient, 1000);
        if(error != MQTT_SUCCESS){
            ESP_LOGE(LOCAL_TAG, "CayenneMQTTYield error: %d", error);
        }
        //delay(1000);
        // Publish some example data every second. This should be changed to send your actual data to Cayenne.
        CayenneMQTTPublishDataFloat(&mqttClient, NULL, DATA_TOPIC, 0, TYPE_TEMPERATURE, UNIT_CELSIUS, 30.5+i);
        CayenneMQTTPublishDataInt(&mqttClient, NULL, DATA_TOPIC, 1, TYPE_LUMINOSITY, UNIT_LUX, 1000+i);
        printf(".\n");

        if(i >1000){
            i=0;
        }

}