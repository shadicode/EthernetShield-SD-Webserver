/*--------------------------------------------------------------
  Program:      Ethernet Shield SD Webserver

  Date:         30 Mars 2018
 
  Author:       W.A. Smith / Shadi Soundation
--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// MAC address of Ethernet shield 
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x58, 0x5F};

// IP address
IPAddress ip(192, 168, 0, 30); 

// Port 80
EthernetServer server(80);  

File webFile;

void setup()
{
    Ethernet.begin(mac, ip); 
    server.begin();           // start to listen
    
    // initialize SD card
    SD.begin(4);
        
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    
                    // send web page
                    webFile = SD.open("index.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    break;
                }
                
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } 
        }
        
        delay(1);      // give the web browser time to receive the data
        client.stop();
    } 
}
