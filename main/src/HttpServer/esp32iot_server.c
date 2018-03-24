#include "esp32iot_server.h"

// handles websocket events
void http_websocket_server_callback(uint8_t num, WEBSOCKET_TYPE_t type, char* msg, uint64_t len) {
  const static char* LOCAL_TAG = "http_websocket_server_callback";
  int value;

  switch(type) {
    case WEBSOCKET_CONNECT:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_CONNECT: client %i connected!",num);

      break;
    case WEBSOCKET_DISCONNECT_EXTERNAL:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_DISCONNECT_EXTERNAL: client %i sent a disconnect message",num);

      break;
    case WEBSOCKET_DISCONNECT_INTERNAL:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_DISCONNECT_INTERNAL: client %i was disconnected",num);

      break;
    case WEBSOCKET_DISCONNECT_ERROR:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_DISCONNECT_ERROR: client %i was disconnected due to an error",num);

      break;
    case WEBSOCKET_TEXT:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_TEXT:");
      //ws_server_send_text_all_from_callback(msg,len); // broadcast it!

      break;
    case WEBSOCKET_BIN:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_BIN: client %i sent binary message of size %i:\n%s",num,(uint32_t)len,msg);

      break;
    case WEBSOCKET_PING:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_PING: client %i pinged us with message of size %i:\n%s",num,(uint32_t)len,msg);

      break;
    case WEBSOCKET_PONG:
      ESP_LOGI(LOCAL_TAG,"WEBSOCKET_PONG: client %i responded to the ping",num);
      
      break;
  }
}

static void http_netconn_serve(struct netconn *conn){
  const static char* LOCAL_TAG = "http_server";
  const static char HTML_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
  const static char ERROR_HEADER[] = "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n";
  const static char JS_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/javascript\n\n";
  const static char CSS_HEADER[] = "HTTP/1.1 200 OK\nContent-type: text/css\n\n";
  //const static char PNG_HEADER[] = "HTTP/1.1 200 OK\nContent-type: image/png\n\n";
  const static char ICO_HEADER[] = "HTTP/1.1 200 OK\nContent-type: image/x-icon\n\n";
  //const static char PDF_HEADER[] = "HTTP/1.1 200 OK\nContent-type: application/pdf\n\n";
  //const static char EVENT_HEADER[] = "HTTP/1.1 200 OK\nContent-Type: text/event-stream\nCache-Control: no-cache\nretry: 3000\n\n";
  struct netbuf* inbuf;
  static char* buf;
  static uint16_t buflen;
  static err_t err;

  // default page
  extern const uint8_t root_html_start[] asm("_binary_root_html_start");
  extern const uint8_t root_html_end[] asm("_binary_root_html_end");
  const uint32_t root_html_len = root_html_end - root_html_start;

  // test.js
  extern const uint8_t test_js_start[] asm("_binary_test_js_start");
  extern const uint8_t test_js_end[] asm("_binary_test_js_end");
  const uint32_t test_js_len = test_js_end - test_js_start;

  // test.css
  extern const uint8_t test_css_start[] asm("_binary_test_css_start");
  extern const uint8_t test_css_end[] asm("_binary_test_css_end");
  const uint32_t test_css_len = test_css_end - test_css_start;

  // favicon.ico
  extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
  extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");
  const uint32_t favicon_ico_len = favicon_ico_end - favicon_ico_start;

  // error page
  extern const uint8_t error_html_start[] asm("_binary_error_html_start");
  extern const uint8_t error_html_end[] asm("_binary_error_html_end");
  const uint32_t error_html_len = error_html_end - error_html_start;

  netconn_set_recvtimeout(conn,1000); // allow a connection timeout of 1 second
  ESP_LOGI(LOCAL_TAG,"reading from client...");
  err = netconn_recv(conn, &inbuf);
  ESP_LOGI(LOCAL_TAG,"read from client");
  if(err==ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);
    if(buf) {

      // default page
      if(strstr(buf,"GET / ") && !strstr(buf,"Upgrade: websocket") && !strstr(buf,"Connection: Upgrade")) {
        
        ESP_LOGI(LOCAL_TAG,"Sending /");
        netconn_write(conn, HTML_HEADER, sizeof(HTML_HEADER)-1,NETCONN_NOCOPY);
        netconn_write(conn, root_html_start,root_html_len,NETCONN_NOCOPY);
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }

      // default page websocket
      else if(strstr(buf,"GET / ") && strstr(buf,"Upgrade: websocket") && strstr(buf,"Connection: Upgrade")) {

        ESP_LOGI(LOCAL_TAG,"Requesting websocket on /");
        ws_server_add_client(conn,buf,buflen,"/",http_websocket_server_callback);
        netbuf_delete(inbuf);
      }

      else if(strstr(buf,"GET /test.js ")) {
        
        ESP_LOGI(LOCAL_TAG,"Sending /test.js");
        netconn_write(conn, JS_HEADER, sizeof(JS_HEADER)-1,NETCONN_NOCOPY);
        netconn_write(conn, test_js_start, test_js_len,NETCONN_NOCOPY);
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }

      else if(strstr(buf,"GET /test.css ")) {
        
        ESP_LOGI(LOCAL_TAG,"Sending /test.css");
        netconn_write(conn, CSS_HEADER, sizeof(CSS_HEADER)-1,NETCONN_NOCOPY);
        netconn_write(conn, test_css_start, test_css_len,NETCONN_NOCOPY);
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }

      else if(strstr(buf,"GET /favicon.ico ")) {
        
        ESP_LOGI(LOCAL_TAG,"Sending favicon.ico");
        netconn_write(conn,ICO_HEADER,sizeof(ICO_HEADER)-1,NETCONN_NOCOPY);
        netconn_write(conn,favicon_ico_start,favicon_ico_len,NETCONN_NOCOPY);
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }

      else if(strstr(buf,"GET /")) {
        
        ESP_LOGI(LOCAL_TAG,"Unknown request, sending error page: %s",buf);
        netconn_write(conn, ERROR_HEADER, sizeof(ERROR_HEADER)-1,NETCONN_NOCOPY);
        netconn_write(conn, error_html_start, error_html_len,NETCONN_NOCOPY);
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }

      else {
        
        ESP_LOGI(LOCAL_TAG,"Unknown request");
        netconn_close(conn);
        netconn_delete(conn);
        netbuf_delete(inbuf);
      }
    }
    else {
      
      ESP_LOGI(LOCAL_TAG,"Unknown request (empty?...)");
      netconn_close(conn);
      netconn_delete(conn);
      netbuf_delete(inbuf);
    }
  }
  else { // if err==ERR_OK
    
    ESP_LOGI(LOCAL_TAG,"error on read, closing connection");
    netconn_close(conn);
    netconn_delete(conn);
    netbuf_delete(inbuf);
  }
}

