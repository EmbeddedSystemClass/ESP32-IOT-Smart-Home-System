#ifndef NOTIFICATIONS_HTML
#define NOTIFICATIONS_HTML

const static unsigned char success_html[] =
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
  "<head>\r\n" \
    "<title>ESP32IOT Manager: Success</title>\r\n" \
  "</head>\r\n" \
  "<body>\r\n" \
    "<h1>Operation completed successfully!</h1>\r\n" \
  "</body>\r\n" \
"</html>\r\n" \
"\r\n";

const static unsigned char unknown_error_html[] =
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
  "<head>\r\n" \
    "<title>ESP32IOT Manager: Unknown Error</title>\r\n" \
  "</head>\r\n" \
  "<body>\r\n" \
    "<h1>Cannot complete operation!</h1>\r\n" \
  "</body>\r\n" \
"</html>\r\n" \
"\r\n";

const static unsigned char not_found_error_html[] =
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
  "<head>\r\n" \
    "<title>ESP32IOT Manager: 404 Not Found<</title>\r\n" \
  "</head>\r\n" \
  "<body>\r\n" \
    "<h1>404 Not Found</h1>\r\n" \
    "<h3>We cannot find the page you`re looking for!</h3>\r\n" \
  "</body>\r\n" \
"</html>\r\n" \
"\r\n";

const static unsigned char empty_field_form_html[] =
"<!DOCTYPE html>\r\n" \
"<html>\r\n" \
  "<head>\r\n" \
    "<title>ESP32IOT Manager: 204 No Content</title>\r\n" \
  "</head>\r\n" \
  "<body>\r\n" \
    "<h1>204 No Content</h1>\r\n" \
    "<h3>Form cannot be submitted, empty fields found. Fill out all fields!</h3>\r\n" \
  "</body>\r\n" \
"</html>\r\n" \
"\r\n";

#endif