#include <Rrest.h>

Rrest::Rrest() { }

void Rrest::begin(ESP8266WebServer* _server) {
  _server = webserver;
}

void Rrest::handleRest() {

}

/*PARSERS*/
void Rrest::directParser() {

}

void Rrest::ressourceParser() {

}

void Rrest::groupParser() {

}

/*RESSOURCE MANAGEMENT FUNCTIONS*/
