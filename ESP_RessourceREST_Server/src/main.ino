#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Timer.h>
#include <Rrest.h>

ESP8266WebServer server(80);
Timer t;
Rrest r;

const char* ssid = "1337 internet";
const char* password = "icanhasinternet";

void handleico() {
  server.send(404);
}

void handleNotFound() {
  if(!r.handleRest()) { //we call rest when the webserver doesn't recognize the entered URI
    String message = "404 not found";
    server.send(404,"text/plain", message);
    notfound.close();
  } else {
    server.send(200);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    delay(1000);
    WiFi.begin(ssid, password);
  }

  Serial.println(F("WiFi connected"));
  Serial.println(WiFi.localIP());

  while(!MDNS.begin("esp8266Server")) {
    delay(1000);
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS Responder setup");

  server.on("/", handleNotFound);
  server.on("/favicon.ico", handleico);
  server.onNotFound(handleNotFound);

  //HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

}
