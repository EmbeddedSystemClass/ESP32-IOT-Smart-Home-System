#include "esp32iot_bluetooth.h"

int hexadecimalToDecimal(char hexVal[])
{   
    int len = strlen(hexVal);
     
    // Initializing base value to 1, i.e 16^0
    int base = 1;
     
    int dec_val = 0;
     
    // Extracting characters as digits from last character
    for (int i=len-1; i>=0; i--)
    {   
        // if character lies in '0'-'9', converting 
        // it to integral 0-9 by subtracting 48 from
        // ASCII value.
        if (hexVal[i]>='0' && hexVal[i]<='9')
        {
            dec_val += (hexVal[i] - 48)*base;
                 
            // incrementing base by power
            base = base * 16;
        }
 
        // if character lies in 'A'-'F' , converting 
        // it to integral 10 - 15 by subtracting 55 
        // from ASCII value
        else if (hexVal[i]>='A' && hexVal[i]<='F')
        {
            dec_val += (hexVal[i] - 55)*base;
         
            // incrementing base by power
            base = base*16;
        }
    }
     
    return dec_val;
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_REG_EVT");
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GATTC_CONNECT_EVT:{
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_CONNECT_EVT conn_id %d, if %d", p_data->connect.conn_id, gattc_if);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    }
    case ESP_GATTC_OPEN_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT");
        if (param->open.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "open failed, status %d", p_data->open.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "open success");
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"config mtu failed, error status = %x", param->cfg_mtu.status);
        }
        //printf("%x %x\n", remote_filter_service_uuid.uuid.uuid128[0], remote_filter_service_uuid.uuid.uuid128[16]);
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_CFG_MTU_EVT, Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &HTU21D_service_uuid);
        /*esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &HTU21D_data_char_uuid);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &HTU21D_status_char_uuid);*/
        //esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &MS5637_service_uuid);
        /*esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &MS5637_data_char_uuid);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &MS5637_calibration_char_uuid);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &MS5637_status_char_uuid);*/
        //esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &battery_service_uuid);
        /*esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &battery_data_char_uuid);*/
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_RES_EVT");
        esp_gatt_srvc_id_t *srvc_id =(esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
         //= ;
        //strcpy(temp_uuid.uuid.uuid128, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128);
        //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, HTU21D_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "HTU21D_service_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }
/*        else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, HTU21D_data_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "HTU21D_data_char_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, HTU21D_status_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "HTU21D_status_char_uuidfound");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }*/
        else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, MS5637_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "MS5637_service_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }
/*        else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, MS5637_data_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "MS5637_data_char_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, MS5637_calibration_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "MS5637_calibration_char_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, MS5637_status_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "MS5637_status_char_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }*/
        else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, battery_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "battery_service_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }
/*        else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, battery_data_char_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "battery_data_char_uuid found");
            get_server = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle = p_data->search_res.end_handle;
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        }*/
        else{
          ESP_LOGI(GATTC_TAG, "service not found");  
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SEARCH_CMPL_EVT");

        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        
        if (get_server){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }

        //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, p_data->search_res.srvc_id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
/*        ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            //ESP_LOGI(GATTC_TAG, "res: %d", memcmp(srvc_id->id.uuid.uuid.uuid128, HTU21D_service_uuid.uuid.uuid128, ESP_UUID_LEN_128));
            if(!memcmp(srvc_id->id.uuid.uuid.uuid128, HTU21D_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)){
                //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, p_data->search_res.srvc_id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
                ESP_LOGI(GATTC_TAG, "HTU21D characteristics handling:");
            }else if(!memcmp(srvc_id->id.uuid.uuid.uuid128, MS5637_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)){
                //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, p_data->search_res.srvc_id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
                ESP_LOGI(GATTC_TAG, "MS5637 characteristics handling:");
            }else if(!memcmp(srvc_id->id.uuid.uuid.uuid128, battery_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)){
                //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, p_data->search_res.srvc_id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
                ESP_LOGI(GATTC_TAG, "battery characteristics handling:");

            }
*/
            ESP_LOGI(GATTC_TAG, "count_pre: %d", count);
            if (count > 0){

                char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else{
                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             HTU21D_data_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "HTU21D data characteristics found");
                    }else if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             HTU21D_status_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "HTU21D status characteristics found");
                    }else if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             MS5637_data_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "MS5637 data characteristics found");
                    }else if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             MS5637_calibration_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "MS5637 calibration characteristics found");
                    }else if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             MS5637_status_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "MS5637 status characteristics found");
                    }else if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                             battery_data_char_uuid,
                                                             char_elem_result,
                                                             &count) == ESP_GATT_OK){
                        ESP_LOGI(GATTC_TAG, "battery data characteristics found");
                    }else{
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }
                    ESP_LOGI(GATTC_TAG, "count_post: %d", count);
                   /* esp_ble_gattc_get_all_descr(esp_gatt_if_t gattc_if,
                                              uint16_t conn_id,
                                              uint16_t char_handle,
                                              esp_gattc_descr_elem_t *result,
                                              uint16_t *count, uint16_t offset);*/

                    /*static esp_gattc_char_elem_t *result;
                        esp_ble_gattc_get_all_descr(gattc_if,
                                                    p_data->search_cmpl.conn_id,
                                                    gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                    result,
                                                    0, 1);
                        ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, result->uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);

                    for(int i = 0; i < count; ++i){

                    }*/
                    
                    /*for(int i = 0; i < strlen(char_elem_result); ++i){
                        ESP_LOGI(GATTC_TAG, "char_elem_result[%d].properties: %d", i, char_elem_result[i].properties);
                        if ((char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                            gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[i].char_handle;
                            esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, char_elem_result[i].char_handle);
                            ESP_LOGI(GATTC_TAG, "char_elem_result[%d].properties: %d", i, char_elem_result[i].properties);
                            break;
                        }
                         
                    }*/
                    /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_A_APP_ID].remote_bda, char_elem_result[0].char_handle);
                    }       
                }
                
                /* free char_elem_result */
                free(char_elem_result);
            }else{
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
         break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_REG_FOR_NOTIFY_EVT");
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "REG FOR NOTIFY failed: error status = %d", p_data->reg_for_notify.status);
        }else{
            uint16_t count = 0;
            uint16_t notify_en = 1;
            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         notify_descr_uuid,
                                                                         descr_elem_result,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                                                     descr_elem_result[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }

        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        if (p_data->notify.is_notify){
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, receive notify value:");
        }else{
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, receive indicate value:");
        }
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
        //ESP_LOGI(GATTC_TAG, "temperature=%x | humidity=%x ", p_data->notify.value[0], p_data->notify.value[1]);
        //char temp[4] = 
        char temp_temperature[]={p_data->notify.value[0], p_data->notify.value[1]};
        char temp_humidity[]={p_data->notify.value[3], p_data->notify.value[4]};
       // char *p1, *p2;
        //temperature = strtol(temp_temperature, NULL, 16);
        //humidity = strtol(temp_humidity, NULL, 16);
        temperature = (p_data->notify.value[1]) | (p_data->notify.value[0] << 8);
        humidity = (p_data->notify.value[4]) | (p_data->notify.value[3] << 8);
        ESP_LOGI(GATTC_TAG, "1 temperature=%d | humidity=%d ", temperature, humidity);

        temperature=-46.85 + 175.72 * temperature/65536; //(°C) = -46.85 + 175.72 x Temperature Word / 2 16
        humidity=-6 + 125 * humidity/65536; //(%RH) = -6 + 125 x Humidity Word / 2 16
        //ESP_LOGI(GATTC_TAG, "temperature=%s | humidity=%s ", temp_temperature, temp_humidity);
        ESP_LOGI(GATTC_TAG, "2 temperature=%d | humidity=%d ", temperature, humidity);
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_WRITE_DESCR_EVT");
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write descr success ");
        uint8_t write_char_data[35];
        for (int i = 0; i < sizeof(write_char_data); ++i)
        {
            write_char_data[i] = i % 256;
        }
        esp_ble_gattc_write_char( gattc_if,
                                  gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                  gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                  sizeof(write_char_data),
                                  write_char_data,
                                  ESP_GATT_WRITE_TYPE_RSP,
                                  ESP_GATT_AUTH_REQ_NONE);
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:");
        esp_log_buffer_hex(GATTC_TAG, bda, sizeof(esp_bd_addr_t));
        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_WRITE_CHAR_EVT");
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write char success ");
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        bt_connect = false;
        get_server = false;
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_DISCONNECT_EVT, reason = %d", p_data->disconnect.reason);
        break;
    default:
        ESP_LOGI(GATTC_TAG, "default");
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT");
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_SCAN_START_COMPLETE_EVT");
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x", param->scan_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "scan start success");

        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_SCAN_RESULT_EVT");
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_INQ_RES_EVT");
            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
            ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
            esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
            ESP_LOGI(GATTC_TAG, "\n");
            if (adv_name != NULL) {
                if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                    ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
                    if (bt_connect == false) {
                        bt_connect = true;
                        ESP_LOGI(GATTC_TAG, "connect to the remote device.");
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                    }
                }
            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_INQ_CMPL_EVT");
            break;
        case ESP_GAP_SEARCH_DISC_RES_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_DISC_RES_EVT");
            break;
        case ESP_GAP_SEARCH_DISC_BLE_RES_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_DISC_BLE_RES_EVT");
            break;
        case ESP_GAP_SEARCH_DISC_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_DISC_CMPL_EVT");
            break;
        case ESP_GAP_SEARCH_DI_DISC_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_DI_DISC_CMPL_EVT");
            break;
        case ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT");
            break;
        default:
            ESP_LOGI(GATTC_TAG, "default");
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT");
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "stop scan successfully");
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT");
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "adv stop failed, error status = %x", param->adv_stop_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "stop adv successfully");
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                    param->update_conn_params.status,
                    param->update_conn_params.min_int,
                    param->update_conn_params.max_int,
                    param->update_conn_params.conn_int,
                    param->update_conn_params.latency,
                    param->update_conn_params.timeout);
        break;
    default:
        ESP_LOGI(GATTC_TAG, "default");
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

esp_err_t bluetooth_initialize(void){

    esp_err_t ret = ESP_OK;
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s initialize controller failed, error code = %x\n", __func__, ret);
        return ret;
    }

/*    There are four Bluetooth modes supported:

    ESP_BT_MODE_IDLE: Bluetooth not running
    ESP_BT_MODE_BLE: BLE mode
    ESP_BT_MODE_CLASSIC_BT: BT Classic mode
    ESP_BT_MODE_BTDM: Dual mode (BLE + BT Classic)*/

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed, error code = %x\n", __func__, ret);
        return ret;
    }

    //register the  callback function to the gap module
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gap register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        ESP_LOGE(GATTC_TAG, "%s gattc register failed, error code = %x\n", __func__, ret);
        return ret;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register failed, error code = %x\n", __func__, ret);
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return ret;
}

