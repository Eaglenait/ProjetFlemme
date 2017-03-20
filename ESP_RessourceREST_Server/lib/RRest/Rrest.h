#ifndef Rrest_h
#define Rrest_h
#ifdef ESP8266
#include <pgmspace.h>
#endif

#include <ESP8266WebServer.h>

#define ARRCOUNT(a) = (sizeof(a) / sizeof(*a));

#define MAX_URI_ACTIONS 3
#define MAX_RESSOURCE_COUNT 5

extern ESP8266WebServer server;
extern void cycleLight();
extern void listLight();
extern void groupLight(uint8_t ressourceId);
/*
-Ressource based REST interface
TODO
-ajouter une méthode qui va appeller les fonctions du main
parsers
-ressource parsing
-group parsing
addRessource & addRessourceGroup
AddRessourceCallback
defaultCallback

Stockage des ressources
*/

typedef void (*Callback)();
typedef void (*Gcallback)(uint8_t ressourceID);

enum RessourceType { GROUPED, SIMPLE };

typedef struct Action {
  const char* name;
  Callback actionCallback;
}Action;

typedef struct gAction {
  const char* name;
  Gcallback actionCallback;
}gAction;

typedef struct ressource {
  const char* ressourceName;
  Action actions[MAX_URI_ACTIONS];
  Callback defaultCallback;
}ressource;

typedef struct groupRessource {
  const char* ressourceName;
  gAction actions[MAX_URI_ACTIONS];
  uint8_t count;
  Callback defaultCallback;
}groupressource;

static const struct ressource s_ressource[] = {
  {"light", {{"on", cycleLight},{"off", cycleLight}}, listLight}
};

static const struct groupRessource s_groupRessource[] = {
  {"lights", {{"on",groupLight},{"off", groupLight}},2,listLight}
};

class Rrest {
public:

  //default constructor
  Rrest();

  //called periodically to check if an url has been sent
  //we could make a better system for this to be triggered when a client sends a request
  bool handleRest();

private:
  /*
  String extractor
  will extract a c string from src from char s to char e
  it has absolutly no safty it will fuck up your code if not used with care (overflow etc)
  */
  char* strExtr(char* src, uint8_t s, uint8_t e);

  /*
  Ressource locator
  returns 255 if the ressource doesn't exists
  */
  uint8_t locateRessource(char* ressourceName, struct ressource* st);
  uint8_t locateRessource(char* ressourceName, struct groupRessource* st);

  /*PARSERS*/
  Callback directParser(char** blockStorage, uint8_t blockSize);
  Callback ressourceParser(char** blockStorage, uint8_t blockSize);
  Gcallback groupParser(char** blockStorage);
};
#endif
