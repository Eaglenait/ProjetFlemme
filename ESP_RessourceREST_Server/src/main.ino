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

const int GREEN   = 12;
const int BLUE    = 13;
const int RED     = 15;

void lightOn() {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,255);
}

void lightOn(uint8_t ressourcesID) {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,255);
}

void lightOff() {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,0);
}

void lightOff(uint8_t ressourceID) {
  analogWrite(RED,0);
  analogWrite(BLUE,0);
  analogWrite(GREEN,0);
}

void lightStatus() {
  Serial.print("Light is ");
  Serial.println(analogRead(GREEN));
}

void listLight() {
  Serial.print("Green is ");
  analogRead(GREEN) == 255 ? Serial.println("on") : Serial.println("off");
  Serial.print("Red is ");
  analogRead(RED) == 255 ? Serial.println("on") : Serial.println("off");
  Serial.print("Blue is ");
  analogRead(BLUE) == 255 ? Serial.println("on") : Serial.println("off");
}

void groupLight(uint8_t ressourceId) {
  switch (ressourceId) {
    case 0:
      analogWrite(GREEN,255);
      t.after(1000, lightOff);
    break;
    case 1:
      analogWrite(BLUE,255);
      t.after(1000, lightOff);
    break;
    case 2:
      analogWrite(RED,255);
      t.after(1000, lightOff);
    break;
    default:
    analogWrite(RED,127);
    analogWrite(BLUE,127);
    t.after(1000, lightOff);
    break;
  }
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
