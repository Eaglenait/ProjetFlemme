#include <Rrest.h>

Rrest::Rrest() {}

//TODO Support uri argument ex:/light/on?arg=2
//ISSUE:
/*
  Parfois quand on envoie un uri avec qu'un seul argument le / de fin est quand même inclus
*/
//TODO Test

bool Rrest::handleRest() {
  char* _uri = new char[server.uri().length() + 1];
  strcpy(_uri, server.uri().c_str());
  uint8_t uriLength = strlen(_uri);

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

   //separate each uri block into blockStorage
   char *blockStorage[b];
   c = sizeof(bPos);
   for (size_t k = 0; k < b; ++k) {
     if(k == c-1){
       blockStorage[k] = strExtr(_uri, bPos[k]+1, uriLength);
     } else {
       blockStorage[k] = strExtr(_uri,bPos[k]+1,bPos[k+1]);
     }
   }

  callbackType cbt = directParser(blockStorage, b);
  if(cbt.callbackPos != 255) {
    if (cbt.type == SIMPLE) {
      s_ressource[cbt.callbackPos].defaultCallback();
      return true;
    } else if ( cbt.type == GROUPED) {
      s_groupRessource[cbt.callbackPos].defaultCallback();
      return true;
    } else {
      return false;
    }
  }

  cbt = ressourceParser(blockStorage,b);
  if(cbt.callbackPos != 255 && cbt.actionPos != 255) {
    if(cbt.type != NONE) {
    s_ressource[cbt.callbackPos].actions[cbt.actionPos].actionCallback();
    return true;
    }
  }

  gCallbackType gcbt = groupParser(blockStorage, b);
  if(gcbt.callbackPos != 255 && gcbt.actionPos != 255 && gcbt.ressourceID != 255) {
    if(gcbt.type == GROUPED) {
      s_groupRessource[gcbt.callbackPos].actions[gcbt.actionPos].actionCallback(gcbt.ressourceID);
      return true;
    }
  }

  return false; //if no parser works return false
}

/*PARSERS*/
//TODO Test group callback
callbackType Rrest::directParser(char** blockStorage, uint8_t blockSize) {
  //if there is only one uri block we are accessing the ressource directly
  if(blockSize != 1) {
      return {255,255,NONE};
  }

  uint8_t pos = locateRessource(blockStorage[0], s_ressource);
  if(pos != 255) {
    return {pos,255,SIMPLE};
  }

  pos = locateRessource(blockStorage[0], s_groupRessource);
  if(pos != 255) {
    return {pos,255,GROUPED};
  }
    return {255,255,NONE}; // if no ressource is found
  }

//TODO test
callbackType Rrest::ressourceParser(char** blockStorage, uint8_t blockSize) {

  //detects if a block is only made of numeric values
  for (size_t i = 0; i < blockSize; ++i) {
    uint8_t cv = 0;
    for (size_t j = 0; j < strlen(blockStorage[i]); ++j) {
      if(isdigit(blockStorage[i][j])) {
        cv++;
      }
      if(cv == strlen(blockStorage[i])) {
        return {255,255,NONE};
      }
    }
    cv = 0;
  }

  uint8_t resPos = locateRessource(blockStorage[0], s_ressource);
  if(resPos == 255) {
    return {255,255,NONE};
  }

  uint8_t actPos = 0;//found action position

  while (strcmp(blockStorage[1], s_ressource[resPos].actions[actPos].name) != 0) {
    actPos++;
    if(actPos > MAX_URI_ACTIONS) return {255,255,NONE};
  }

  return {resPos,actPos,SIMPLE};
}

//TODO end and test
gCallbackType Rrest::groupParser(char** blockStorage, uint8_t blockSize) {

  Serial.println("CALLING GROUPPARSER");

  uint8_t numPos = 0;
  uint8_t resID = 0;

  //TODO garder la postiion du bloc numeriqueF
  for (size_t i = 0; i < blockSize; ++i) {
    uint8_t cv = 0;
    for (size_t j = 0; j < strlen(blockStorage[i]); ++j) {
      if(isdigit(blockStorage[i][j])) {
        cv++;
      }
      if(cv == strlen(blockStorage[i])) {
        numPos = i;
      }
    }
    cv = 0;
  }

  Serial.println(blockStorage[numPos]);
  resID = atoi(blockStorage[numPos]);

  Serial.print("bloc numérique ");
  Serial.println(numPos);

  uint8_t resPos = locateRessource(blockStorage[0], s_groupRessource);
  if(resPos == 255) {
    return {255,255,255,NONE};
  }

  uint8_t actPos = 0;

  if(numPos >= blockSize) {
    return {255,255,255,NONE};
  }

  while (strcmp(blockStorage[numPos+1], s_groupRessource[resPos].actions[actPos].name) != 0) {
    actPos++;
    if(actPos < MAX_URI_ACTIONS) return {255,255,255,NONE};
  }

  return {resPos,actPos,resID,GROUPED};
}

uint8_t Rrest::locateRessource(char* ressourceName, const struct ressource* st) {
  uint8_t res = 0;
  while (strcmp(st[res++].ressourceName, ressourceName) != 0) {
    if(res == ressourceElements) return 255;
  }
  return res-1;
}

uint8_t Rrest::locateRessource(char* ressourceName, const struct groupRessource* st) {
  uint8_t res = 0;
  while (strcmp(st[res++].ressourceName, ressourceName) != 0) {
    if(res == groupRessourceElements) return 255;
  }
  return res-1;
}

char* Rrest::strExtr(char* src, uint8_t s, uint8_t e) {
  uint8_t len = e-s;

  //debug
  /*
  Serial.println("");
  Serial.println("Starting extraction with ");
  Serial.print(s);
  Serial.print(" - ");
  Serial.print(e);
  Serial.print(" len ");
  Serial.print(len);
  Serial.println("");
  */

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
