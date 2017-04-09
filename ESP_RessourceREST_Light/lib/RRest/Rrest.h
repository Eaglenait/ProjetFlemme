#ifndef Rrest_h
#define Rrest_h
#ifdef ESP8266
#include <pgmspace.h>
#endif

#include <ESP8266WebServer.h>

#define MAX_URI_ACTIONS 3
#define MAX_RESSOURCE_COUNT 5

extern ESP8266WebServer server;
extern void lightOn();
extern void lightOff();

extern void lightStatus();

typedef void (*Callback)();

enum RessourceType { GROUPED, SIMPLE, NONE };

typedef struct Action {
  const char* name;
  Callback actionCallback;
}Action;

typedef struct callbackType {
  uint8_t callbackPos;
  uint8_t actionPos;
  RessourceType type;
}callbackType;

typedef struct ressource {
  const char* ressourceName;
  Action actions[MAX_URI_ACTIONS];
  Callback defaultCallback;
}ressource;

/*
Empty actions has to be initialized at {'\0',NULL}
*/
static const struct ressource s_ressource[] = {
  {"light", {{"on", lightOn},{"off", lightOff},{"\0",NULL}}, lightStatus}
};

class Rrest {
public:

  const size_t ressourceElements = (sizeof(s_ressource)/(sizeof(*s_ressource)));

  /*
  to be called when an uri needs to be parsed
  handleRest will do the basic task of separating the uri into blocks that can be parsed with the 3 parser functions
  it returns true when the uri is parsable (and calls the function callback)
  */
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

  /*PARSERS*/
  callbackType directParser(char** blockStorage, uint8_t blockSize);
  callbackType ressourceParser(char** blockStorage, uint8_t blockSize);
  };
#endif
