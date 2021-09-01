/* ESP-32 Captive portal example
 * github.com/elliotmade/ESP32-Captive-Portal-Example
 * This isn't anything new, and doesn't do anything special
 * just an example I would have appreciated while I was searching for a solution
 */


#include <Arduino.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"



const char* ssid = "test_captive_portal"; //Name of the WIFI network hosted by the device
const char* password =  "";               //Password

AsyncWebServer server(80);                //This creates a web server, required in order to host a page for connected devices

DNSServer dnsServer;                      //This creates a DNS server, required for the captive portal


void webServerSetup(){
  
  //This is a super simple page that will be served up any time the root location is requested.  Get here intentionally by typing in the IP address.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<!DOCTYPE html><html><head><title>Success</title></head><body><p>Hooray</p></body>");
    Serial.println("requested /");
  });


  //This is an example of triggering for a known location.  This one seems to be common for android devices
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "You were sent here by a captive portal after requesting generate_204");
    Serial.println("requested /generate_204");
  });

  //This is an example of a redirect type response.  onNotFound acts as a catch-all for any request not defined above
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
    Serial.print("server.notfound triggered: ");
    Serial.println(request->url());       //This gives some insight into whatever was being requested
  });

  server.begin();                         //Starts the server process
  Serial.println("Web server started");
}




void setup() {
  
  Serial.begin(115200);

  WiFi.softAP(ssid, password);            //This starts the WIFI radio in access point mode
  Serial.println("Wifi initialized");

  Serial.println(WiFi.softAPIP());        //Print out the IP address on the serial port (this is where you should end up if the captive portal works)

  dnsServer.start(53, "*", WiFi.softAPIP());  //This starts the DNS server.  The "*" sends any request for port 53 straight to the IP address of the device

  webServerSetup();                       //Configures the behavior of the web server
  
  Serial.println("Setup complete");
}


void loop() {
  dnsServer.processNextRequest();         //Without this, the connected device will simply timeout trying to reach the internet
                                          //or it might fall back to mobile data if it has it
  
}