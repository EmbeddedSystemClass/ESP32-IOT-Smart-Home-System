#ifndef INDEX_HTML
#define INDEX_HTML

static unsigned char index_html[] = 
"<!DOCTYPE html>"
"<html>\n"
"<head>\n"
  "<meta charset=\"UTF-8\">\n"
  "<title>Configuration Menu</title>\n"
"</head>\n"
"<body>\n"
  "<div style=\"zoom: 340%;\">\n"
    "<h1>Configuration Menu</h1>\n"
    "<form action=\"/wifi-setup-menu\" method=\"post\">\n"
        "<button type=\"submit\">Configure WiFi</button><br>\n"
        //"<!-- <button type="submit" formaction="/wifi-setup-menu">Configure WiFi</button> -->\n""
    "</form>\n" 
    "<form action=\"/mqtt-setup-menu\" method=\"post\">\n"
        "<button type=\"submit\">Configure MQTT</button><br>\n"
        //"<!-- <button type="submit" formaction="/mqtt-setup-menu">Configure MQTT</button> -->\n"
    "</form>\n"
  "</div>\n"
"</body>\n"
"</html>\n";

#endif