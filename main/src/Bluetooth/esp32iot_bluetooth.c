#include "esp32iot_bluetooth.h"

void print_properities(esp_gatt_char_prop_t property){

    if(property & ESP_GATT_CHAR_PROP_BIT_BROADCAST)
        printf("| ESP_GATT_CHAR_PROP_BIT_BROADCAST |");
    if(property & ESP_GATT_CHAR_PROP_BIT_READ)
        printf("| ESP_GATT_CHAR_PROP_BIT_READ |");
    if(property & ESP_GATT_CHAR_PROP_BIT_WRITE_NR)
        printf("| ESP_GATT_CHAR_PROP_BIT_WRITE_NR |");
    if(property & ESP_GATT_CHAR_PROP_BIT_WRITE)
        printf("| ESP_GATT_CHAR_PROP_BIT_WRITE |");
    if(property & ESP_GATT_CHAR_PROP_BIT_NOTIFY)
        printf("| ESP_GATT_CHAR_PROP_BIT_NOTIFY |");
    if(property & ESP_GATT_CHAR_PROP_BIT_INDICATE)
        printf("| ESP_GATT_CHAR_PROP_BIT_INDICATE |");
    if(property & ESP_GATT_CHAR_PROP_BIT_AUTH)
        printf("| ESP_GATT_CHAR_PROP_BIT_AUTH |");
    if(property & ESP_GATT_CHAR_PROP_BIT_EXT_PROP)
        printf("| ESP_GATT_CHAR_PROP_BIT_EXT_PROP |");

    printf("\n");
}

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

static void start_scan(void)
{
    stop_scan_done = false;
    Isconnecting = false;
    uint32_t duration = 30;
    esp_ble_gap_start_scanning(duration);
}

