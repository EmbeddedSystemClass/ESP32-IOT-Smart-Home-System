#include "esp32iot_wifi.h"

char *wifi_err_to_string(int ev) {
	switch (ev) {
		case ESP_ERR_WIFI_OK:
			return "ESP_ERR_WIFI_OK";
		case ESP_ERR_WIFI_FAIL:
			return "ESP_ERR_WIFI_FAIL";
		case ESP_ERR_WIFI_NO_MEM:
			return "ESP_ERR_WIFI_NO_MEM";
		case ESP_ERR_WIFI_ARG:
			return "ESP_ERR_WIFI_ARG";
		case ESP_ERR_WIFI_NOT_SUPPORT:
			return "ESP_ERR_WIFI_NOT_SUPPORT";
		case ESP_ERR_WIFI_NOT_INIT:
			return "ESP_ERR_WIFI_NOT_INIT";
		case ESP_ERR_WIFI_NOT_STARTED:
			return "ESP_ERR_WIFI_NOT_STARTED";
		case ESP_ERR_WIFI_NOT_STOPPED:
			return "ESP_ERR_WIFI_NOT_STOPPED";
		case ESP_ERR_WIFI_IF:
			return "ESP_ERR_WIFI_IF";
		case ESP_ERR_WIFI_MODE:
			return "ESP_ERR_WIFI_MODE";
		case ESP_ERR_WIFI_STATE:
			return "ESP_ERR_WIFI_STATE";
		case ESP_ERR_WIFI_CONN:
			return "ESP_ERR_WIFI_CONN";
		case ESP_ERR_WIFI_NVS:
			return "ESP_ERR_WIFI_NVS";
		case ESP_ERR_WIFI_MAC:
			return "ESP_ERR_WIFI_MAC";
		case ESP_ERR_WIFI_SSID:
			return "ESP_ERR_WIFI_SSID";
		case ESP_ERR_WIFI_PASSWORD:
			return "ESP_ERR_WIFI_PASSWORD";
		case ESP_ERR_WIFI_TIMEOUT:
			return "ESP_ERR_WIFI_TIMEOUT";
		case ESP_ERR_WIFI_WAKE_FAIL:
			return "ESP_ERR_WIFI_WAKE_FAIL";
		case ESP_ERR_WIFI_WOULD_BLOCK:
			return "ESP_ERR_WIFI_WOULD_BLOCK";
		case ESP_ERR_WIFI_NOT_CONNECT:
			return "ESP_ERR_WIFI_NOT_CONNECT";


	}

	static char unknown[100];
	sprintf(unknown, "Unknown event: %d", ev);
	return unknown;
}

esp_err_t wifi_scan_start(void) {
	esp_err_t err;

	wifi_scan_config_t scanConf = {
	    .ssid = NULL,
	    .bssid = NULL,
	    .channel = 0,
	    .show_hidden = false
	};

	err = esp_wifi_scan_start(&scanConf, true);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	//vTaskDelete(NULL);

	return ESP_OK;
}

