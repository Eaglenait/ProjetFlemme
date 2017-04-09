#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Timer.h>
#include <Rrest.h>

ESP8266WebServer server(80);
Timer t;
Rrest r;

const char* ssid = "SFR_8550";
const char* password = "noff0erumustaltmisec";

// const char* ssid = "SFR-d4d7";
// const char* password = "v9ngdwtka37c";

// const char* ssid = "SebastianAp";
// const char* password = "exiacesi2017";

const int GREEN   = 12;
const int BLUE    = 13;
const int RED     = 15;

void lightOn() {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,255);
  server.send(200);
}

void lightOff() {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,0);
  server.send(200);
}

void lightStatus() {
  Serial.print("Light is ");
  Serial.println(analogRead(GREEN));
  server.send(200);
}

void handleico() {
  server.send(404);
}

void handleNotFound() {
  if(!r.handleRest()) {
    server.send(404);
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

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  pinMode(RED,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(GREEN,OUTPUT);

  while(!MDNS.begin("esp")) {
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
  t.update();
}
