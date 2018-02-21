#ifndef WIFI_SETUP_HTML
#define WIFI_SETUP_HTML

static unsigned char wifi_setup_html[] = 
"<!DOCTYPE html>"
"<html>\n"
"<head>\n"
  "<meta charset=\"UTF-8\">\n"
  "<title>ESP32IOT Manager: Configure WiFi</title>\n"
"</head>\n"
"<body>\n"
  "<div style=\"zoom: 340%;\">\n"
    "<h1>Configure WiFi</h1>\n"
    "<form action=\"wifi-setup\" method=\"post\">\n"
      "<table>\n"
        "<tbody>\n"
          "<tr>\n"
            "<td>SSID:</td>\n"
            "<td><input name=\"ssid\" type=\"text\"></td>\n"
          "</tr>\n"
          "<tr>\n"
            "<td>Password:</td>\n"
            "<td><input name=\"password\" type=\"text\"></td>\n"
          "</tr>\n"
        "</tbody>\n"
      "</table>\n"
      "<p><input type=\"submit\" value=\"Submit\"></p>\n"
    "</form>\n"
  "</div>\n"
"</body>\n"
"</html>\n";

#endif