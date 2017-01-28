/*
Base OTA Sketch for ESP8266
*/

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "1337 internet";
const char* password = "icanhasinternet";
const char* mMDNSName = "esp8266Base";

WiFiClient wclient;

void setup() {
  Serial.begin(115200);

  //Wifi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Error connecting to network");
    delay(2000);
    WiFi.begin(ssid,password);
  }
  Serial.println("");
  Serial.print("Connected to:");
  Serial.println(ssid);
  Serial.print("On address: ");
  Serial.println(WiFi.localIP());

  //MDNS setup
  while(!MDNS.begin(mMDNSName)){
    Serial.println("Error setting up MDNS responder");
  }

/*
  Arduino OTA methods
*/
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating ");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("Ready");
}

void loop() {
  ArduinoOTA.handle();
}
