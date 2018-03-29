#ifndef INDEX_HTML
#define INDEX_HTML

const static unsigned char index_template_header[] = 
"HTTP/1.1 200 OK\r\n" \
"Content-Type: text/html\r\n" \
"\r\n";

const static unsigned char index_template_html[] =

"<!DOCTYPE html><html lang=\"en\" >  <head>    <meta charset=\"UTF-8\">    <title>WebPage Template</title>    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=yes\">    <style media=\"screen\" type=\"text/css\">    </style>  </head>  <body>        <script type=\"text/javascript\">         </script>  </body></html>"

;

/*"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">\r\n" \
"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">\r\n" \
"<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>\r\n" \
*/
#endif