// handles clients when they first connect. passes to a queue
static void server_task(void* pvParameters) {
  const static char* LOCAL_TAG = "server_task";
  struct netconn *conn, *newconn;
  static err_t err;
  client_queue = xQueueCreate(client_queue_size,sizeof(struct netconn*));

  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn,NULL,80);
  netconn_listen(conn);
  ESP_LOGI(LOCAL_TAG,"server listening");
  do {
    err = netconn_accept(conn, &newconn);
    ESP_LOGI(LOCAL_TAG,"new client");
    if(err == ERR_OK) {
      xQueueSendToBack(client_queue,&newconn,portMAX_DELAY);
      //http_serve(newconn);
    }
  } while(err == ERR_OK);
  netconn_close(conn);
  netconn_delete(conn);
  ESP_LOGE(LOCAL_TAG,"task ending, rebooting board");
  esp_restart();
}



static void http_server_handle_task(void* pvParameters) {
  const static char* LOCAL_TAG = "http_server_handle_task";
  struct netconn* conn;
  ESP_LOGI(LOCAL_TAG,"task starting");
  for(;;) {
    xQueueReceive(client_queue,&conn,portMAX_DELAY);
    if(!conn) continue;
    http_netconn_serve(conn);
  }
  vTaskDelete(NULL);
}