static void gattc_profile_BA5C_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{

    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_REG_EVT");

        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    /* one device connect successfully, all profiles callback function will get the ESP_GATTC_CONNECT_EVT,
     so must compare the mac address to check which device is connected, so it is a good choice to use ESP_GATTC_OPEN_EVT. */
    case ESP_GATTC_CONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_CONNECT_EVT");

        break;
    case ESP_GATTC_OPEN_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_OPEN_EVT");

        if (p_data->open.status != ESP_GATT_OK){
            //open failed, ignore the first device, connect the second device
            ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
            conn_device_BA5C = false;
            //start_scan();
            break;
        }
        memcpy(gl_profile_tab[PROFILE_BA5C_APP_ID].remote_bda, p_data->open.remote_bda, 6);
        gl_profile_tab[PROFILE_BA5C_APP_ID].conn_id = p_data->open.conn_id;
        //gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id = p_data->open.conn_id;
        ESP_LOGI(GATTC_TAG, "conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret_BA5C = esp_ble_gattc_send_mtu_req (gattc_if, p_data->open.conn_id);
        if (mtu_ret_BA5C){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret_BA5C);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_CFG_MTU_EVT");

        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"Config mtu failed");
        }
        ESP_LOGI(GATTC_TAG, "status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_HTU21D_service_uuid);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_MS5637_service_uuid);
        //esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_Battery_service_uuid);
        
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_SEARCH_RES_EVT");

        bool found_any = false;
        esp_gatt_srvc_id_t *srvc_id = (esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
        ESP_LOGI(GATTC_TAG, "conn_id = %x", p_data->search_res.conn_id);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_HTU21D_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "BA5C HTU21D service found");
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            get_service_BA5C = true;
            get_service_MS5637 = false;

            gl_profile_tab[PROFILE_BA5C_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_BA5C_APP_ID].service_end_handle = p_data->search_res.end_handle; 
        }else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_MS5637_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "BA5C MS5637 service found");
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            get_service_BA5C = false;
            get_service_MS5637 = true;

            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle = p_data->search_res.end_handle; 
        }
        // if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_Battery_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
        //     ESP_LOGI(GATTC_TAG, "BA5C Battery service found");
        //     ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
        // }
       
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_SEARCH_CMPL_EVT");
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        //ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, p_data->search_cmpl->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);

        uint16_t count = 0;
        uint16_t tmp_count = 0;

        if (get_service_BA5C){
            ESP_LOGI(GATTC_TAG, "get_service_BA5C");

            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_BA5C_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0) {
                tmp_count = count;
                char_elem_result_BA5C = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result_BA5C){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else {


                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                        p_data->search_cmpl.conn_id,
                                                        gl_profile_tab[PROFILE_BA5C_APP_ID].service_start_handle,
                                                        gl_profile_tab[PROFILE_BA5C_APP_ID].service_end_handle,
                                                        BA5C_HTU21D_data_char_uuid,
                                                        char_elem_result_BA5C,
                                                        &count) != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }else{
                        /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                        if (count > 0 && (char_elem_result_BA5C[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                            gl_profile_tab[PROFILE_BA5C_APP_ID].char_handle = char_elem_result_BA5C[0].char_handle;
                            esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_BA5C_APP_ID].remote_bda, char_elem_result_BA5C[0].char_handle);
                        }
                    }
                    count = tmp_count;

                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                        p_data->search_cmpl.conn_id,
                                                        gl_profile_tab[PROFILE_BA5C_APP_ID].service_start_handle,
                                                        gl_profile_tab[PROFILE_BA5C_APP_ID].service_end_handle,
                                                        BA5C_HTU21D_status_char_uuid,
                                                        char_elem_result_BA5C,
                                                        &count) != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }else{  
                        /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                        if (count > 0 && (char_elem_result_BA5C[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                            gl_profile_tab[PROFILE_BA5C_APP_ID].status_char_handle = char_elem_result_BA5C[0].char_handle;
                        }
                    }
                    count = tmp_count;

                }
                /* free char_elem_result */
                free(char_elem_result_BA5C);
            }else {
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        if (get_service_MS5637){
            ESP_LOGI(GATTC_TAG, "get_service_MS5637");

            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0) {
                char_elem_result_BA5C = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result_BA5C){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else {
                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                        p_data->search_cmpl.conn_id,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                        BA5C_MS5637_data_char_uuid,
                                                        char_elem_result_BA5C,
                                                        &count) != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }else{
                        /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                        if (count > 0 && (char_elem_result_BA5C[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle = char_elem_result_BA5C[0].char_handle;
                            esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].remote_bda, char_elem_result_BA5C[0].char_handle);
                        }
                    }
                    count = tmp_count;

                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                        p_data->search_cmpl.conn_id,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                        BA5C_MS5637_calibration_char_uuid,
                                                        char_elem_result_BA5C,
                                                        &count) != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }else{
                        /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                        if (count > 0 && (char_elem_result_BA5C[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].calibration_char_handle = char_elem_result_BA5C[0].char_handle;
                        }
                    }
                    count = tmp_count;

                    if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                        p_data->search_cmpl.conn_id,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                        BA5C_MS5637_status_char_uuid,
                                                        char_elem_result_BA5C,
                                                        &count) != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }else{
                        /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                        if (count > 0 && (char_elem_result_BA5C[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].status_char_handle = char_elem_result_BA5C[0].char_handle;
                        }
                    }
                    count = tmp_count;

                }
                /* free char_elem_result */
                free(char_elem_result_BA5C);
            }else {
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_REG_FOR_NOTIFY_EVT");

        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
            break;
        }

        uint16_t count = 0;
        uint16_t notify_en = 1;

        if(p_data->reg_for_notify.handle == gl_profile_tab[PROFILE_BA5C_APP_ID].char_handle){
            ESP_LOGI(GATTC_TAG, "PROFILE_BA5C char_handle: %d", p_data->reg_for_notify.handle);

            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_BA5C_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_BA5C_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_BA5C_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_BA5C_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result_BA5C = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result_BA5C){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_BA5C_APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         BA5C_notify_descr_char_uuid,
                                                                         descr_elem_result_BA5C,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result_BA5C[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_BA5C[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_APP_ID].conn_id,
                                                                     descr_elem_result_BA5C[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result_BA5C);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }
        }

        if(p_data->reg_for_notify.handle == gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle){
            ESP_LOGI(GATTC_TAG, "PROFILE_BA5C_MS5637 char_handle: %d", p_data->reg_for_notify.handle);

            esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                         ESP_GATT_DB_DESCRIPTOR,
                                                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle,
                                                                         &count);
            if (ret_status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0){
                descr_elem_result_BA5C = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);
                if (!descr_elem_result_BA5C){
                    ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
                }else{
                    ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                         p_data->reg_for_notify.handle,
                                                                         BA5C_notify_descr_char_uuid,
                                                                         descr_elem_result_BA5C,
                                                                         &count);
                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                    }

                    /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                    if (count > 0 && descr_elem_result_BA5C[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_BA5C[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                        ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                     descr_elem_result_BA5C[0].handle,
                                                                     sizeof(notify_en),
                                                                     (uint8_t *)&notify_en,
                                                                     ESP_GATT_WRITE_TYPE_RSP,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                    }

                    if (ret_status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                    }

                    /* free descr_elem_result */
                    free(descr_elem_result_BA5C);
                }
            }
            else{
                ESP_LOGE(GATTC_TAG, "decsr not found");
            }
        }

        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_NOTIFY_EVT\n");
        if(p_data->notify.handle == gl_profile_tab[PROFILE_BA5C_APP_ID].char_handle){
            //ESP_LOGI(GATTC_TAG, "PROFILE_BA5C char_handle: %d", p_data->notify.handle);
            ESP_LOGI(GATTC_TAG, "Receive HTU21D data notify value:");

            esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

            BA5C_HTU21D_temperature = (p_data->notify.value[1]) | (p_data->notify.value[0] << 8);
            BA5C_HTU21D_humidity = (p_data->notify.value[4]) | (p_data->notify.value[3] << 8);

            BA5C_HTU21D_temperature=-46.85 + 175.72 * BA5C_HTU21D_temperature/65536; //(°C) = -46.85 + 175.72 x Temperature Word / 2 16
            BA5C_HTU21D_humidity=-6 + 125 * BA5C_HTU21D_humidity/65536; //(%RH) = -6 + 125 x Humidity Word / 2 16
            ESP_LOGI(GATTC_TAG, "BA5C HTU21D: temperature=%d | humidity=%d", BA5C_HTU21D_temperature, BA5C_HTU21D_humidity);

            if (esp_ble_gattc_read_char( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_APP_ID].status_char_handle,
                                         ESP_GATT_AUTH_REQ_NONE) != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "BA5C HTU21D status: esp_ble_gattc_get_descr_by_char_handle error");
            }
        }   

        if(p_data->notify.handle == gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle){
            //ESP_LOGI(GATTC_TAG, "BA5C_MS5637 char_handle: %d", p_data->notify.handle);
            ESP_LOGI(GATTC_TAG, "Receive MS5637 data notify value:");

            esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

            if (esp_ble_gattc_read_char( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].calibration_char_handle,
                                         ESP_GATT_AUTH_REQ_NONE) != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "BA5C MS5637 calibration: esp_ble_gattc_get_descr_by_char_handle error");
            }

            if (esp_ble_gattc_read_char( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].status_char_handle,
                                         ESP_GATT_AUTH_REQ_NONE) != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "BA5C MS5637 status: esp_ble_gattc_get_descr_by_char_handle error");
            }
        }   
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_READ_CHAR_EVT");

        if (p_data->read.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "read characteristics failed, error status = %x", p_data->read.status);
            break;
        }

        if(p_data->read.handle == gl_profile_tab[PROFILE_BA5C_APP_ID].status_char_handle){
            //ESP_LOGI(GATTC_TAG, "PROFILE_BA5C char_handle: %d", p_data->read.handle);
            ESP_LOGI(GATTC_TAG, "Read HTU21D status value:");

            esp_log_buffer_hex(GATTC_TAG, p_data->read.value, p_data->read.value_len);
            BA5C_HTU21D_status=p_data->read.value[0]<<8;
            ESP_LOGI(GATTC_TAG, "BA5C_HTU21D_status=%d", BA5C_HTU21D_status);
        }   

        if(p_data->read.handle == gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].calibration_char_handle){
            //ESP_LOGI(GATTC_TAG, "PROFILE_BA5C_MS5637 char_handle: %d", p_data->read.handle);
            ESP_LOGI(GATTC_TAG, "Read MS5637 calibration value:");

            esp_log_buffer_hex(GATTC_TAG, p_data->read.value, p_data->read.value_len);
        }

        if(p_data->read.handle == gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].status_char_handle){
            //ESP_LOGI(GATTC_TAG, "PROFILE_BA5C_MS5637 char_handle: %d", p_data->read.handle);
            ESP_LOGI(GATTC_TAG, "Read MS5637 status value:");

            esp_log_buffer_hex(GATTC_TAG, p_data->read.value, p_data->read.value_len);

            ESP_LOGI(GATTC_TAG, "");
        } 

        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_WRITE_DESCR_EVT");

        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write descr success");
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_WRITE_CHAR_EVT");

        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
        }else{
            ESP_LOGI(GATTC_TAG, "write char success");
        }
        start_scan();
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_SRVC_CHG_EVT");

        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                 (bda[4] << 8) + bda[5]);
        break;
    }
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C ESP_GATTC_DISCONNECT_EVT");

        //Start scanning again
        start_scan();
        if (memcmp(p_data->disconnect.remote_bda, gl_profile_tab[PROFILE_BA5C_APP_ID].remote_bda, 6) == 0){
            ESP_LOGI(GATTC_TAG, "device a disconnect");
            conn_device_BA5C = false;
            get_service_BA5C = false;

            get_service_MS5637 = false;
        }
        break;
    default:
        ESP_LOGI(GATTC_TAG, "BA5C unhandled event: %d", event);

        break;
    }
}

