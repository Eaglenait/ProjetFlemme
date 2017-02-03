#include <Rrest.h>

Rrest::Rrest() {}

void Rrest::begin(ESP8266WebServer* server) {
  server = _webserver;
}

/*TODO debug design and finish
  program enters here

  store uri
  call parser with chopped uri
*/
bool Rrest::handleRest() {
  char* _uri = new char[_webserver->uri().length() + 1];
  strcpy(_uri, _webserver->uri().c_str());

  uint8_t blocks, nextBlock = 0;
  uint8_t uriLength = strlen(_uri);
  //first pass count each valid blocks (block with more than a char in it)
  for (size_t i = 0; i < uriLength; ++i) {
    if(_uri[i] == '/'){
      if(isdigit(_uri[++i]) || isalpha(_uri[++i])) ++blocks;
    }
  }

  char* tmp[blocks];

  //second pass store uri blocks in char*[]
  for (size_t j = 0; j < uriLength; ++j) {
    if(_uri[j] == '/') {
      uint8_t cnt = j+1;          //+1 to omit '/'
      uint8_t c = 0;
      while (_uri[++j] != '/') {
        tmp[c][nextBlock] = _uri[cnt];
        Serial.print(_uri[cnt]);
        ++cnt;
      }
    }
    ++nextBlock;
  }

  return false;

}

/*PARSERS*/
//TODO
Callback Rrest::directParser() {

}

//TODO
Callback Rrest::ressourceParser() {

}

//TODO
Gcallback Rrest::groupParser() {

}

/*RESSOURCE MANAGEMENT FUNCTIONS*/

//TODO Test
void Rrest::addRessource(char* ressourceName) {
  uint8_t ressourceSlot = 0; //empty ressource slot that we can use to create the ressource

  //Check if ressource already exists
  if(locateRessource(ressourceName, s_ressource) == 255) return;

  //check for already used ressource slots end function if none is left
  while (s_ressource[ressourceSlot].ressourceName != 0) {
    ressourceSlot++;
    if(ressourceSlot > MAX_RESSOURCE_COUNT) return;
  }

  //assign
  s_ressource[ressourceSlot].ressourceName = ressourceName;
}

//TODO Test
void Rrest::addRessourceGroup(char* ressourceName, uint8_t count) {
  uint8_t ressourceSlot = 0;

  //check if ressource already exists
  if(locateRessource(ressourceName, s_groupRessources) == 255) return;

  //check for already used ressource slots end function if none is left
  while (s_groupRessources[ressourceSlot].groupRessourceName != 0) {
    ressourceSlot++;
    if(ressourceSlot > MAX_RESSOURCE_COUNT) return;
  }

  //assign
  s_groupRessources[ressourceSlot].groupRessourceName = ressourceName;
  s_groupRessources[ressourceSlot].count = count;
}

//TODO Test
void Rrest::addRessourceCallback(char* ressourceName, Callback funcPtr, RessourceType t) {
  if(t = GROUPED) {
    uint8_t pos = locateRessource(ressourceName, s_groupRessources);
    if(pos != 255) {
      s_groupRessources[pos].defaultCallback = funcPtr;
    }
  }

  if(t = SIMPLE) {
    uint8_t pos = locateRessource(ressourceName, s_ressource);
    if(pos != 255) {
      s_ressource[pos].defaultCallback = funcPtr;
    }
  }
}

//TODO Test
void Rrest::addAction(char* ressourceName, char* action, Callback funcPtr) {
  uint8_t pos = locateRessource(ressourceName, s_ressource);
  if(pos != 255) {
    uint8_t actionPos = 0;

    //locate empty action slot
    while (s_ressource[pos].actions[actionPos].name != 0) {
      ++actionPos;
      if(actionPos > MAX_URI_ACTIONS) return;
    }

    //assign
    s_ressource[pos].actions[actionPos].funcptr = funcPtr;
    s_ressource[pos].actions[actionPos].grpFuncptr = NULL;
  }
}

//TODO Test
void Rrest::addGroupAction(char* ressourceName, char* action, Gcallback funcPtr) {
  uint8_t pos = locateRessource(ressourceName, s_groupRessources);
  if(pos != 255) {
    uint8_t actionPos = 0;

    //locate empty action slot
    while (s_groupRessources[pos].actions[actionPos].name != 0) {
      ++actionPos;
      if(actionPos > MAX_URI_ACTIONS) return;
    }

    //assign
    s_groupRessources[pos].actions[actionPos].funcptr = NULL;
    s_groupRessources[pos].actions[actionPos].grpFuncptr = funcPtr;
  }
}

//TODO Test
void Rrest::removeRessource(char* ressourceName) {
    uint8_t pos = locateRessource(ressourceName, s_ressource);
    if(pos != 255) {
      s_ressource[pos].ressourceName = 0;
      s_ressource[pos].funcPtrCallback = NULL;
      s_ressource[pos].defaultCallback = NULL;
      for (size_t i = 0; i < MAX_URI_ACTIONS; ++i) {
        s_ressource[pos].actions[i].name = 0;
        s_ressource[pos].actions[i].funcptr = NULL;
        s_ressource[pos].actions[i].grpFuncptr = NULL;
      }
    }
}

//TODO Test
void Rrest::removeRessourceGroup(char* ressourceName) {
  uint8_t pos = locateRessource(ressourceName, s_groupRessources);
  if(pos != 255) {
    s_groupRessources[pos].groupRessourceName = 0;
    s_groupRessources[pos].count = 0;
    s_groupRessources[pos].groupPtrCallback = NULL;
    s_groupRessources[pos].defaultCallback = NULL;
    for (size_t i = 0; i < MAX_URI_ACTIONS; ++i) {
      s_groupRessources[pos].actions[i].name = 0;
      s_groupRessources[pos].actions[i].funcptr = NULL;
      s_groupRessources[pos].actions[i].grpFuncptr = NULL;
    }
  }
}

//TODO Test
uint8_t Rrest::locateRessource(char* ressourceName,struct ressource* st) {
  uint8_t res = 0;
  while (st[res].ressourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  return res;
}

//TODO Test
uint8_t Rrest::locateRessource(char* ressourceName,struct groupRessources* st) {
  uint8_t res = 0;
  while (st[res].groupRessourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  return res;
}
