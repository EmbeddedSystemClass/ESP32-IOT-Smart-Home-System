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

//int i = 0;
void cayenne_task(void)
{

    //ESP_LOGI(cayenne_tag, "Pozdro 600 na rejonie\n");
    //CayenneMQTTYield(&mqttClient, 1);
    delay(1000);

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





    //i++;


}