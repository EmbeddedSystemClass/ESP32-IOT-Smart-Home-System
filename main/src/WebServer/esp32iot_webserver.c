#include "esp32iot_webserver.h"


esp_err_t webserver_initialize(void){
	esp_err_t err;

	err = wifi_initialize();
	if(err != ESP_OK){
		ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}

/*	err = wifi_sta_start("hotosk", "W6game555");
	if(err != ESP_OK){
		ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}*/

    err = wifi_ap_create();
    if(err != ESP_OK){
        ESP_LOGW(webserver_tag, "%s", wifi_err_to_string(err));
        ESP_ERROR_CHECK( err );
    }

/*	err = wifi_scan_initialize();
	if(err != ESP_OK){
		ESP_LOGW(webserver_webserver_tag, "%s", wifi_err_to_string(err));
		ESP_ERROR_CHECK( err );
	}*/

	//char ssid[] = "hotosk";
    //char* password;
	//save_last_connected_wifi("asda", "asdasd");
/*	wifi_manager_state = 0;
    err = get_last_connected_wifi(&actual_wifi.ssid, &actual_wifi.password);
    if (err != ESP_OK){
        ESP_LOGW(webserver_webserver_tag, "( %d )", err);
        ESP_ERROR_CHECK( err );
    }else{
    	if(actual_wifi.password){
    		printf("Last connected wifi: ssid=%s password=%s\n",  actual_wifi.ssid, actual_wifi.password);
    		//ESP_ERROR_CHECK( wifi_sta_conect(ssid, password) );
    		err = wifi_sta_start(actual_wifi.ssid, actual_wifi.password);
			if(err != ESP_OK){
				ESP_LOGW(webserver_webserver_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
    	}else{
    		err = wifi_ap_create();
			if(err != ESP_OK){
				ESP_LOGW(webserver_webserver_tag, "%s", wifi_err_to_string(err));
				ESP_ERROR_CHECK( err );
			}
    	}
	}*/

/*    err = save_wifi(ssid, "W6game55");
    if (err != ESP_OK){
        ESP_LOGW(webserver_webserver_tag, "( %d )", err);
        ESP_ERROR_CHECK( err );
    }*/
	
	return ESP_OK;
}