static void http_server_task(void *pvParameters){
  const static char* LOCAL_TAG = "http_server_task";
  struct netconn *conn, *newconn;
  static err_t err;
  client_queue = xQueueCreate(client_queue_size,sizeof(struct netconn*));

  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn,NULL,80);
  netconn_listen(conn);
  ESP_LOGI(LOCAL_TAG,"server listening");
  do {
    err = netconn_accept(conn, &newconn);
    ESP_LOGI(LOCAL_TAG,"new client");
    if(err == ERR_OK) {
      xQueueSendToBack(client_queue,&newconn,portMAX_DELAY);
      //http_serve(newconn);
    }
  } while(err == ERR_OK);
  netconn_close(conn);
  netconn_delete(conn);
  ESP_LOGE(LOCAL_TAG,"task ending, rebooting board");
  esp_restart();
}


void http_server_init(void){
  err_t err;

  ws_server_start();

  xTaskCreate(&http_server_task,"http_server_task",3000,NULL,9,NULL);
  xTaskCreate(&http_server_handle_task,"http_server_handle_task",4000,NULL,6,NULL);
}


static err_t parse_http_request(const char* request, const char key[], char* value){
  err_t err = 0;

/*
  const char *request_params = strstr(request, key);
    printf("\n1: %s",request_params);*/

  if(strstr(request, key) != NULL){
    const char *param_start = strstr(request, key);
    //  printf("\n1: %s",param_start);

    char *param_end = strchr(param_start, '&');
    if(param_end == NULL){
      param_end = strchr(param_start, '\0');
    //  printf("\n2b: %s",param_end);
    }else{
    //  printf("\n2a: %s",param_end);
    }
    
    char tmp_param[param_end-param_start];
    strncpy(tmp_param, param_start, param_end-param_start);
    tmp_param[sizeof(tmp_param)] = 0;
    // printf("\n3: %s, size: %i",tmp_param, sizeof(tmp_param));

    // printf("\n4: %d",strlen(key));
    char param[strlen(tmp_param)-strlen(key)-1];
    //  printf("\n5: %d", sizeof(param));
    for(int i = 0; i < sizeof(param); i++){
      param[i]=tmp_param[strlen(key) + 1 + i];
    }
    param[sizeof(param)]='\0';
    //  printf("\n6: %s", param);

    //value = (char*)malloc(strlen(param) * sizeof(char));
    for(size_t i = 0; i<= sizeof(param); i++)
      value[i] = param[i];    
    //copy_string((uint8_t)value, (uint8_t)param);

  }else{
    ESP_LOGI(http_server_tag, "Key: %s not found.", key);
    err = 1;
  }

  fflush(stdout);
  return err;
}

static err_t parse_http_request2(const char* request, const char key[], char** value){
  err_t err = 0;

/*
  const char *request_params = strstr(request, key);
    printf("\n1: %s",request_params);*/

  if(strstr(request, key) != NULL){
    const char *param_start = strstr(request, key);
    //  printf("\n1: %s",param_start);

    char *param_end = strchr(param_start, '&');
    if(param_end == NULL){
      param_end = strchr(param_start, '\0');
    //  printf("\n2b: %s",param_end);
    }else{
    //  printf("\n2a: %s",param_end);
    }
    
    char tmp_param[param_end-param_start];
    strncpy(tmp_param, param_start, param_end-param_start);
    tmp_param[sizeof(tmp_param)] = 0;
    // printf("\n3: %s, size: %i",tmp_param, sizeof(tmp_param));

    // printf("\n4: %d",strlen(key));
    char param[strlen(tmp_param)-strlen(key)-1];
    //  printf("\n5: %d", sizeof(param));
    for(int i = 0; i < sizeof(param); i++){
      param[i]=tmp_param[strlen(key) + 1 + i];
    }
    param[sizeof(param)]='\0';
    //printf("\n6: %s", param);

    //ESP_LOGI(http_server_tag, "\nsizeof(param): %d | strlen(param): %d",sizeof(param), strlen(param));

    *value = (char*)malloc((strlen(param)+1) * sizeof(char));
    if(value == NULL){
      ESP_LOGI(http_server_tag, "\nvalue pointer NULL");
      return 100;
    }
    sprintf(*value, "%s", param);
    //ESP_LOGI(http_server_tag, "\nparam: %s | value: %s",param, *value);
  }else{
    ESP_LOGI(http_server_tag, "Key: %s not found.", key);
    err = 1;
  }

  fflush(stdout);
  return err;
}