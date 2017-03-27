#ifndef Rapi_h
#define Rapi_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <Fs.h>
#include <ArduinoJson.h>

#ifdef ESP8266
#include <pgmspace.h>
#endif

#define MAX_HTTP_METHODS 2  //Maximum HTTP Methods we want to support
#define MAX_URI_ACTIONS  3  //Maximum of actions a uri can have

extern ESP8266WebServer server;
extern void list(String action, uint8_t resourceID);
extern void doorCycle(String action, uint8_t resourceID);
extern void doorAction(String action, uint8_t resourceID);

/*URI_STRUCT MEMBERS
  String uri -> generic uri we will try to find with the parser
  static void (*funcPtrCallback)(String action, uint8_t resourceID) -> function that will be called if the uri is parsed.
      ->String action: last part of the uri that tell the action to do (ex: door/1/open -> action = open)
      ->uint8_t resourceID: numerical identifier that identifies the resource we want to use  (ex: door/1/open -> resourceID = 1)
  bool hasresourcesIdentifier -> if the uri has a resourceID in it
  HTTPMethod methods -> array of allowed http methods to call the uri with
  String actions -> array of allowed action to use on the uri
*/

typedef void (*Callback)(String action, uint8_t resourceID);
typedef struct uri_struct {
  String uri;
  Callback funcPtrCallback;
  bool hasResourceIdentifier;
  uint8_t pos;
  HTTPMethod methods[MAX_HTTP_METHODS];
  String actions[MAX_URI_ACTIONS];
}uri_struct;

/*
  Storage for the parsable URIs
  Incoming uris will be parsed in lower case
  Use /num/ to represent a resource identifier (door/1/open -> /door/num/open) num is stored in a uint8_t so 256 is the max value
  Use /action at the end of the uri to represent any action you want to add (door/num/open || door/num/close -> /door/num/action). max numbers of action is defined by MAX_URI_ACTIONS
  Using two num in a row will not work
  Using action anywhere but at the end of the uri won't work
*/
static const struct uri_struct uri_storage[] = {
  {"/list",            list,       false, 0, {HTTP_GET},             {}},
  {"/door/action",     doorCycle,  false, 0, {HTTP_GET},             {"cycle"}},
  {"/door/num/action", doorAction, true,  7, {HTTP_POST,HTTP_GET},   {"open","close","state"}}
};

class Rapi {
public:
  Rapi();

  /*
    Starting point of the lib
    Will check that the parsedURI is in uri_struct
    Will check if the parsedURI supports the called method
    calls the callback function
    return true if we find anything to callback, false if not
  */
  bool handleRest();

  /*
  Use part of ParseURI that extracts the resource id from any uri
  parsedUri is the generic uri (ex: /door/num/open)
  */
  uint8_t extractResourceID(String parsedUri, String uri);

  /*
  Return the list of uris you can use and their actions if they have any
  Format:
    method
    method -> action -> action
    method
  */
  char* listMethods();

  /*
    Allow to get an argument from the body of a post or put method when sent in json
  */
  String getJsonArg(String arg);
  int getIntJsonArg(String arg);

  private:
  /*
    This function will parse the incoming uri and return a generic URI.
    In this function we also set the values for getLastAction() and getLastResourceID()
  */
  String parseURI(String uri);

};
#endif