static void gattc_profile_BA5C_HTU21D_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{

    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "REG_EVT");
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    /* one device connect successfully, all profiles callback function will get the ESP_GATTC_CONNECT_EVT,
     so must compare the mac address to check which device is connected, so it is a good choice to use ESP_GATTC_OPEN_EVT. */
    case ESP_GATTC_CONNECT_EVT:
        break;
    case ESP_GATTC_OPEN_EVT:
        if (p_data->open.status != ESP_GATT_OK){
            //open failed, ignore the first device, connect the second device
            ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
            conn_device_HTU21D = false;
            //start_scan();
            break;
        }
        memcpy(gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].remote_bda, p_data->open.remote_bda, 6);
        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id = p_data->open.conn_id;
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->open.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"Config mtu failed");
        }
        ESP_LOGI(GATTC_TAG, "Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_HTU21D_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        esp_gatt_srvc_id_t *srvc_id = (esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
        ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x", p_data->search_res.conn_id);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_HTU21D_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "HTU21D service found");
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            get_service_HTU21D = true;
            gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle = p_data->search_res.end_handle;
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        if (get_service_HTU21D){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }
            if (count > 0) {
                char_elem_result_BA5C_HTU21D = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result_BA5C_HTU21D){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else {
                    status = esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                                             BA5C_HTU21D_data_char_uuid,
                                                             char_elem_result_BA5C_HTU21D,
                                                             &count);
                    if (status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }

                    /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                    if (count > 0 && (char_elem_result_BA5C_HTU21D[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle = char_elem_result_BA5C_HTU21D[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].remote_bda, char_elem_result_BA5C_HTU21D[0].char_handle);
                    }
                }
                /* free char_elem_result */
                free(char_elem_result_BA5C_HTU21D);
            }else {
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
            break;
        }
        uint16_t count = 0;
        uint16_t notify_en = 1;
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                     ESP_GATT_DB_DESCRIPTOR,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle,
                                                                     &count);
        if (ret_status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
        }
        if (count > 0){
            descr_elem_result_BA5C_HTU21D = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result_BA5C_HTU21D){
                ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
            }else{
                ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                     p_data->reg_for_notify.handle,
                                                                     BA5C_notify_descr_char_uuid,
                                                                     descr_elem_result_BA5C_HTU21D,
                                                                     &count);
                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                }

                /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                if (count > 0 && descr_elem_result_BA5C_HTU21D[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_BA5C_HTU21D[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                    ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                 gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                 descr_elem_result_BA5C_HTU21D[0].handle,
                                                                 sizeof(notify_en),
                                                                 (uint8_t *)&notify_en,
                                                                 ESP_GATT_WRITE_TYPE_RSP,
                                                                 ESP_GATT_AUTH_REQ_NONE);
                }

                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                }

                /* free descr_elem_result */
                free(descr_elem_result_BA5C_HTU21D);
            }
        }
        else{
            ESP_LOGE(GATTC_TAG, "decsr not found");
        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, Receive HTU21D notify value:");
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

        BA5C_HTU21D_temperature = (p_data->notify.value[1]) | (p_data->notify.value[0] << 8);
        BA5C_HTU21D_humidity = (p_data->notify.value[4]) | (p_data->notify.value[3] << 8);

        BA5C_HTU21D_temperature=-46.85 + 175.72 * BA5C_HTU21D_temperature/65536; //(°C) = -46.85 + 175.72 x Temperature Word / 2 16
        BA5C_HTU21D_humidity=-6 + 125 * BA5C_HTU21D_humidity/65536; //(%RH) = -6 + 125 x Humidity Word / 2 16
        ESP_LOGI(GATTC_TAG, "BA5C HTU21D: temperature=%d | humidity=%d", BA5C_HTU21D_temperature, BA5C_HTU21D_humidity);

        uint16_t count = 1;
        if (count > 0){
            char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
            if (!char_elem_result){
                ESP_LOGE(GATTC_TAG, "gattc no mem");
            }else{
                if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                         BA5C_HTU21D_status_char_uuid,
                                         char_elem_result,
                                         &count) == ESP_GATT_OK){

                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle = char_elem_result[0].char_handle;
                        //ESP_LOGI(GATTC_TAG, "strlen(char_elem_result): %d, char_elem_result->properties: %d",strlen(char_elem_result), char_elem_result->properties);
                        //esp_log_buffer_hex(GATTC_TAG, char_elem_result->uuid.uuid.uuid128, ESP_UUID_LEN_128);

                        esp_gatt_status_t ret_status = esp_ble_gattc_read_char( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                     char_elem_result->char_handle,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                        if (ret_status != ESP_GATT_OK){
                            ESP_LOGE(GATTC_TAG, "BA5C HTU21D status: esp_ble_gattc_get_descr_by_char_handle error");
                        }
                    }
                }else{
                    ESP_LOGE(GATTC_TAG, "HTU21D status characteristics not found");
                } 
            }
            /* free char_elem_result */
            free(char_elem_result);
        }
        break;
        case ESP_GATTC_READ_CHAR_EVT:
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_READ_CHAR_EVT for BA5C HTU21D");
            if (p_data->read.status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "read characteristics failed, error status = %x", p_data->read.status);
                break;
            }
            //esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

            BA5C_HTU21D_status=p_data->read.value[0]<<8;
            ESP_LOGI(GATTC_TAG, "BA5C_HTU21D_status=%d", BA5C_HTU21D_status);
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write descr success");
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
        }else{
            ESP_LOGI(GATTC_TAG, "write char success");
        }
        start_scan();
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                 (bda[4] << 8) + bda[5]);
        break;
    }
    case ESP_GATTC_DISCONNECT_EVT:
        //Start scanning again
        start_scan();
        if (memcmp(p_data->disconnect.remote_bda, gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].remote_bda, 6) == 0){
            ESP_LOGI(GATTC_TAG, "device a disconnect");
            conn_device_HTU21D = false;
            get_service_HTU21D = false;
        }
        break;
    default:
        break;
    }
}