esp_err_t wifi_scan_get(void ) {
	esp_err_t err;

	uint16_t apCount = 0;
    esp_wifi_scan_get_ap_num(&apCount);
    if (apCount == 0) {
        ESP_LOGI(wifi_tag, "mg_event_handler: No APs found");
        return 0006;//error id
    }
    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
    if (!ap_list) {
        ESP_LOGI(wifi_tag, "mg_event_handler: malloc error, ap_list is NULL");
        return 0007;//error id
    }

    err = esp_wifi_scan_get_ap_records(&apCount, ap_list);
	if(err != ESP_OK){
		ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

    for (int i = 0; i < apCount; ++i)
    {
        printf("\n%s", ap_list[i].ssid);
        //fflush(stdout);
    }
    free(ap_list);

	return ESP_OK;
}


// static void cayenne_task1(void *pvParameters){

// 	while(1){
// 		//ESP_LOGI(wifi_tag, "Pozdro 600 na rejonie\n");
// 		loop();
// 		//vTaskDelay(5000 / portTICK_RATE_MS);
// 	}

// }
static void cayenne_task_handler(void *param){
    
    while(1){
        cayenne_task();
    }





/*    Timer timer1;
    Timer timer;

    TimerInit(&timer1);
    TimerCountdownMS(&timer1, 5000);

    TimerInit(&timer);
    TimerCountdownMS(&timer, 10000);

    while(1){
        ESP_LOGI(cayenne_tag, "TimerX left: timer: %d | timer1: %d\n", TimerLeftMS(&timer), TimerLeftMS(&timer1));
        if(TimerIsExpired(&timer) && TimerIsExpired(&timer1)){
            ESP_LOGI(cayenne_tag, "Timer has expired. Another timer 10s.\n");
        }
    }*/
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
	static esp_err_t err;

/*	if(network.connected == 1){
		loop();
	}*/

	switch (event->event_id) {
		case SYSTEM_EVENT_WIFI_READY:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_WIFI_READY");
        	
     		break;
     	case SYSTEM_EVENT_SCAN_DONE:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_SCAN_DONE");
        	err = wifi_scan_get();
        	if(err != ESP_OK){
				ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
     		break;
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_START");
            //wifi_manager_state = WIFI_MANAGER_CONNECTION_ATTEMPT_STA;
        	err = esp_wifi_connect();
			if(err != ESP_OK){
				ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
            break;
        case SYSTEM_EVENT_STA_STOP:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_STOP");

            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_CONNECTED");
            //connection_failure_counter = 0;

            //wifi_manager_state = WIFI_MANAGER_CONNECTED_STA;
            save_last_connected_wifi(actual_wifi.ssid, actual_wifi.password);
            save_wifi(actual_wifi.ssid, actual_wifi.password);
            
   //          char username[100];
   //          char password[100];
   //          char clientID[100];
   //          err = get_last_connected_mqtt(username, password, clientID);
		 //    if (err != ESP_OK){
		 //        ESP_LOGE(wifi_tag, "( %d )", err);
		 //        ESP_ERROR_CHECK( err );
		 //    }else{
		 //    	if(strcmp(username, "") && strcmp(password, "") && strcmp(clientID, "")){
		 //    		ESP_LOGI(wifi_tag, "Last connected mqtt: username=%s password=%s clientID=%s\n", username, password, clientID);
		    		
		 //    		CayenneInit(username, password, clientID);

			// 		// Connect to Cayenne.
			// 		/*err = connectClient();
			// 		if(err != CAYENNE_SUCCESS){
			// 			ESP_LOGE(wifi_tag, "%s", "Cayenne MQTT connection failed.\n");
			// 			//ESP_ERROR_CHECK( err );
			// 		}else{

			// 		}*/

			// 		while(connectClient() != CAYENNE_SUCCESS){
			// 			ESP_LOGE(wifi_tag, "%s", "Cayenne MQTT connection failed.\n");
			// 		}
		 //    	}else{

		 //    	}cayenne_task
			// }
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_DISCONNECTED");
            //xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

            /*connection_failure_counter++;
            if(connection_failure_counter < MAX_WIFI_CONNECTION_ATTEMPTS){

            	err = esp_wifi_connect();
				if(err != ESP_OK){
					ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
					ESP_ERROR_CHECK( err );
				}
            }else{
            	ESP_LOGI(wifi_tag, "Cannot connect to WiFi. Creating Access Point.");
            	wifi_ap_start();

            	connection_failure_counter = 0;
            }*/

            esp_err_t err = wifi_ap_configure();
			if(err != ESP_OK){
				ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}else{
				ESP_ERROR_CHECK(esp_wifi_start());
			}
            
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
        	
     		break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(wifi_tag, "wifi_event_handler:  SYSTEM_EVENT_STA_GOT_IP IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            
            //xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        	//http_server_init();
            
            static char username[100];
            static char password[100];
            static char clientID[100];
            err = get_last_connected_mqtt(username, password, clientID);
		    if (err != ESP_OK){
		        ESP_LOGE(wifi_tag, "( %d )", err);
		        ESP_ERROR_CHECK( err );
		    }else{
		    	if(strcmp(username, "") && strcmp(password, "") && strcmp(clientID, "")){
		    		ESP_LOGI(wifi_tag, "Last connected mqtt: username=%s password=%s clientID=%s\n", username, password, clientID);
		    		
		    		delay(1000);

		    		CayenneInit(username, password, clientID);
				    while(connectClient() != CAYENNE_SUCCESS){
				        delay(2000);
				        ESP_LOGE(wifi_tag, "%s", "Cayenne MQTT connection failed.\n");   
				    }
				    xTaskCreate(&cayenne_task, "cayenne_task", 4096, NULL, 5, NULL);
		    		
		    		//!CayenneMQTTConnected(&mqttClient)

		    	}else{

		    	}
			}
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_LOST_IP");
        	
        	/*err = esp_wifi_disconnect();
			if(err != ESP_OK){
				ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
            connection_failure_counter = 0;*/
            
     		break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_SUCCESS");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_FAILED");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");

     		break;
     	case SYSTEM_EVENT_STA_WPS_ER_PIN:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_STA_WPS_ER_PIN");

     		break;    
        case SYSTEM_EVENT_AP_START:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_START");
        	//wifi_manager_state = WIFI_MANAGER_CONNECTION_ATTEMPT_AP;

     		break;
     	case SYSTEM_EVENT_AP_STOP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STOP");
        	
     		break;
     	case SYSTEM_EVENT_AP_STADISCONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STADISCONNECTED, MAC=%02x:%02x:%02x:%02x:%02x:%02x AID=%i",
				event->event_info.sta_disconnected.mac[0],event->event_info.sta_disconnected.mac[1],
				event->event_info.sta_disconnected.mac[2],event->event_info.sta_disconnected.mac[3],
				event->event_info.sta_disconnected.mac[4],event->event_info.sta_disconnected.mac[5],
				event->event_info.sta_disconnected.aid);
        	
        	//xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        	
        	/*connection_failure_counter++;
            if(connection_failure_counter < MAX_WIFI_CONNECTION_ATTEMPTS){
            	//wifi_ap_start();
            	//STA WIFI CONNECT
            	err = wifi_sta_start(actual_wifi.ssid, actual_wifi.password);
				if(err != ESP_OK){
					ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
					ESP_ERROR_CHECK( err );
				}
            }else{
            	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STADISCONNECTED: Major error. Cannot create WiFi Access Point.");

            	connection_failure_counter = 0;

            	ESP_LOGI(wifi_tag, "Restarting now.");
			    //esp_restart();
            }*/
     		break;  
     	case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STACONNECTED, MAC=%02x:%02x:%02x:%02x:%02x:%02x AID=%i",
		        event->event_info.sta_connected.mac[0],event->event_info.sta_connected.mac[1],
		        event->event_info.sta_connected.mac[2],event->event_info.sta_connected.mac[3],
		        event->event_info.sta_connected.mac[4],event->event_info.sta_connected.mac[5],
		        event->event_info.sta_connected.aid);

            //ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_STACONNECTED IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

            //xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        	//http_server_init();

     		break;
     	case SYSTEM_EVENT_AP_PROBEREQRECVED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_AP_PROBEREQRECVED");
        	
     		break;
     	case SYSTEM_EVENT_GOT_IP6:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_GOT_IP6 IP6=%01x:%01x:%01x:%01x",
        		event->event_info.got_ip6.ip6_info.ip.addr[0],event->event_info.got_ip6.ip6_info.ip.addr[1],
        		event->event_info.got_ip6.ip6_info.ip.addr[2],event->event_info.got_ip6.ip6_info.ip.addr[3]);
        	
     		break;
     	case SYSTEM_EVENT_ETH_START:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_START");
        	
     		break;
     	case SYSTEM_EVENT_ETH_STOP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_STOP");
        	
     		break;
     	case SYSTEM_EVENT_ETH_CONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_CONNECTED");
        	
        	break;
     	case SYSTEM_EVENT_ETH_DISCONNECTED:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_DISCONNECTED");
        	
     		break;
     	case SYSTEM_EVENT_ETH_GOT_IP:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_ETH_GOT_IP");
        	
     		break;
     	case SYSTEM_EVENT_MAX:
        	ESP_LOGI(wifi_tag, "wifi_event_handler: SYSTEM_EVENT_MAX");
        	
     		break;
        default:
			ESP_LOGW(wifi_tag, "( %d )", event->event_id);
        	
     		break;
	}
	return ESP_OK;
}

