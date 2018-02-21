#include "esp32iot_webserver.h"


esp_err_t webserver_initialize(void){
	esp_err_t err;

	err = wifi_initialize();
	if(err != ESP_OK){
		ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

	err = wifi_sta_start("hotosk", "W6game555");
	if(err != ESP_OK){
		ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}
/*	err = wifi_scan_initialize();
	if(err != ESP_OK){
		ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}*/

	//char ssid[] = "hotosk";
    //char* password;
	//save_last_connected_wifi("asda", "asdasd");
/*	wifi_manager_state = 0;
    err = get_last_connected_wifi(&actual_wifi.ssid, &actual_wifi.password);
    if (err != ESP_OK){
        ESP_LOGW(webserver_tag, "( %d )", err);
        ESP_ERROR_CHECK( err );
    }else{
    	if(actual_wifi.password){
    		printf("Last connected wifi: ssid=%s password=%s\n",  actual_wifi.ssid, actual_wifi.password);
    		//ESP_ERROR_CHECK( wifi_sta_conect(ssid, password) );
    		err = wifi_sta_start(actual_wifi.ssid, actual_wifi.password);
			if(err != ESP_OK){
				ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
    	}else{
    		err = wifi_ap_create();
			if(err != ESP_OK){
				ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
    	}
	}*/

/*    err = save_wifi(ssid, "W6game55");
    if (err != ESP_OK){
        ESP_LOGW(webserver_tag, "( %d )", err);
        ESP_ERROR_CHECK( err );
    }*/
	
	return ESP_OK;
}