#include <Rrest.h>

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
      s_ressource[cbt.callbackPos].defaultCallback();
      return true;
  }

  cbt = ressourceParser(blockStorage,b);
  if(cbt.callbackPos != 255 && cbt.actionPos != 255) {

    s_ressource[cbt.callbackPos].actions[cbt.actionPos].actionCallback();
    return true;
  }
  return false; //if no parser works return false
}

callbackType Rrest::directParser(char** blockStorage, uint8_t blockSize) {
  //if there is only one uri block we are accessing the ressource directly
  if(blockSize != 1) {
      return {255,255,NONE};
  }

  uint8_t pos = locateRessource(blockStorage[0], s_ressource);
  if(pos != 255) {
    return {pos,255,SIMPLE};
  }
    return {255,255,NONE}; // if no ressource is found
  }

callbackType Rrest::ressourceParser(char** blockStorage, uint8_t blockSize) {
  //detects if a block is only made of numeric values
  //if true it's a grouped ressource
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

  //locate Ressource
  uint8_t resPos = locateRessource(blockStorage[0], s_ressource);
  if(resPos == 255) {
    return {255,255,NONE};
  }

  //locate action
  uint8_t actPos = 0;
  while(strcmp(blockStorage[1], s_ressource[resPos].actions[actPos].name) != 0) {
    if(actPos+1 >= MAX_URI_ACTIONS) {
      return {255,255,NONE};
    }
    ++actPos;
  }
  return {resPos,actPos,SIMPLE};
}

uint8_t Rrest::locateRessource(char* ressourceName, const struct ressource* st) {
  uint8_t res = 0;
  while (strcmp(st[res++].ressourceName, ressourceName) != 0) {
    if(res == ressourceElements) return 255;
  }
  return res-1;
}
char* Rrest::strExtr(char* src, uint8_t s, uint8_t e) {
  uint8_t len = e-s;

  if(len == 0) {
    // Serial.println("len = 0");
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