static void gattc_profile_BA5C_MS5637_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_REG_EVT");

        break;
    case ESP_GATTC_CONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_CONNECT_EVT");

        ESP_LOGI(GATTC_TAG, "conn_id %d, if %d", p_data->connect.conn_id, gattc_if);

        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].remote_bda, sizeof(esp_bd_addr_t));
/*        mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->connect.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }*/

        break;
    case ESP_GATTC_OPEN_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_OPEN_EVT");

        /*if (p_data->open.status != ESP_GATT_OK){
            //open failed, ignore the second device, connect the third device
            ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
            conn_device_MS5637 = false;
            //start_scan();
            break;
        }
        memcpy(gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].remote_bda, p_data->open.remote_bda, 6);
        gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id = p_data->open.conn_id;
        ESP_LOGI(GATTC_TAG, "conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->open.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }*/
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_CFG_MTU_EVT");

        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"config mtu failed");
        }
        ESP_LOGI(GATTC_TAG, "ctatus %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_MS5637_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_SEARCH_RES_EVT");
        esp_gatt_srvc_id_t *srvc_id = (esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
        ESP_LOGI(GATTC_TAG, "conn_id = %x", p_data->search_res.conn_id);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_MS5637_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "MS5637 service found");
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            get_service_MS5637 = true;
            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle = p_data->search_res.end_handle;
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_SEARCH_CMPL_EVT");

        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        if (get_service_MS5637){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }

            if (count > 0){
                char_elem_result_BA5C_MS5637 = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result_BA5C_MS5637){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else{
                    status = esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                             BA5C_MS5637_data_char_uuid,
                                                             char_elem_result_BA5C_MS5637,
                                                             &count);
                    if (status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }

                    /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle = char_elem_result[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].remote_bda, char_elem_result[0].char_handle);
                    }
                }
                /* free char_elem_result */
                free(char_elem_result_BA5C_MS5637);
            }else{
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_REG_FOR_NOTIFY_EVT");

        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
            break;
        }
        uint16_t count = 0;
        uint16_t notify_en = 1;
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                     ESP_GATT_DB_DESCRIPTOR,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].service_end_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle,
                                                                     &count);
        if (ret_status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
        }
        if (count > 0){
            descr_elem_result_BA5C_MS5637 = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result_BA5C_MS5637){
                ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
            }else{
                ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                     p_data->reg_for_notify.handle,
                                                                     BA5C_notify_descr_char_uuid,
                                                                     descr_elem_result_BA5C_MS5637,
                                                                     &count);
                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                }

                /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                if (count > 0 && descr_elem_result_BA5C_MS5637[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_BA5C_MS5637[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                    ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                 gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                                                 descr_elem_result_BA5C_MS5637[0].handle,
                                                                 sizeof(notify_en),
                                                                 (uint8_t *)&notify_en,
                                                                 ESP_GATT_WRITE_TYPE_RSP,
                                                                 ESP_GATT_AUTH_REQ_NONE);
                }

                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                }

                /* free descr_elem_result */
                free(descr_elem_result_BA5C_MS5637);
            }
        }
        else{
            ESP_LOGE(GATTC_TAG, "decsr not found");
        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_NOTIFY_EVT");

        ESP_LOGI(GATTC_TAG, "receive notify value:");
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

        uint16_t count = 1;
        if (count > 0){
            char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
            if (!char_elem_result){
                ESP_LOGE(GATTC_TAG, "gattc no mem");
            }else{
                if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                         BA5C_MS5637_calibration_char_uuid,
                                         char_elem_result,
                                         &count) == ESP_GATT_OK){

                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle = char_elem_result[0].char_handle;
                        //ESP_LOGI(GATTC_TAG, "strlen(char_elem_result): %d, char_elem_result->properties: %d",strlen(char_elem_result), char_elem_result->properties);
                        //esp_log_buffer_hex(GATTC_TAG, char_elem_result->uuid.uuid.uuid128, ESP_UUID_LEN_128);

                        esp_gatt_status_t ret_status = esp_ble_gattc_read_char( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                     char_elem_result->char_handle,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                        if (ret_status != ESP_GATT_OK){
                            ESP_LOGE(GATTC_TAG, "BA5C MS5637 calibration: esp_ble_gattc_get_descr_by_char_handle error");
                        }
                    }
                }else{
                    ESP_LOGE(GATTC_TAG, "BA5C MS5637 calibration characteristics not found");
                } 

                if (esp_ble_gattc_get_char_by_uuid( gattc_if,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_start_handle,
                                         gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].service_end_handle,
                                         BA5C_MS5637_status_char_uuid,
                                         char_elem_result,
                                         &count) == ESP_GATT_OK){

                    if (count > 0 && (char_elem_result[0].properties & ESP_GATT_CHAR_PROP_BIT_READ)){
                        gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].char_handle = char_elem_result[0].char_handle;
                        //ESP_LOGI(GATTC_TAG, "strlen(char_elem_result): %d, char_elem_result->properties: %d",strlen(char_elem_result), char_elem_result->properties);
                        //esp_log_buffer_hex(GATTC_TAG, char_elem_result->uuid.uuid.uuid128, ESP_UUID_LEN_128);

                        esp_gatt_status_t ret_status = esp_ble_gattc_read_char( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_HTU21D_APP_ID].conn_id,
                                                                     char_elem_result->char_handle,
                                                                     ESP_GATT_AUTH_REQ_NONE);
                        if (ret_status != ESP_GATT_OK){
                            ESP_LOGE(GATTC_TAG, "BA5C MS5637 status: esp_ble_gattc_get_descr_by_char_handle error");
                        }
                    }
                }else{
                    ESP_LOGE(GATTC_TAG, "BA5C MS5637 status characteristics not found");
                }
            }
            /* free char_elem_result */
            free(char_elem_result);
        }
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_READ_CHAR_EVT");

        if (p_data->read.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "read characteristics failed, error status = %x", p_data->read.status);
            break;
        }
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
    case ESP_GATTC_WRITE_DESCR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_WRITE_DESCR_EVT");

        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write descr success");
        /*uint8_t write_char_data[35];
        for (int i = 0; i < sizeof(write_char_data); ++i)
        {
            write_char_data[i] = i % 256;
        }
        esp_ble_gattc_write_char( gattc_if,
                                  gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].conn_id,
                                  gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].char_handle,
                                  sizeof(write_char_data),
                                  write_char_data,
                                  ESP_GATT_WRITE_TYPE_RSP,
                                  ESP_GATT_AUTH_REQ_NONE);*/
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_WRITE_CHAR_EVT");

        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write char failed, error status = %x", p_data->write.status);
        }else{
            ESP_LOGI(GATTC_TAG, "write char success");
        }
        start_scan();
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_SRVC_CHG_EVT");

        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                 (bda[4] << 8) + bda[5]);
        break;
    }
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 ESP_GATTC_DISCONNECT_EVT");

        if (memcmp(p_data->disconnect.remote_bda, gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].remote_bda, 6) == 0){
            ESP_LOGI(GATTC_TAG, "device b disconnect");
            conn_device_MS5637 = false;
            get_service_MS5637 = false;
        }
        break;
    default:
        ESP_LOGI(GATTC_TAG, "BA5C MS5637 unhandled event: %d", event);

        break;
    }
}

