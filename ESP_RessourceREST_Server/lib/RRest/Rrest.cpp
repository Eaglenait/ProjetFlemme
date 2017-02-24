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

  //debug: show the uri and its lenght
  // Serial.print("URI: ");
  // Serial.print(_uri);
  // Serial.print(" lenght: ");
  // Serial.println(uriLength);

  //count number of blocks
   uint8_t b = 0; //number of blocks
   for (size_t i = 0; i < uriLength; i++) {
     if(_uri[i] == '/' && i < uriLength-1) {
       if(isdigit(_uri[i+1]) || isalpha(_uri[i+1])) {
         ++b;
       }
     }
   }
   //get position of blocks
   uint8_t bPos[b], c = 0;
   for (size_t j = 0; j < uriLength; j++) {
     if(_uri[j] == '/' && j < uriLength-1) {
       if(isdigit(_uri[j+1]) || isalpha(_uri[j+1])){
        bPos[c++] = j;
       }
     }
   }

  //debug : show number of blocks position of each block
  //  Serial.print("number of blocks: ");
  //  Serial.println(b);
  //  Serial.print("block pos: ");
  //  for (size_t u = 0; u < b; ++u) {
  //    Serial.print(bPos[u]);
  //    if(u != b-1) {
  //      Serial.print(',');
  //    }
  //  }

   //separate each uri block into blockStorage
   char *blockStorage[b];
   c = sizeof(bPos);//powerful naming convention
   for (size_t k = 0; k < b; ++k) {
     if(k == c-1){
       blockStorage[k] = strExtr(_uri, bPos[k]+1, uriLength);
     } else {
       blockStorage[k] = strExtr(_uri,bPos[k]+1,bPos[k+1]); //garbage coding but it works
     }
   }

  //debug : shows the content of blockStorage
  //  Serial.println("");
  //  Serial.println("blockStorage");
  //  for (size_t e = 0; e < b; ++e) {
  //    Serial.print(e);
  //    Serial.print(" : ");
  //    Serial.println(blockStorage[e]);
  //  }

  return false; //default return until we call the parsers
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

char* Rrest::strExtr(char* src, uint8_t s, uint8_t e) {
  uint8_t len = e-s;

  //debug
  // Serial.println("");
  // Serial.println("Starting extraction with ");
  // Serial.print(s);
  // Serial.print(" - ");
  // Serial.print(e);
  // Serial.print(" len ");
  // Serial.print(len);
  // Serial.println("");

  if(len == 0) {
    Serial.println("len = 0");
    return NULL;
  }
  uint8_t bufLen = 0;
  char* buffer = new char[len+1];
  for (size_t i = 0; i < len; ++i) {
    // Serial.print(src[s]); //also debug
    buffer[i] = src[s++];
  }
  buffer[len] = '\0';
  return buffer;
}
