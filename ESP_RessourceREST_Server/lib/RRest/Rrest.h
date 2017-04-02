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
extern void lightOn();
extern void lightOff();

extern void lightOn(uint8_t ressourceID);
extern void lightOff(uint8_t ressourceID);

extern void lightStatus();
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

enum RessourceType { GROUPED, SIMPLE, NONE };

typedef struct Action {
  const char* name;
  Callback actionCallback;
}Action;

typedef struct gAction {
  const char* name;
  Gcallback actionCallback;
}gAction;

typedef struct callbackType {
  uint8_t callbackPos;
  uint8_t actionPos;
  RessourceType type;
}callbackType;

typedef struct gCallbackType {
  uint8_t callbackPos;
  uint8_t actionPos;
  uint8_t ressourceID;
  RessourceType type;
}gCallbackType;


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

/*
Empty actions has to be initialized at {'\0',NULL}
*/
static const struct ressource s_ressource[] = {
  {"light", {{"on", lightOn},{"off", lightOff},{"\0",NULL}}, lightStatus}
};

static const struct groupRessource s_groupRessource[] = {
  {"lights", {{"on",lightOn},{"off", lightOff},{'\0',NULL}},2,listLight}
};

class Rrest {
public:

  const size_t ressourceElements = (sizeof(s_ressource)/(sizeof(*s_ressource)));
  const size_t groupRessourceElements = (sizeof(s_groupRessource)/(sizeof(*s_groupRessource)));
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
  uint8_t locateRessource(char* ressourceName, const struct ressource* st);
  uint8_t locateRessource(char* ressourceName, const struct groupRessource* st);

  /*PARSERS*/
  callbackType directParser(char** blockStorage, uint8_t blockSize);
  callbackType ressourceParser(char** blockStorage, uint8_t blockSize);
  gCallbackType groupParser(char** blockStorage, uint8_t blockSize);
};
#endif
