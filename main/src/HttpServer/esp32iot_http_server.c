#include "esp32iot_http_server.h"

char *mg_ev_to_string(int ev) {
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      return "MG_EV_HTTP_REQUEST";
    case MG_EV_HTTP_REPLY:
      return "MG_EV_HTTP_REPLY";
    case MG_EV_HTTP_CHUNK:
      return "MG_EV_HTTP_CHUNK";
    case MG_EV_SSI_CALL:
      return "MG_EV_SSI_CALL";
    case MG_EV_SSI_CALL_CTX:
      return "MG_EV_SSI_CALL_CTX";
    #if MG_ENABLE_HTTP_WEBSOCKET
    case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST:
      return "MG_EV_WEBSOCKET_HANDSHAKE_REQUEST";
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
      return "MG_EV_WEBSOCKET_HANDSHAKE_DONE";
    case MG_EV_WEBSOCKET_FRAME:
      return "MG_EV_WEBSOCKET_FRAME";
    case MG_EV_WEBSOCKET_CONTROL_FRAME:
      return "MG_EV_WEBSOCKET_CONTROL_FRAME";
    #endif
    #if MG_ENABLE_HTTP_STREAMING_MULTIPART
    case MG_EV_HTTP_MULTIPART_REQUEST:
      return "MG_EV_HTTP_MULTIPART_REQUEST";
    case MG_EV_HTTP_PART_BEGIN:
      return "MG_EV_HTTP_PART_BEGIN";
    case MG_EV_HTTP_PART_DATA:
      return "MG_EV_HTTP_PART_DATA";
    case MG_EV_HTTP_PART_END:
      return "MG_EV_HTTP_PART_END";
    /* struct mg_http_multipart_part */
    case MG_EV_HTTP_MULTIPART_REQUEST_END:
      return "MG_EV_HTTP_MULTIPART_REQUEST_END";
    #endif
    case MG_EV_CONNECT:
      return "MG_EV_CONNECT";
    case MG_EV_ACCEPT:
      return "MG_EV_ACCEPT";
    case MG_EV_CLOSE:
      return "MG_EV_CLOSE";
    case MG_EV_SEND:
      return "MG_EV_SEND";
    case MG_EV_RECV:
      return "MG_EV_RECV";
    case MG_EV_MQTT_CONNACK:
      return "MG_EV_MQTT_CONNACK";
    case MG_EV_MQTT_CONNACK_ACCEPTED:
      return "MG_EV_MQTT_CONNACK_ACCEPTED";
    case MG_EV_MQTT_CONNECT:
      return "MG_EV_MQTT_CONNECT";
    case MG_EV_MQTT_DISCONNECT:
      return "MG_EV_MQTT_DISCONNECT";
    case MG_EV_MQTT_PINGREQ:
      return "MG_EV_MQTT_PINGREQ";
    case MG_EV_MQTT_PINGRESP:
      return "MG_EV_MQTT_PINGRESP";
    case MG_EV_MQTT_PUBACK:
      return "MG_EV_MQTT_PUBACK";
    case MG_EV_MQTT_PUBCOMP:
      return "MG_EV_MQTT_PUBCOMP";
    case MG_EV_MQTT_PUBLISH:
      return "MG_EV_MQTT_PUBLISH";
    case MG_EV_MQTT_PUBREC:
      return "MG_EV_MQTT_PUBREC";
    case MG_EV_MQTT_PUBREL:
      return "MG_EV_MQTT_PUBREL";
    case MG_EV_MQTT_SUBACK:
      return "MG_EV_MQTT_SUBACK";
    case MG_EV_MQTT_SUBSCRIBE:
      return "MG_EV_MQTT_SUBSCRIBE";
    case MG_EV_MQTT_UNSUBACK:
      return "MG_EV_MQTT_UNSUBACK";
    case MG_EV_MQTT_UNSUBSCRIBE:
      return "MG_EV_MQTT_UNSUBSCRIBE";
  }

  static char unknown[100];
  sprintf(unknown, "Unknown event: %d", ev);
  return unknown;
}

static void mg_ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  esp_err_t err;

  struct http_message *hm = (struct http_message *) ev_data;
  switch (ev) {
    case MG_EV_HTTP_REQUEST: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));

      char addr[32];
      mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      ESP_LOGI(http_server_tag, "HTTP request from %s: %.*s %.*s\n", addr, (int) hm->method.len,
             hm->method.p, (int) hm->uri.len, hm->uri.p);
      mg_send(nc, index_html, sizeof(index_html));
      nc->flags |= MG_F_SEND_AND_CLOSE;

      break;
    }
    case MG_EV_ACCEPT: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));

      char addr[32];
      mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      ESP_LOGI(http_server_tag, "Connection %p from %s\n", nc, addr);

      break;
    }
    case MG_EV_CLOSE: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));
      ESP_LOGI(http_server_tag, "Connection %p closed\n", nc);

      break;
    }
    case MG_EV_RECV: {
      
      break;
    }
    case MG_EV_HTTP_CHUNK: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));

      break;
    }
    case MG_EV_SEND: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));

      break;
    }
    case MG_EV_MQTT_CONNACK: {
      //ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));
      
      break;
    }
    case MG_EV_MQTT_CONNACK_ACCEPTED: {
      ESP_LOGI(http_server_tag, "%s", mg_ev_to_string(ev));

      break;
    }
    default: {
      ESP_LOGW(http_server_tag, "( %s )", mg_ev_to_string(ev));

      break;
    }
  }
}

void mg_init(void *data){
  /* Starting Mongoose */
  struct mg_mgr mgr;
  struct mg_connection *nc;

  ESP_LOGI(http_server_tag, "Starting web-server on port %s\n", MG_LISTEN_ADDR);

  mg_mgr_init(&mgr, NULL);

  nc = mg_bind(&mgr, MG_LISTEN_ADDR, mg_ev_handler);
  if (nc == NULL) {
    ESP_LOGI(http_server_tag, "Error setting up listener!\n");
    return;
  }
  mg_set_protocol_http_websocket(nc);

  /* Processing events */
  for(;;){
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return;
}

void http_server_init(void *data){
  mg_init(data);

  return;
}