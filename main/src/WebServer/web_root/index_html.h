#ifndef INDEX_HTML
#define INDEX_HTML

const static unsigned char index_html_header[] = 
"HTTP/1.1 200 OK\r\n" \
"Content-Type: text/html\r\n\r\n";

const static unsigned char index_html[] =
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
"<head>\r\n" \
  "<meta charset=\"UTF-8\">\r\n" \
  "<title>ESP32IOT Manager: Configuration Menu</title>\r\n" \
"</head>\r\n" \
"<body>\r\n" \
  "<div style=\"zoom: 340%;\">\r\n" \
    "<h1>Configuration Menu</h1>\r\n" \
    "<form action=\"/wifi-setup-menu\" method=\"get\">\r\n" \
        "<button type=\"submit\">Configure WiFi</button><br>\r\n" \
    "</form>\r\n" \ 
    "<form action=\"/mqtt-setup-menu\" method=\"get\">\r\n" \
        "<button type=\"submit\">Configure MQTT</button><br>\r\n" \
    "</form>\r\n" \
  "</div>\r\n" \
"</body>\r\n" \
"</html>\r\n" \
"\r\n";

/*"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">\r\n" \
"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">\r\n" \
"<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>\r\n" \
*/
#endif