static void gattc_profile_BA5C_Battery_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "REG_EVT");
        break;
    case ESP_GATTC_CONNECT_EVT:
        break;
    case ESP_GATTC_OPEN_EVT:
        if (p_data->open.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "connect device failed, status %d", p_data->open.status);
            conn_device_Battery = false;
            //start_scan();
            break;
        }
        memcpy(gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].remote_bda, p_data->open.remote_bda, 6);
        gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].conn_id = p_data->open.conn_id;
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d, mtu %d", p_data->open.conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
        esp_err_t mtu_ret = esp_ble_gattc_send_mtu_req (gattc_if, p_data->open.conn_id);
        if (mtu_ret){
            ESP_LOGE(GATTC_TAG, "config MTU error, error code = %x", mtu_ret);
        }
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG,"Config mtu failed");
        }
        ESP_LOGI(GATTC_TAG, "Status %d, MTU %d, conn_id %d", param->cfg_mtu.status, param->cfg_mtu.mtu, param->cfg_mtu.conn_id);
        esp_ble_gattc_search_service(gattc_if, param->cfg_mtu.conn_id, &BA5C_Battery_service_uuid);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        esp_gatt_srvc_id_t *srvc_id = (esp_gatt_srvc_id_t *)&p_data->search_res.srvc_id;
        ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x", p_data->search_res.conn_id);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_128 && !memcmp(srvc_id->id.uuid.uuid.uuid128, BA5C_Battery_service_uuid.uuid.uuid128, ESP_UUID_LEN_128)) {
            ESP_LOGI(GATTC_TAG, "BA5C Battery service found");
            ESP_LOG_BUFFER_HEX_LEVEL(GATTC_TAG, srvc_id->id.uuid.uuid.uuid128, ESP_UUID_LEN_128, ESP_LOG_INFO);
            get_service_Battery = true;
            gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_end_handle = p_data->search_res.end_handle;
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "search service failed, error status = %x", p_data->search_cmpl.status);
            break;
        }
        if (get_service_Battery){
            uint16_t count = 0;
            esp_gatt_status_t status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     p_data->search_cmpl.conn_id,
                                                                     ESP_GATT_DB_CHARACTERISTIC,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_end_handle,
                                                                     INVALID_HANDLE,
                                                                     &count);
            if (status != ESP_GATT_OK){
                ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
            }

            if (count > 0){
                char_elem_result_BA5C_Battery = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (!char_elem_result_BA5C_Battery){
                    ESP_LOGE(GATTC_TAG, "gattc no mem");
                }else{
                    status = esp_ble_gattc_get_char_by_uuid( gattc_if,
                                                             p_data->search_cmpl.conn_id,
                                                             gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_start_handle,
                                                             gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_end_handle,
                                                             BA5C_Battery_data_char_uuid,
                                                             char_elem_result_BA5C_Battery,
                                                             &count);
                    if (status != ESP_GATT_OK){
                        ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_char_by_uuid error");
                    }

                    /*  Every service have only one char in our 'ESP_GATTS_DEMO' demo, so we used first 'char_elem_result' */
                    if (count > 0 && (char_elem_result_BA5C_Battery[0].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)){
                        gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].char_handle = char_elem_result_BA5C_Battery[0].char_handle;
                        esp_ble_gattc_register_for_notify (gattc_if, gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].remote_bda, char_elem_result_BA5C_Battery[0].char_handle);
                    }
                }
                /* free char_elem_result */
                free(char_elem_result_BA5C_Battery);
            }else{
                ESP_LOGE(GATTC_TAG, "no char found");
            }
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "reg notify failed, error status =%x", p_data->reg_for_notify.status);
            break;
        }
        uint16_t count = 0;
        uint16_t notify_en = 1;
        esp_gatt_status_t ret_status = esp_ble_gattc_get_attr_count( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].conn_id,
                                                                     ESP_GATT_DB_DESCRIPTOR,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_start_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].service_end_handle,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].char_handle,
                                                                     &count);
        if (ret_status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_attr_count error");
        }
        if (count > 0){
            descr_elem_result_BA5C_Battery = (esp_gattc_descr_elem_t *)malloc(sizeof(esp_gattc_descr_elem_t) * count);
            if (!descr_elem_result_BA5C_Battery){
                ESP_LOGE(GATTC_TAG, "malloc error, gattc no mem");
            }else{
                ret_status = esp_ble_gattc_get_descr_by_char_handle( gattc_if,
                                                                     gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].conn_id,
                                                                     p_data->reg_for_notify.handle,
                                                                     BA5C_notify_descr_char_uuid,
                                                                     descr_elem_result_BA5C_Battery,
                                                                     &count);
                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_get_descr_by_char_handle error");
                }

                /* Erery char have only one descriptor in our 'ESP_GATTS_DEMO' demo, so we used first 'descr_elem_result' */
                if (count > 0 && descr_elem_result_BA5C_Battery[0].uuid.len == ESP_UUID_LEN_16 && descr_elem_result_BA5C_Battery[0].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG){
                    ret_status = esp_ble_gattc_write_char_descr( gattc_if,
                                                                 gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].conn_id,
                                                                 descr_elem_result_BA5C_Battery[0].handle,
                                                                 sizeof(notify_en),
                                                                 (uint8_t *)&notify_en,
                                                                 ESP_GATT_WRITE_TYPE_RSP,
                                                                 ESP_GATT_AUTH_REQ_NONE);
                }

                if (ret_status != ESP_GATT_OK){
                    ESP_LOGE(GATTC_TAG, "esp_ble_gattc_write_char_descr error");
                }

                /* free descr_elem_result */
                free(descr_elem_result_BA5C_Battery);
            }
        }
        else{
            ESP_LOGE(GATTC_TAG, "decsr not found");
        }
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_NOTIFY_EVT, Receive BA5C Battery notify value:");
        esp_log_buffer_hex(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);

        BA5C_Battery_level = (p_data->notify.value[0] << 8);
        BA5C_Battery_status = (p_data->notify.value[1]);

        ESP_LOGI(GATTC_TAG, "BA5C Battery: level=%d | status=%d ", BA5C_Battery_level, BA5C_Battery_status);
        
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "write descr failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "write descr success");
/*        uint8_t write_char_data[35];
        for (int i = 0; i < sizeof(write_char_data); ++i)
        {
            write_char_data[i] = i % 256;
        }
        esp_ble_gattc_write_char( gattc_if,
                                  gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].conn_id,
                                  gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].char_handle,
                                  sizeof(write_char_data),
                                  write_char_data,
                                  ESP_GATT_WRITE_TYPE_RSP,
                                  ESP_GATT_AUTH_REQ_NONE);*/
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (p_data->write.status != ESP_GATT_OK){
            ESP_LOGE(GATTC_TAG, "Write char failed, error status = %x", p_data->write.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Write char success");
        start_scan();
        break;
    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_SRVC_CHG_EVT, bd_addr:%08x%04x",(bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                 (bda[4] << 8) + bda[5]);
        break;
    }
    case ESP_GATTC_DISCONNECT_EVT:
        if (memcmp(p_data->disconnect.remote_bda, gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].remote_bda, 6) == 0){
            ESP_LOGI(GATTC_TAG, "device c disconnect");
            conn_device_Battery = false;
            get_service_Battery = false;
        }
        break;
    default:
        break;
    }
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    switch (event) {
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(GATTC_TAG, "update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(GATTC_TAG, "Scan start success");
        }else{
            ESP_LOGE(GATTC_TAG, "Scan start failed");
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
            ESP_LOGI(GATTC_TAG, "Searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            ESP_LOGI(GATTC_TAG, "Searched Device Name Len %d\n", adv_name_len);
            esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);

            if (Isconnecting){
                break;
            }
            if (conn_device_BA5C && !stop_scan_done){
                stop_scan_done = true;
                esp_ble_gap_stop_scanning();
                ESP_LOGI(GATTC_TAG, "all devices are connected");
                break;
            }
            if (adv_name != NULL) {

                if (strlen(remote_device_name[0]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[0], adv_name_len) == 0) {
                    if (conn_device_BA5C == false) {
                        conn_device_BA5C = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[0]);
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_BA5C_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                        Isconnecting = true;
                    }
                    break;
                }
                /*else if (strlen(remote_device_name[1]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[1], adv_name_len) == 0) {
                    if (conn_device_MS5637 == false) {
                        conn_device_MS5637 = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[1]);
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_BA5C_MS5637_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                        Isconnecting = true;

                    }
                }
                else if (strlen(remote_device_name[2]) == adv_name_len && strncmp((char *)adv_name, remote_device_name[2], adv_name_len) == 0) {
                    if (conn_device_Battery == false) {
                        conn_device_Battery = true;
                        ESP_LOGI(GATTC_TAG, "Searched device %s", remote_device_name[2]);
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(gl_profile_tab[PROFILE_BA5C_Battery_APP_ID].gattc_if, scan_result->scan_rst.bda, true);
                        Isconnecting = true;
                    }
                    break;
                }*/

            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Scan stop failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Stop scan successfully");

        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Adv stop failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Stop adv successfully");
        break;

    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    //ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d, app_id %d", event, gattc_if, param->reg.app_id);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d",
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

    ret = esp_ble_gattc_app_register(PROFILE_BA5C_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_BA5C_MS5637_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
        return;
    }


/*    ret = esp_ble_gattc_app_register(PROFILE_BA5C_HTU21D_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_BA5C_MS5637_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_BA5C_Battery_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "gattc app register error, error code = %x", ret);
        return;
    }*/

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(200);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return ret;
}

