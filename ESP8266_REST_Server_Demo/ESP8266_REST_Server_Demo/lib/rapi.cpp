#include <rapi.h>

#define MAX_HTTP_METHODS 2  //Maximum HTTP Methods we want to support
#define MAX_URI_ACTIONS 3   //Maximum of actions a uri can have

uint8_t _LastresourceID;
String _LastAction;
uint8_t _numericChars;

//number of elements in the struct array
const uint8_t structElements = (sizeof(uri_storage) / sizeof(uri_storage[0]));

Rapi::Rapi() {
  //Header keys we will look for when receiving a request, Authorization is stored by default
  const char * headerKeys[] = {"Content-Type", "Allow"};
  const size_t headerKeysSize = sizeof(headerKeys) / sizeof(char*);
  server.collectHeaders(headerKeys, headerKeysSize); //Header values will then be accessible by server.header(headerKey);
}

String Rapi::parseURI(String uri) {
  //this function will parse the uri to detect the Arguments and replace them to have a generic forma
  uri.toLowerCase();
  char c_uri[uri.length()];
  //Convert uri string to char array
  strcpy(c_uri, uri.c_str());
  uint8_t c_uriLength = strlen(c_uri);

  for (size_t i = 0; i < c_uriLength; i++) {
    if (c_uri[i] == '/') {
      if (isdigit((int)c_uri[i+1]) == true) {
        uint8_t startSlashPos = i; //first '/' position before the numeric group
        uint8_t endSlashPos = i+1; //last '/' position

        //loop until the next '/' is found we also check wheter the chars we go trough are numeric.
        while(c_uri[endSlashPos] != '/' && isdigit((int)c_uri[endSlashPos]) == true) {
          if (endSlashPos > c_uriLength) { //handle that there may be no '/' after the numbers
          endSlashPos == c_uriLength;
        } else { endSlashPos++; }
      }
      _numericChars = (endSlashPos - startSlashPos) - 1;

      if (c_uri[endSlashPos] != '/' && isdigit((int)c_uri[endSlashPos]) == false) return "";
      _LastresourceID = uri.substring(startSlashPos+1, endSlashPos).toInt();
      uri.replace(uri.substring(startSlashPos+1, endSlashPos), "num");
    }
  }
}

/*
Action relacement block
we replace the last block with "action" and see if there is any correspondence in the uri_struct
*/
String tempUri = uri;
uint8_t lastSlash = uri.lastIndexOf('/');
String action = tempUri.substring(lastSlash+1);
tempUri.replace(tempUri.substring(lastSlash+1), "action"); //replace everything after lastSlash to "action"

//test if the uri +action at the end is present in uri_struct
uint8_t uriID = 255;
for (size_t y = 0; y < structElements; y++) {
  if (uri_storage[y].uri == tempUri) {
    uriID = y;
  }
}

//if uriID is still 255 it didn't find tempUri in uri_storage
if (uriID == 255) {
  return uri;
} else {
  //if we did find a uri that is in uri_struct we check every actions in the uri_struct actions array
  for (uint8_t i = 0; i < MAX_URI_ACTIONS; i++) {
    if (uri_storage[uriID].actions[i] == action) {
      _LastAction = action;
      return tempUri;
    }
  }
}
return uri;
}

bool Rapi::handleRest() {
  String parsed_uri = parseURI(server.uri());

  for (size_t i = 0; i < structElements; i++) {
    if (uri_storage[i].uri == parsed_uri) {  //if uri is in uri_storage
      for (int y = 0; y < MAX_HTTP_METHODS; y++) {
        if (uri_storage[i].methods[y] == server.method()) { //if specified uri supports method
          uri_storage[i].funcPtrCallback(_LastAction, _LastresourceID);
          return true;
        }
      }
    }
  }
  return false;
}

uint8_t Rapi::extractResourceID(String parsedUri, String uri) {
uint8_t a;
while(uri_storage[a].uri != parsedUri) {
  a++;
}

if (uri_storage[a].uri == parsedUri && uri_storage[a].hasResourceIdentifier) {
  uint8_t startSlash = uri_storage[a].pos;
  uint8_t endSlash;

  char c_uri[uri.length()];
  //Convert uri string to char array
  strcpy(c_uri, uri.c_str());
  uint8_t c_uriLength = strlen(c_uri);

  for (size_t i = uri_storage[a].pos+1; i < c_uriLength; i++) {
    if (c_uri[i] == '/') {
      endSlash = i;
    }
  }
  return uri.substring(startSlash, endSlash).toInt();
  } else {
    return 0;
  }
}

char* Rapi::listMethods() {

    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();

    for (size_t j = 0; j < structElements; j++) {
      root.add(uri_storage[j].uri);
      JsonObject& commands = root.createNestedObject();
      for (size_t i = 0; i < MAX_URI_ACTIONS; i++) {
        commands[String(i)] = uri_storage[j].actions[i];
      }
    }
    size_t t = root.prettyPrintTo(Serial);
    t += 1; //last char isn't included otherwise
    char* c = (char*) malloc(t);
    root.prettyPrintTo(c, t);
    return c;
}

String Rapi::getJsonArg(String arg) {
  if (server.header(F("Content-Type")) == F("application/json")) {
    char *json = (char *) malloc(server.arg("plain").length());
    server.arg("plain").toCharArray(json, *json);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& jobject = jsonBuffer.parseObject(json);

    if (jobject.containsKey(arg)) {
      return jobject[arg];
    } else {
      return F("No Json argument found");
    }
  } else {
    return F("Content-type mismatch");
  }
}

int Rapi::getIntJsonArg(String arg) {
  if (server.header(F("Content-Type")) == F("application/json")) {
    char *json = (char *) malloc(server.arg("plain").length());
    server.arg("plain").toCharArray(json, *json);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& jobject = jsonBuffer.parseObject(json);

    if (jobject.containsKey(arg)) {
      return jobject[arg];
    } else {
      return 0; //No Json argument found
    }
  } else {
    return 0; //Content type mismatch
  }
}
