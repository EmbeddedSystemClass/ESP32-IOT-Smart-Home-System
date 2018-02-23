#include "esp32iot_http_server.h"


static void http_server_netconn_serve(struct netconn *conn){
  struct netbuf *inbuf;
  char *recv_buf;
  u16_t buflen;
  err_t err;

  /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
  err = netconn_recv(conn, &inbuf);
  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&recv_buf, &buflen);

    // strncpy(_mBuffer, recv_buf, buflen);

    /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
    printf("buffer = %s \n", recv_buf);
/*    if (buflen>=5 &&
        recv_buf[0]=='G' &&
        recv_buf[1]=='E' &&
        recv_buf[2]=='T' &&
        recv_buf[3]==' ' &&
        recv_buf[4]=='/' ) {
          printf("recv_buf[5] = %c\n", recv_buf[5]);
       Send the HTML header
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
       

      netconn_write(conn, http_html_header, sizeof(http_html_header)-1, NETCONN_NOCOPY);

      if(recv_buf[5]=='j') {
          netconn_write(conn, json_unformatted, strlen(json_unformatted), NETCONN_NOCOPY);
      }
      else {
          netconn_write(conn, index_html, sizeof(index_html)-1, NETCONN_NOCOPY);
      }
    }*/

    if (strstr(recv_buf, "GET ") && strstr(recv_buf, " HTTP/1.1")) {
      //printf("\nGET recv_buf: %s\n",recv_buf );
      if(strstr(recv_buf, "/ ")){
        netconn_write(conn, index_html_header, sizeof(index_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, index_html, sizeof(index_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        }
      }else if(strstr(recv_buf, "/index ")){
        netconn_write(conn, index_html_header, sizeof(index_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, index_html, sizeof(index_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        }
      }else if(strstr(recv_buf, "/wifi-setup-menu ")){
        netconn_write(conn, wifi_setup_html_header, sizeof(wifi_setup_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, wifi_setup_html, sizeof(wifi_setup_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        }
      }else if(strstr(recv_buf, "/mqtt-setup-menu ")){
        netconn_write(conn, mqtt_setup_html_header, sizeof(mqtt_setup_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, mqtt_setup_html, sizeof(mqtt_setup_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        }
      }else {
        netconn_write(conn, not_found_error_html_header, sizeof(not_found_error_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, not_found_error_html, sizeof(not_found_error_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        } 
      }
  }else if(strstr(recv_buf, "POST ") && strstr(recv_buf, " HTTP/1.1")){
      //printf("\ninbuf: %s\n",  inbuf.p.payload);

      if(strstr(recv_buf, "/wifi-setup")){
        char ssid[33];
        char password[64];
        
        parse_http_request(recv_buf, "ssid", &ssid);
        printf("\nssid=%s", ssid); 
        parse_http_request(recv_buf, "password", &password);
        printf("\password=%s", password); 
        //if(ssid!=""){
         
       /// }

        if(strstr(recv_buf, "ssid") && strstr(recv_buf, "password")){
          netconn_write(conn, success_html_header, sizeof(success_html_header)-1, NETCONN_NOCOPY);
          err = netconn_write(conn, success_html, sizeof(success_html), NETCONN_NOCOPY);
          if (err != ESP_OK) {
            ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
            ESP_ERROR_CHECK( err );
          }
        }else{
          netconn_write(conn, empty_field_form_html_header, sizeof(empty_field_form_html_header)-1, NETCONN_NOCOPY);
          err = netconn_write(conn, empty_field_form_html, sizeof(empty_field_form_html), NETCONN_NOCOPY);
          if (err != ESP_OK) {
            ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
            ESP_ERROR_CHECK( err );
          }
        }
      }else if(strstr(recv_buf, "/mqtt-setup")){
        if(strstr(recv_buf, "username") && strstr(recv_buf, "password")){
          netconn_write(conn, success_html_header, sizeof(success_html_header)-1, NETCONN_NOCOPY);
          err = netconn_write(conn, success_html, sizeof(success_html), NETCONN_NOCOPY);
          if (err != ESP_OK) {
            ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
            ESP_ERROR_CHECK( err );
          }
        }else{
          netconn_write(conn, empty_field_form_html_header, sizeof(empty_field_form_html_header)-1, NETCONN_NOCOPY);
          err = netconn_write(conn, empty_field_form_html, sizeof(empty_field_form_html), NETCONN_NOCOPY);
          if (err != ESP_OK) {
            ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
            ESP_ERROR_CHECK( err );
          }
        }
      }else if(strstr(recv_buf, "/erase-wifi-data ")){
        err = erase_storage();
        if(err != ESP_OK){
          ESP_LOGW(wifi_tag, "%s", wifi_err_to_string(err));
          ESP_ERROR_CHECK( err );
        }else{
          netconn_write(conn, success_html_header, sizeof(success_html_header)-1, NETCONN_NOCOPY);
          err = netconn_write(conn, success_html, sizeof(success_html), NETCONN_NOCOPY);
          if (err != ESP_OK) {
            ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
            ESP_ERROR_CHECK( err );
          }
        }
      }else if(strstr(recv_buf, "/restart ")){
        netconn_write(conn, success_html_header, sizeof(success_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, success_html, sizeof(success_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        }
        printf("Restarting now.\n");
        fflush(stdout);
        delay(5000);
        esp_restart();
      }else {
        netconn_write(conn, not_found_error_html_header, sizeof(not_found_error_html_header)-1, NETCONN_NOCOPY);
        err = netconn_write(conn, not_found_error_html, sizeof(not_found_error_html), NETCONN_NOCOPY);
        if (err != ESP_OK) {
          ESP_LOGW(http_server_tag, "error: %s ", lwip_strerr(err));
          ESP_ERROR_CHECK( err );
        } 
      }
    }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);

  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

static void http_server(void *pvParameters){
  struct netconn *conn, *newconn;
  err_t err;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, NULL, 80);
  netconn_listen(conn);
  do {
     err = netconn_accept(conn, &newconn);
     if (err == ERR_OK) {
       http_server_netconn_serve(newconn);
       netconn_delete(newconn);
     }
   } while(err == ERR_OK);
   netconn_close(conn);
   netconn_delete(conn);
}

void http_server_init(void){
    err_t err;

    err = xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
    if (err != pdPASS)  {
        ESP_LOGI(http_server_tag, "create task %s failed", HTTP_SERVER_ESP32IOT_TASK_NAME);
    }
}

#include <ctype.h>
//ssid=hh&password=gg 
static err_t parse_http_request(const char* request, const char* key, const char* value){
  err_t err = 0;


  fflush(stdout);

  return err;
}