esp_err_t wifi_initialize(void)
{
	esp_err_t err;
	
	const char* LOCAL_TAG = "wifi_initialize";

	tcpip_adapter_init();
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	//tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP,"esp32");
	tcpip_adapter_ip_info_t info;
	memset(&info, 0, sizeof(info));
	IP4_ADDR(&info.ip, 192, 168, 4, 1);
	IP4_ADDR(&info.gw, 192, 168, 4, 1);
	IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	ESP_LOGI(LOCAL_TAG,"Setting gateway IP");
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	//ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP,"esp32"));
	
	ESP_LOGI(LOCAL_TAG,"Starting DHCPS adapter");
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

/*	static char *hostname = "smart_home_system";
	ESP_LOGI(LOCAL_TAG,"set hostname to \"%s\"",hostname);
	ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP,hostname));*/
	
	ESP_LOGI(LOCAL_TAG,"Starting event loop");
	err = esp_event_loop_init(wifi_event_handler, NULL);
	if(err != ESP_OK){
		ESP_LOGW(LOCAL_TAG, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	ESP_LOGI(LOCAL_TAG,"Initializing WiFi");
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	
  	ESP_LOGI(LOCAL_TAG,"WiFi Initialized");

	return ESP_OK;
}




esp_err_t wifi_ap_configure(void ) {
	esp_err_t err;

	const char* LOCAL_TAG = "wifi_ap_configure";

/*	static char *hostname = "smart_home_system";
	ESP_LOGI(LOCAL_TAG,"set hostname to \"%s\"",hostname);
	ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP,hostname));*/

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

	wifi_config_t wifi_config = {
		.ap = {
		  .ssid=DEFAULT_AP_SSID,
		  .password=DEFAULT_AP_PASSWORD,
		  .channel = 0,
		  .authmode = DEFAULT_AP_AUTHMODE,
		  .ssid_hidden = 0,
		  .max_connection = 4,
		  .beacon_interval = 100
		},
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));


  	ESP_LOGI(LOCAL_TAG,"WiFi AP Configured");


	return ESP_OK;
}

esp_err_t wifi_sta_configure(const char ssid[], const char password[]) {
	esp_err_t err;

	const char* LOCAL_TAG = "wifi_sta_configure";

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = "",
			.password = "",
			.bssid_set = false,
			.channel = 1.
		},
	};

	for(size_t i = 0; i<= strlen(ssid); ++i)
      wifi_config.sta.ssid[i] = ssid[i];
	for(size_t i = 0; i<= strlen(password); ++i)
      wifi_config.sta.password[i] = password[i];  

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));


  	ESP_LOGI(LOCAL_TAG,"WiFi STA Configured");

	return ESP_OK;
}

