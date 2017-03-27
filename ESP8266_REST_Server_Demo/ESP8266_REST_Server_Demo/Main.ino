/*
  Demo project for rapi library

  consult rapi.h for more information
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Fs.h>
#include <rapi.h>
#include <Timer.h>

// const char* ssid = "ActiStaff";
// const char* password = "Act1K3hl";
// const char* ssid = "1337 internet";
// const char* password = "icanhasinternet";
const char* ssid = "SebastianAP";
const char* password = "exiacesi2017";

MDNSResponder mdns;
Timer t;

WiFiClient wclient;
ESP8266WebServer server(80);

Rapi rest;

const int GREEN   = 12;
const int BLUE    = 13;
const int BUTTON  = 4;
const int RELAY   = 14;
const int RED     = 15;

//handles the website icon requests
void handleico() {
  server.send(404);
}

void handleNotFound() {
  if(!rest.handleRest()) { //we call rest when the webserver doesn't recognize the entered URI
    File notfound = SPIFFS.open("/notfound.html", "r");
    size_t sent = server.streamFile(notfound,"text/html");
    notfound.close();
  } else {
    server.send(200);
  }
}

//Will list every uri and action that are supported by Rapi in Json
void list(String action, uint8_t RessourceID) {
  server.send(200, "application/json", rest.listMethods());
}

//Will cycle the relay and respond with the relay status in JSON
void doorCycle(String action, uint8_t RessourceID) {
  analogWrite(GREEN,255);
  StaticJsonBuffer<25> jsonBuffer;              //create a buffer for the json response
  JsonObject& root = jsonBuffer.createObject(); //create an instance of the jsonobject
  root["relay"] = digitalRead(RELAY);           //add a variable
  String c;
  root.prettyPrintTo(c);                        //convert and format the json into a string
  server.send(200, "application/json", c);      //send json response
}

/*
REST Handler for the /door/num/action URI
Handles the open, close, state actions
open : opens the relay for 2.5 sec (can be changed via a json argument)
close: forces the relay to close
state: displays the relays current state
*/
void doorAction(String action, uint8_t resourceID) {
  int openDelay = 2500;

  if(server.method() == HTTP_POST){
  openDelay = rest.getIntJsonArg("openDelay"); //will search for "openDelay" and return its value in int
  if (openDelay == 0) openDelay = 2500;

  }

  analogWrite(GREEN,255);
  String s = "Relay is ";
  if (resourceID == 1) {
    if (action == "open") {
      digitalWrite(RELAY, 1);
      t.after(openDelay, relayOff);
    }
    if (action == "close") {
      digitalWrite(RELAY, 0);
      relayOff();
    }
    s += digitalRead(RELAY) == 0 ? "off" : "on ";
  } else {
    s += "not setup";
  }

  server.send(200, "text/plain", s);
}

void setup(){
  Serial.begin(115200);

  pinMode(RED,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(RELAY,OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

    if(!SPIFFS.begin()){
    Serial.println("Error beginning SPIFFS");
  }

  if (mdns.begin("esp", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  mdns.addService("door", "tcp", 80);
  mdns.addService("rest", "tcp", 80);


  server.on("/", handleNotFound);
  server.on("/favicon.ico", handleico);
  server.onNotFound(handleNotFound);

  //HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  t.update();

  //Wifi reconnection loop
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Connecting to "));
    Serial.print(ssid);
    Serial.println(F("..."));
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    return;
    Serial.println(F("WiFi connected"));
    Serial.println(WiFi.localIP());
  }
  server.handleClient();

    if(!digitalRead(BUTTON)) {
    digitalWrite(RELAY, 1);

    analogWrite(GREEN,255);
    analogWrite(RED,0);
    analogWrite(BLUE,0);

    t.after(2000, relayOff);
  }
}

void relayOff() {
  digitalWrite(RELAY,0);
  analogWrite(GREEN,0);
}
