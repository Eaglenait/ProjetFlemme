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
  if(_nextRessource != MAX_RESSOURCE_COUNT) {
    s_ressource[_nextRessource].ressourceName = ressourceName;
    _nextRessource++;
  } else {
    Serial.println("ERROR: Max ressource count reached");
  }
}

void addRessourceGroup(char* ressourceName, uint8_t count = 0) {
  if(_nextGroupRessource <= MAX_RESSOURCE_COUNT) {
    s_groupRessources[_nextGroupRessource].groupRessourceName = ressourceName;
    s_groupRessources[_nextGroupRessource].count = count;
    _nextGroupRessource++;
  } else {
    Serial.println("ERROR: Max ressource count reached");
  }
}

void addRessourceCallback(char* ressourceName, Callback funcPtr) {
  //search for the ressource
  for (size_t i = 0; i < MAX_RESSOURCE_COUNT; i++) {
    if(s_ressource[i].ressourceName == ressourceName) {
      //assign the callback when found
      s_ressource[i].funcPtrCallback = funcPtr;
    }
  }
}

//broken
//search for ressrouce, then search the number of action and append
void addAction(char* ressourceName, char* action, Callback funcPtr) {
  uint8_t ressourcePos = 0;
  uint8_t actionCount;

  //search for the ressource
  for (size_t i = 0; i < MAX_RESSOURCE_COUNT; i++) {
    if(s_ressource[i].ressourceName == ressourceName) {
      ressourcePos = i;
    }
  }
  if()
}

void addGroupAction(char* ressourceName, char* action, Gcallback funcPtr) {
  //TODO
}

void removeRessource(char* ressourceName) {
  //TODO
}

void removeRessourceGroup(char* ressourceName) {
  //TODO
}
