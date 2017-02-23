#ifndef Rrest_h
#define Rrest_h
#ifdef ESP8266
#include <pgmspace.h>
#endif

#include <ESP8266WebServer.h>
#define MAX_URI_ACTIONS 3
#define MAX_RESSOURCE_COUNT 5

#define ARRCOUNT(a) = (sizeof(a) / sizeof(*a));

extern ESP8266WebServer server;
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

enum RessourceType { GROUPED, SIMPLE };

struct Action {
  char* name = 0;   //init to default value to allow to be checked if empty
  Callback funcptr;
  Gcallback grpFuncptr;
};

typedef struct ressource {
  char* ressourceName;
  Action actions[MAX_URI_ACTIONS];
  Callback funcPtrCallback = NULL;
  Callback defaultCallback = NULL;
}ressource;

typedef struct groupRessources {
  char* groupRessourceName;
  Action actions[MAX_URI_ACTIONS];
  uint8_t count = 0;                  //number of ressources (ex if we want to control 2 lights count will be 2)
  Callback defaultCallback = NULL;
  Gcallback groupPtrCallback = NULL;
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

  /*RESSOURCE MANAGEMENT FUNCTIONS*/

  /*
  Add a simple ressource
  example: http://esp.local/light is a ressource
  */
  void addRessource(char* ressourceName);

  /*
  Add a grouped ressource
  example: http://esp.local/light/1/ is a group resssource with id 1
  */
  void addRessourceGroup(char* ressourceName, uint8_t count);

  /*
  add a default callback for the ressource
  example on ressource light:
  http://esp.local/light will be the default callback (usually read)
  ressourceType = GROUPED or SIMPLE
  */
  void addRessourceCallback(char* ressourceName, Callback funcPtr, RessourceType t);

  /*
  Add action to simple ressource
  example http://esp.local/light/on is an action
  */
  void addAction(char* ressourceName, char* action, Callback funcPtr);  //direct ressource

  /*
  Add action to group ressource
  example http://esp.local/light/1/on is an action on a grouped ressource
  */
  void addGroupAction(char* ressourceName, char* action, Gcallback funcPtr); //grouped ressource

  /*
  Removes simple ressource
  sets the name to 0 and deletes all callbacks + actions
  */
  void removeRessource(char* ressourceName);

  /*
  Removes grouped ressource
  sets the name to 0 and deletes all callbacks + actions
  */
  void removeRessourceGroup(char* ressourceName);

private:
  /*Ressource locator
  returns 255 if the ressource doesn't exists
  */
  uint8_t locateRessource(char* ressourceName, struct ressource* st);
  uint8_t locateRessource(char* ressourceName, struct groupRessources* st);

  /*PARSERS*/
  Callback directParser();
  Callback ressourceParser();
  Gcallback groupParser();

};

#endif
