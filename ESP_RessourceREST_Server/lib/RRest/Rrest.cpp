#include <Rrest.h>

uint8_t _nextRessource;
uint8_t _nextGroupRessource;

Rrest::Rrest() {


  _nextGroupRessource = _nextRessource = 0;
}

void Rrest::begin(ESP8266WebServer* _server) {
  _server = webserver;
}

bool Rrest::handleRest() {
  return false;
  //TODO
}

/*PARSERS*/
Callback Rrest::directParser() {
  //TODO
}

Callback Rrest::ressourceParser() {
  //TODO
}

Gcallback Rrest::groupParser() {
  //TODO
}

/*RESSOURCE MANAGEMENT FUNCTIONS*/
void addRessource(char* ressourceName) {
  s_ressource[_nextRessource].ressourceName = ressourceName;
  _nextRessource++;
}

void addRessourceGroup(char* ressourceName, uint8_t count = 0) {
  s_groupRessources[_nextGroupRessource].groupRessourceName = ressourceName;
  s_groupRessources[_nextGroupRessource].count = count;
  _nextGroupRessource++;
}

void addRessourceCallback(char* ressourceName, Callback funcPtr) {
  for (size_t i = 0; i < MAX_RESSOURCE_COUNT; i++) {
    if(s_ressource[i].ressourceName == ressourceName) {
      s_ressource[i].funcPtrCallback = funcPtr;
    }
  }
}

void addAction(char* ressourceName, char* action, Callback funcPtr) {
  //TODO
  //we'll need to loop into the actions either by taking the number of action already inside or by using a global variable like _nextRessource
}

void addAction(char* ressourceName, char* action, Gcallback funcPtr) {
  //TODO
}

void removeRessource(char* ressourceName) {
  //TODO
}

void removeRessourceGroup(char* ressourceName) {
  //TODO
}
