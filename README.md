# Smart Home System README

0. ToDo: 

    -          Dodanie mozliwosci wyboru sieci po SSID z listy dostępnych

    -          Przywrócenie ustawien fabrycznych przyciskiem

    -          Wyświetlenie adresu konfiguracji IP w panelu sterowania

    -          Wyświetl haslo podczas wpisywania

1.  Manual:

        a)  Configure:

        b)  
                    
2.  How to develop WebPage:

        1.  Code pageX.html content file like for regularar website except all script and styles must be implemented inline in pageX.html file. 

        2.  Create pageX_html.h file according to template in project_dir/tmp/webpage directory.

        3.  Convert pageX.html to c/c++ string format. 
                For example on webpage:
                    1.  
                        a)  Go toOnline C++ string Escape Tool | Online CPP Escape Tool | Online C Escape Tool: http://easyonlineconverter.com/converters/cpp-string-escape.html
                        b)
                    2. 
        3.  Copy converted content to const static unsigned char pageX_html[] variable in pageX_html.h file.

        4.  

3.  Cayenne MQTT:
        
        I.  Used channels description:

            0.  S/M/H gate:

                Channel 9:
                    a) data:
                        S/M/H gate IP info
                    b)

                Channel 8:
                    a) data:
                        S/M/H gate restart
                    b)  

                Channel X:
                    a)  data:

                    b) 


            1.  BA5C:

                Channel 0:
                    a) data:
                        BA5C HTU21D temperature
                    b)  

                Channel 1:
                    a) data:
                        BA5C HTU21D humidity
                    b)  

                Channel 2:
                    a) data:
                        BA5C HTU21D status
                    b)  

                Channel 3:
                    a) data:
                        BA5C MS5637 temperature
                    b)  

                Channel 4:
                    a) data:
                        BA5C MS5637 pressure
                    b)  

                Channel 5:
                    a) data:
                        BA5C MS5637 status
                    b)  

                Channel 6:
                    a) data:
                        BA5C Battery level
                    b)  

                Channel 7:
                    a) data:
                        BA5C Battery status
                    b)  


        II. 


4.                       