#include <Rrest.h>

Rrest::Rrest() {
  static struct ressource s_ressource;
  static struct groupRessources s_groupRessources;
}

void Rrest::begin(ESP8266WebServer* _server) {
  _server = webserver;
}

bool Rrest::handleRest() {
  return false;
}

/*PARSERS*/
void Rrest::directParser() {

}

void Rrest::ressourceParser() {

}

void Rrest::groupParser() {

}

/*RESSOURCE MANAGEMENT FUNCTIONS*/
