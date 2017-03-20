#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Timer.h>
#include <Rrest.h>

ESP8266WebServer server(80);
Timer t;
Rrest r;

// const char* ssid = "SFR_8550";
// const char* password = "noff0erumustaltmisec";

// const char* ssid = "SFR-d4d7";
// const char* password = "v9ngdwtka37c";

const char* ssid = "SebastianAp";
const char* password = "exiacesi2017";


void cycleLight() {
  static bool lightStatus = false;

  if(!lightStatus){
    Serial.println("light turned on");
    lightStatus = true;
  } else {
    Serial.println("light turned off");
    lightStatus = false;
  }
}

void listLight() {
  Serial.print("There is ");
  static int i = 0;
  Serial.print(i++);
  Serial.println(" lights");
}

void groupLight(uint8_t ressourceId) {
  Serial.print("grouped light callback with id");
  Serial.println(ressourceId);
}

void handleico() {
  server.send(404);
}

void handleNotFound() {
  Serial.println("handleNotFound");
  r.handleRest();
  server.send(404);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    delay(1000);
    WiFi.begin(ssid, password);
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  while(!MDNS.begin("esp8266server")) {
    delay(1000);
    Serial.println("Error setting up MDNS responder!");
  }

  MDNS.addService("esp", "tcp", 80);
  Serial.println("mDNS Responder setup");

  server.on("/", [](){
    String message = "root not found";
    server.send(200,"text/plain", message);
  });
  server.on("/favicon.ico", handleico);
  server.onNotFound(handleNotFound);

  //HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
