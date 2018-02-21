#include "esp32iot_storage.h"

char *nvs_event_to_string(int ev) {
	switch (ev) {
		case ESP_ERR_NVS_BASE:
			return "ESP_ERR_NVS_BASE";
		case ESP_ERR_NVS_NOT_INITIALIZED:
			return "ESP_ERR_NVS_NOT_INITIALIZED";
		case ESP_ERR_NVS_NOT_FOUND:
			return "ESP_ERR_NVS_NOT_FOUND";
		case ESP_ERR_NVS_TYPE_MISMATCH:
			return "ESP_ERR_NVS_TYPE_MISMATCH";
		case ESP_ERR_NVS_READ_ONLY:
			return "ESP_ERR_NVS_READ_ONLY";
		case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
			return "ESP_ERR_NVS_NOT_ENOUGH_SPACE";
		case ESP_ERR_NVS_INVALID_NAME:
			return "ESP_ERR_NVS_INVALID_NAME";
		case ESP_ERR_NVS_INVALID_HANDLE:
			return "ESP_ERR_NVS_INVALID_HANDLE";
		case ESP_ERR_NVS_REMOVE_FAILED:
			return "ESP_ERR_NVS_REMOVE_FAILED";
		case ESP_ERR_NVS_KEY_TOO_LONG:
			return "ESP_ERR_NVS_KEY_TOO_LONG";
		case ESP_ERR_NVS_PAGE_FULL:
			return "ESP_ERR_NVS_PAGE_FULL";
		case ESP_ERR_NVS_INVALID_STATE :
			return "ESP_ERR_NVS_INVALID_STATE";
		case ESP_ERR_NVS_NO_FREE_PAGES:
			return "ESP_ERR_NVS_NO_FREE_PAGES";
		case ESP_ERR_NVS_VALUE_TOO_LONG:
			return "ESP_ERR_NVS_VALUE_TOO_LONG";
		case ESP_ERR_NVS_PART_NOT_FOUND:
			return "ESP_ERR_NVS_VALUE_TOO_LONG";
	}

	static char unknown[100];
	sprintf(unknown, "Unknown event: %d", ev);
	return unknown;
}

esp_err_t storage_init(void){
	esp_err_t err;
	err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        err = nvs_flash_erase();
        if(err != ESP_OK) {
        	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
			ESP_ERROR_CHECK( err );
			return err;
		}
        err = nvs_flash_init();
        if(err != ESP_OK) {
        	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
			ESP_ERROR_CHECK( err );
			return err;
		}
    }
    else if(err != ESP_OK){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
		return err;
    }
    return err;
}

esp_err_t erase_storage(void){ //clears all chip memory
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(WIFI_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK( err );
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Read wifi ssid
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_erase_all(my_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 
    
    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t save_wifi(const char* ssid, const char* password){
	nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(WIFI_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
    	ESP_ERROR_CHECK( err );
    	return err;
    } 

    // Read the size of memory space required for str
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_str(my_handle, ssid, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
    	return err;
    }

    // Write password
    err = nvs_set_str(my_handle, ssid, password);
    if (err != ESP_OK){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
    	return err;
    } 

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
    	return err;
    } 

    // Close
    nvs_close(my_handle);

    return ESP_OK;
}

esp_err_t get_wifi(const char* ssid, char* password[]){
	nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(WIFI_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK( err );
    if (err != ESP_OK){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
    	return err;
    } 

    // Read wifi ssid
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_get_str(my_handle, ssid, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
		ESP_ERROR_CHECK( err );
    	return err;
    } 

    if (required_size == 0) {
        ESP_LOGI(storage_tag, "get_wifi: Nothing saved yet!\n");
    } else {
        char* found_password = malloc(required_size);
        err = nvs_get_str(my_handle, ssid, found_password, &required_size);
        if (err != ESP_OK){
	    	ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
			ESP_ERROR_CHECK( err );
	    	return err;
	    } else{
	    	*password = malloc(required_size);
	    	strcpy(*password, found_password);
        	//printf("WiFi: ssid=%s password=%s\n", ssid, *password); 

        	free(found_password);
	    }
	        
    }
	
	// Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t save_last_connected_wifi(const char* ssid, const char* password){
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(WIFI_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Read the size of memory space required for str
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_str(my_handle, "last_ssid", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    }

    // Write last connected wifi ssid
    err = nvs_set_str(my_handle, "last_ssid", ssid);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Read the size of memory space required for str
    required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_str(my_handle, "last_password", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    }

    // Write last connected wifi ssid
    err = nvs_set_str(my_handle, "last_password", password);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Close
    nvs_close(my_handle);

    return ESP_OK;
}

esp_err_t get_last_connected_wifi(const char* ssid[], char* password[]){
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(WIFI_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK( err );
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Read last connected wifi ssid
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    // obtain required memory space to store blob being read from NVS
    err = nvs_get_str(my_handle, "last_ssid", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    if (required_size == 0) {
        ESP_LOGI(storage_tag, "get_last_connected_wifi: No last connected wifi ssid saved!\n");
    } else {
        char* found_ssid = malloc(required_size);
        err = nvs_get_str(my_handle, "last_ssid", found_ssid, &required_size);
        if (err != ESP_OK){
            ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
            ESP_ERROR_CHECK( err );
            return err;
        } else{
            *ssid = malloc(required_size);
            strcpy(*ssid, found_ssid);
            //printf("WiFi: ssid=%s password=%s\n", key, *password); 

            free(found_ssid);
        }
            
    }

    required_size = 0;
    // Read last connected wifi password
    err = nvs_get_str(my_handle, "last_password", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    if (required_size == 0) {
        ESP_LOGI(storage_tag, "get_last_connected_wifi: No last connected wifi password saved!\n");
    } else {
        char* found_password = malloc(required_size);
        err = nvs_get_str(my_handle, "last_password", found_password, &required_size);
        if (err != ESP_OK){
            ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
            ESP_ERROR_CHECK( err );
            return err;
        } else{
            *password = malloc(required_size);
            strcpy(*password, found_password); 
            free(found_password);
        }
            
    }
    
    // Close
    nvs_close(my_handle);

    return ESP_OK;
}

esp_err_t save_mqtt(void){
	nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(MQTT_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
    	ESP_ERROR_CHECK( err );
    	return err;
    } 

	// Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t get_mqtt(void){
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(MQTT_SETTINGS_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t save_restart_counter(void){
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(GENERAL_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Read
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 
    // Write
    restart_counter++;
    err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    err = nvs_commit(my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
        ESP_ERROR_CHECK( err );
        return err;
    } 

    // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t get_restart_counter(void){
	nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(GENERAL_STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
    	ESP_ERROR_CHECK( err );
    	return err;
    } 

    // Read restart counter
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGW(storage_tag, "%s", nvs_event_to_string(err));
    	ESP_ERROR_CHECK( err );
    	return err;
    } 
    printf("Restart counter = %d\n", restart_counter);
	
	// Close
    nvs_close(my_handle);
    return ESP_OK;
}