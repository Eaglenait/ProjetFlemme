#include <Rrest.h>

Rrest::Rrest() {}

/*TODO debug design and finish
  program enters here
  store uri
  call parser with chopped uri
*/
bool Rrest::handleRest() {
  char* _uri = new char[server.uri().length() + 1];
  strcpy(_uri, server.uri().c_str());
  uint8_t uriLength = strlen(_uri);

  Serial.println("");
  Serial.print("Base uri: ");
  Serial.print(_uri);
  Serial.print(" lenght ");
  Serial.println(uriLength);

  uint8_t blocks = 0;
  uint8_t blocksCharLenght[5];

  //first scan: count the number of valid blocks
  for (size_t i = 0; i < uriLength; ++i) {
    if(_uri[i] == '/' && i != uriLength-1){
      if(isalpha(_uri[i+1]) || isdigit(_uri[i+1])){
        uint8_t j = i+1;
        while (_uri[j] != '/' && j != uriLength) {
          blocksCharLenght[blocks]++;
          ++j;
        }
        ++blocks;
      }
    }
  }

  //debug
  Serial.print("Blocks: ");
  Serial.println(blocks);
  Serial.println("blocksCharLenght: ");
  for (size_t cc = 0; cc < sizeof(blocksCharLenght); ++cc) {
    Serial.print(" block ");
    Serial.print(cc);
    Serial.print(": lenght: ");
    Serial.println(blocksCharLenght[cc]);
  }

  char *tmp[blocks]; //TODO changer le mode de stockage ou apprendre à accéder aux char individuels de cette variable

  Serial.println("explode: ");

  uint8_t count = 1; //starts at one to omit first '/' char

  for (size_t k = 0; k <= blocks; ++k) {
    size_t l = 0;
    if(blocksCharLenght[k] != 0) {

      //debug
      Serial.print(" Block ");
      Serial.println(k);
      Serial.print(" lenght ");
      Serial.print(blocksCharLenght[k]);
      Serial.print(" ");

      char* buffer = new char[blocksCharLenght[k]];

      for (l = 0; l < blocksCharLenght[k]; ++l) {
        buffer[l] = _uri[count];
        Serial.print(_uri[count]);
        count++;
      }
      *tmp[blocks] = *buffer;
      buffer = 0;
      Serial.println("");
      count++;
    }
  }

  //debug
  Serial.println("debug tmp: ");
  for (size_t i = 0; i < blocks; i++) {
    Serial.print(" ");
    Serial.println(*tmp[i]);
  }

  directParser(); //modify to call with uri
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

void Rrest::addRessource(char* ressourceName) {
  uint8_t ressourceSlot = 0; //empty ressource slot that we can use to create the ressource

  //if ressource already exists stop the function
  if(locateRessource(ressourceName, s_ressource) != 255) return;

  //check for already used ressource slots end function if none is left
  while (s_ressource[ressourceSlot].ressourceName != 0) {
    ressourceSlot++;
    if(ressourceSlot > MAX_RESSOURCE_COUNT) return;
  }
  //assign
  s_ressource[ressourceSlot].ressourceName = ressourceName;

  //debug
  Serial.print("Ressource ");
  Serial.print(ressourceName);
  Serial.print(" assigned to slot ");
  Serial.println(ressourceSlot);
}

void Rrest::addRessourceGroup(char* ressourceName, uint8_t count) {
  uint8_t ressourceSlot = 0;

  //check if ressource already exists
  if(locateRessource(ressourceName, s_groupRessources) != 255) return;

  Serial.println("Ressource Located");

  //check for already used ressource slots end function if none is left
  while (s_groupRessources[ressourceSlot].groupRessourceName != 0) {
    ressourceSlot++;
    if(ressourceSlot > MAX_RESSOURCE_COUNT) return;
  }

  //assign
  s_groupRessources[ressourceSlot].groupRessourceName = ressourceName;
  s_groupRessources[ressourceSlot].count = count;

  Serial.print("group ressource created on slot :");
  Serial.println(ressourceSlot);
}

//TODO Test
void Rrest::addRessourceCallback(char* ressourceName, Callback funcPtr, RessourceType t) {
  if(t = GROUPED) {
    uint8_t pos = locateRessource(ressourceName, s_groupRessources);
    if(pos != 255) {
      s_groupRessources[pos].defaultCallback = funcPtr;
    } else {
      Serial.println("addRessourceCallback: Ressource Could not be located");
    }
  }

  if(t = SIMPLE) {
    uint8_t pos = locateRessource(ressourceName, s_ressource);
    if(pos != 255) {
      s_ressource[pos].defaultCallback = funcPtr;
    }else{
      Serial.println("addRessourceCallback: Ressource Could not be located");
    }
  }
}

void Rrest::addAction(char* ressourceName, char* action, Callback funcPtr) {
  uint8_t pos = locateRessource(ressourceName, s_ressource);

  if(pos != 255) {
    uint8_t actionPos = 255;

    //locate empty action slot
    for (size_t i = 0; i < MAX_URI_ACTIONS; ++i) {
      if(s_ressource[pos].actions[i].name == 0) {
        actionPos = i;
        i = MAX_URI_ACTIONS; //get out of the for loop
      }
    }
    if(actionPos == 255) return;

    //assign
    s_ressource[pos].actions[actionPos].funcptr = funcPtr;
    s_ressource[pos].actions[actionPos].grpFuncptr = NULL;

    Serial.println("action assigned");

  } else {
    Serial.println("addAction: Ressource not located");
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

uint8_t Rrest::locateRessource(char* ressourceName,struct ressource* st) {
  uint8_t res = 0;
  while (st[res].ressourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  return res;
}

uint8_t Rrest::locateRessource(char* ressourceName,struct groupRessources* st) {
  uint8_t res = 0;
  while (st[res].groupRessourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  return res;
}