esp_err_t wifi_apsta_configure(const char ssid[], const char password[]) {
	esp_err_t err;

	const char* LOCAL_TAG = "wifi_apsta_configure";

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

	wifi_config_t wifi_config = {
		.ap = {
		  .ssid=DEFAULT_AP_SSID,
		  .password=DEFAULT_AP_PASSWORD,
		  .channel = 0,
		  .authmode = WIFI_AUTH_WPA2_PSK,
		  .ssid_hidden = 0,
		  .max_connection = 4,
		  .beacon_interval = 100
		},
		.sta = {
			.ssid = "",
			.password = "",
			.bssid_set = false,
			.channel = 1.
		},
	};

	for(size_t i = 0; i<= strlen(ssid); ++i)
      wifi_config.sta.ssid[i] = ssid[i];
	for(size_t i = 0; i<= strlen(password); ++i)
      wifi_config.sta.password[i] = password[i];  

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));


  	ESP_LOGI(LOCAL_TAG,"WiFi Configured");

	return ESP_OK;
}

esp_err_t wifi_start(void){
	esp_err_t err;

	const char* LOCAL_TAG = "wifi_start";

	err = wifi_initialize();
	if(err != ESP_OK){
		ESP_LOGW(LOCAL_TAG, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

    err = get_last_connected_wifi(&actual_wifi.ssid, &actual_wifi.password);
    if (err != ESP_OK){
        ESP_LOGW(LOCAL_TAG, "( %d )", err);
        ESP_ERROR_CHECK( err );
    }else{
    	if(strcmp(actual_wifi.ssid, "") && strcmp(actual_wifi.password, "")){
    		ESP_LOGW(LOCAL_TAG, "Last connected wifi: ssid=%s password=%s\n",  actual_wifi.ssid, actual_wifi.password);
    		//ESP_ERROR_CHECK( wifi_sta_conect(ssid, password) );
    		err = wifi_sta_configure(actual_wifi.ssid, actual_wifi.password);
			if(err != ESP_OK){
				ESP_LOGW(LOCAL_TAG, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}else{
				ESP_ERROR_CHECK(esp_wifi_start());
			}
    	}else{
    		err = wifi_ap_configure();
			if(err != ESP_OK){
				ESP_LOGW(LOCAL_TAG, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}else{
				ESP_ERROR_CHECK(esp_wifi_start());
			}
    	}
	}
	
	return ESP_OK;
}
