#ifndef Rrest_h
#define Rrest_h

#ifdef ESP8266
#include <pgmspace.h>
#endif

#include <ESP8266WebServer.h>

/*
-Ressource based REST interface
TODO
parsers
-direct parsing
-ressource parsing
-group parsing
addRessource & addRessourceGroup
AddRessourceCallback
addAction
defaultCallback
handleRest

Stockage des ressources
*/

class Rrest {
public:
  //default constructor
  Rrest();

  //called periodically to check if an url has been sent
  //we could make a better system for this to be triggered when a client sends a request
  void handleRest();

  //Setup phase
  void begin(ESP8266WebServer* _server);

  /*RESSOURCE MANAGEMENT FUNCTIONS*/
  void addRessource(char* ressourceName);
  void addRessourceGroup(char* ressourceName, uint8_t count = 0);

  void addRessourceCallback(char* ressourceName, void (*Callback)());

  void addAction(char* ressourceName, char* action, void (*Callback)());
  void addAction(char* ressourceName, char* action, void (*Callback)(uint8_t ressourceID));

  void defaultCallback(void (*Callback)());

private:
  ESP8266WebServer *webserver;

  /*PARSERS*/
  void directParser();
  void ressourceParser();
  void groupParser();

};

#endif