void openssl_esp32iot_task(void *p)
{
    int ret;

    SSL_CTX *ctx;
    SSL *ssl;

    int sockfd, new_sockfd;
    socklen_t addr_len;
    struct sockaddr_in sock_addr;

    char recv_buf[OPENSSL_ESP32IOT_RECV_BUF_LEN];

/*    const char send_data[sizeof(index_html)];
    strcpy(send_data, (char *)index_html);*/

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
    const unsigned int cacert_pem_bytes = cacert_pem_end - cacert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    const unsigned int prvtkey_pem_bytes = prvtkey_pem_end - prvtkey_pem_start;   

    ESP_LOGI(webserver_tag, "SSL server context create ......");
    /* For security reasons, it is best if you can use
       TLSv1_2_server_method() here instead of TLS_server_method().
       However some old browsers may not support TLS v1.2.
    */
    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed1;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server context set own certification......");
    ret = SSL_CTX_use_certificate_ASN1(ctx, cacert_pem_bytes, cacert_pem_start);
    if (!ret) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed2;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server context set private key......");
    ret = SSL_CTX_use_PrivateKey_ASN1(0, ctx, prvtkey_pem_start, prvtkey_pem_bytes);
    if (!ret) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed2;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server create socket ......");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed2;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server socket bind ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_ESP32IOT_LOCAL_TCP_PORT);
    ret = bind(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed3;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server socket listen ......");
    ret = listen(sockfd, 32);
    if (ret) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed3;
    }
    ESP_LOGI(webserver_tag, "OK");

reconnect:
    ESP_LOGI(webserver_tag, "SSL server create ......");
    ssl = SSL_new(ctx);
    if (!ssl) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed3;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server socket accept client ......");
    new_sockfd = accept(sockfd, (struct sockaddr *)&sock_addr, &addr_len);
    if (new_sockfd < 0) {
        ESP_LOGI(webserver_tag, "failed" );
        goto failed4;
    }
    ESP_LOGI(webserver_tag, "OK");

    SSL_set_fd(ssl, new_sockfd);

    ESP_LOGI(webserver_tag, "SSL server accept client ......");
    ret = SSL_accept(ssl);
    if (!ret) {
        ESP_LOGI(webserver_tag, "failed");
        goto failed5;
    }
    ESP_LOGI(webserver_tag, "OK");

    ESP_LOGI(webserver_tag, "SSL server read message ......");
    do {
        memset(recv_buf, 0, OPENSSL_ESP32IOT_RECV_BUF_LEN);
        ret = SSL_read(ssl, recv_buf, OPENSSL_ESP32IOT_RECV_BUF_LEN - 1);
        if (ret <= 0) {
            break;
        }
        ESP_LOGI(webserver_tag, "SSL read: %s", recv_buf);
        if (strstr(recv_buf, "GET ") && strstr(recv_buf, " HTTP/1.1")) {
            //printf("\nGET recv_buf: %s\n",recv_buf );
            ESP_LOGI(webserver_tag, "SSL get matched message")
            ESP_LOGI(webserver_tag, "SSL write message")
            if(strstr(recv_buf, "/ ")){
                ret = SSL_write(ssl, index_html, sizeof(index_html));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                }
            }else if(strstr(recv_buf, "/index ")){
                ret = SSL_write(ssl, index_html, sizeof(index_html));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                }
            }else if(strstr(recv_buf, "/wifi-setup-menu ")){
                ret = SSL_write(ssl, wifi_setup_html, sizeof(wifi_setup_html));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                }
            }else if(strstr(recv_buf, "/mqtt-setup-menu ")){
                ret = SSL_write(ssl, mqtt_setup_html, sizeof(mqtt_setup_html));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                }
            }else {
                ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_ERROR, sizeof(OPENSSL_ESP32IOT_SERVER_ERROR));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                } 
            }
            break;
        }else if(strstr(recv_buf, "POST ") && strstr(recv_buf, " HTTP/1.1")){
            printf("\nPOST recv_buf: %s\n",recv_buf );
            ESP_LOGI(webserver_tag, "SSL get matched message");
            ESP_LOGI(webserver_tag, "SSL write message");

            if(strstr(recv_buf, "/wifi-setup")){
                if(strstr(recv_buf, "ssid") && strstr(recv_buf, "password")){
                    ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_SUCCESS, sizeof(OPENSSL_ESP32IOT_SERVER_SUCCESS));
                    if (ret > 0) {
                        ESP_LOGI(webserver_tag, "OK")
                    } else {
                        ESP_LOGI(webserver_tag, "error")
                    }
                }else{
                    ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_EMPTY_FORM, sizeof(OPENSSL_ESP32IOT_SERVER_EMPTY_FORM));
                    if (ret > 0) {
                        ESP_LOGI(webserver_tag, "OK")
                    } else {
                        ESP_LOGI(webserver_tag, "error")
                    } 
                }
            }else if(strstr(recv_buf, "/mqtt-setup")){
                if(strstr(recv_buf, "username") && strstr(recv_buf, "password")){
                    ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_SUCCESS, sizeof(OPENSSL_ESP32IOT_SERVER_SUCCESS));
                    if (ret > 0) {
                        ESP_LOGI(webserver_tag, "OK")
                    } else {
                        ESP_LOGI(webserver_tag, "error")
                    }
                }else{
                    ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_EMPTY_FORM, sizeof(OPENSSL_ESP32IOT_SERVER_EMPTY_FORM));
                    if (ret > 0) {
                        ESP_LOGI(webserver_tag, "OK")
                    } else {
                        ESP_LOGI(webserver_tag, "error")
                    }
                }
            }else {
                ret = SSL_write(ssl, OPENSSL_ESP32IOT_SERVER_ERROR, sizeof(OPENSSL_ESP32IOT_SERVER_ERROR));
                if (ret > 0) {
                    ESP_LOGI(webserver_tag, "OK")
                } else {
                    ESP_LOGI(webserver_tag, "error")
                } 
            }
            break; 
        }
    } while (1);
    
    SSL_shutdown(ssl);
failed5:
    close(new_sockfd);
    new_sockfd = -1;
failed4:
    SSL_free(ssl);
    ssl = NULL;
    goto reconnect;
failed3:
    close(sockfd);
    sockfd = -1;
failed2:
    SSL_CTX_free(ctx);
    ctx = NULL;
failed1:
    vTaskDelete(NULL);
    return ;
} 

void openssl_server_init(void)
{
    int ret;
    xTaskHandle openssl_handle;

    ret = xTaskCreate(openssl_esp32iot_task,
                      OPENSSL_ESP32IOT_TASK_NAME,
                      OPENSSL_ESP32IOT_TASK_STACK_WORDS,
                      NULL,
                      OPENSSL_ESP32IOT_TASK_PRIORITY,
                      &openssl_handle); 

    if (ret != pdPASS)  {
        ESP_LOGI(webserver_tag, "create task %s failed", OPENSSL_ESP32IOT_TASK_NAME);
    }
}