#ifndef MQTT_SETUP_HTML
#define MQTT_SETUP_HTML

static const char mqtt_setup_html[] = 
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
"<head>\r\n" \
  "<meta charset=\"UTF-8\">\r\n" \
  "<title>ESP32IOT Manager: Configure MQTT<</title>\r\n" \
"</head>\r\n" \
"<body>\r\n" \
  "<div style=\"zoom: 340%;\">\r\n" \
    "<h1>Configure MQTT</h1>\r\n" \
    "<form action=\"mqtt-setup\" method=\"post\">\r\n" \
      "<table>\r\n" \
        "<tbody>\r\n" \
          "<tr>\r\n" \
            "<td>MQTT USERNAME:</td>\r\n" \
            "<td><input name=\"username\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
          "<tr>\r\n" \
            "<td>MQTT PASSWORD:</td>\r\n" \
            "<td><input name=\"password\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
          "<tr>\r\n" \
            "<td>CLIENT ID:</td>\r\n" \
            "<td><input name=\"id\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
          "<tr>\r\n" \
            "<td>MQTT SERVER:</td>\r\n" \
            "<td><input name=\"server\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
          "<tr>\r\n" \
            "<td>MQTT PORT:</td>\r\n" \
            "<td><input name=\"port\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
          "<tr>\r\n" \
            "<td>NAME YOUR DEVICE:</td>\r\n" \
            "<td><input name=\"name\" type=\"text\"></td>\r\n" \
          "</tr>\r\n" \
        "</tbody>\r\n" \
      "</table>\r\n" \
      "<p><input type=\"submit\" value=\"Submit\"></p>\r\n" \
    "</form>\r\n" \
  "</div>\r\n" \
"</body>\r\n" \
"</html>\r\n" \
"\r\n";

#endif