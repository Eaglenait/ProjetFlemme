#ifndef Rrest_h
#define Rrest_h

#ifdef ESP8266
#include <pgmspace.h>
#endif

#include <ESP8266WebServer.h>
#define MAX_URI_ACTIONS 3
#define MAX_RESSOURCE_COUNT 5
#define ARRCOUNT(a) (sizeof(a) / sizeof(*a))

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

typedef void (*Callback)();
typedef void (*Gcallback)(uint8_t ressourceID);

struct Action {
  char* name;
  Callback funcptr;
};

typedef struct ressource {
  char* ressourceName;
  Action actions[MAX_URI_ACTIONS];
  Callback funcPtrCallback;
  Callback defaultCallback;
}ressource;

typedef struct groupRessources {
  char* groupRessourceName;
  Action actions[MAX_URI_ACTIONS];
  uint8_t count;                  //number of ressources (ex if we want to control 2 lights count will be 2)
  Callback defaultCallback;
  Gcallback groupPtrCallback;
}groupRessources;

static struct ressource s_ressource[MAX_RESSOURCE_COUNT];
static struct groupRessources s_groupRessources[MAX_RESSOURCE_COUNT];

class Rrest {
public:
  //default constructor
  Rrest();

  //called periodically to check if an url has been sent
  //we could make a better system for this to be triggered when a client sends a request
  bool handleRest();

  //Setup phase
  void begin(ESP8266WebServer* _server);

  /*RESSOURCE MANAGEMENT FUNCTIONS*/
  void addRessource(char* ressourceName);
  void addRessourceGroup(char* ressourceName, uint8_t count = 0);

  void addRessourceCallback(char* ressourceName, Callback funcPtr); //callbakc for base ressource; typically read

  void addAction(char* ressourceName, char* action, Callback funcPtr);  //direct ressource
  void addGroupAction(char* ressourceName, char* action, Gcallback funcPtr); //grouped ressource

  void removeRessource(char* ressourceName);
  void removeRessourceGroup(char* ressourceName);

private:
  ESP8266WebServer *webserver;

  /*PARSERS*/
  Callback directParser();
  Callback ressourceParser();
  Gcallback groupParser();

};

#endif
