#include <Rrest.h>

Rrest::Rrest() {}

//TODO Support uri argument ex:/light/on?arg=2
//TODO Test

bool Rrest::handleRest() {
  char* _uri = new char[server.uri().length() + 1];
  strcpy(_uri, server.uri().c_str());
  uint8_t uriLength = strlen(_uri);

  //debug: show the uri and its lenght
  /*
  Serial.print("URI: ");
  Serial.print(_uri);
  Serial.print(" lenght: ");
  Serial.println(uriLength);
  */

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
  /*
   Serial.print("number of blocks: ");
   Serial.println(b);
   Serial.print("block pos: ");
   for (size_t u = 0; u < b; ++u) {
     Serial.print(bPos[u]);
     if(u != b-1) {
       Serial.print(',');
     }
   }
  */
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
  Serial.print("Callbacktype directParser position");
  Serial.println(cbt.callbackPos);

  if(cbt.callbackPos != 255) {
    s_ressource[cbt.callbackPos].defaultCallback();
    return true;
  }

  cbt = ressourceParser(blockStorage,b);
  Serial.print("Callbacktype ressourceParser position");
  Serial.println(cbt.callbackPos);
  Serial.print("Callbacktype ressourceParser action position");
  Serial.println(cbt.actionPos);

  if(cbt.callbackPos != 255) {
    if(cbt.actionPos != 255) {
      s_ressource[cbt.callbackPos].actions[cbt.actionPos].actionCallback();
      return true;
    }
  }

  cbt = groupParser(blockStorage, b);
  if(cbt.callbackPos != 255) {
    if(cbt.actionPos != 255) {
      //call group callback with ressource identifier
    }
  }

  return false; //if no parser works return false
}

/*PARSERS*/
/*
  TODO TEST parsing and group parsing
*/
callbackType Rrest::directParser(char** blockStorage, uint8_t blockSize) {

  //DEBUG
  Serial.print("calling directParser with blocksize ");
  Serial.println(blockSize);

  //if there is only one uri block we are accessing the ressource directly
  if(blockSize != 1) {
      return {255,255,NONE};
  }

  uint8_t pos = locateRessource(blockStorage[0], s_ressource);
  Serial.println("RESSOURCE ");
  Serial.println(blockStorage[0]);
  if(pos != 255) {
    Serial.print("Ressource located at pos");
    Serial.println(pos);
    return {pos,255,SIMPLE};
  }

  // while (pos != ressourceElements) {
  //   if(strcmp(blockStorage[0],s_ressource[pos].ressourceName) == 0) {
  //     Serial.print(blockStorage[0]);
  //     Serial.print(" is like ");
  //     Serial.print(s_ressource[pos].ressourceName);
  //     return {pos, 255, SIMPLE};
  //   }
  //   ++pos;
  // }


  pos = locateRessource(blockStorage[0], s_groupRessource);
  if(pos != 255) {
    Serial.print("GRessource located at pos");
    Serial.println(pos);
    return {pos,255,GROUPED};
  }

  // while (pos != groupRessourceElements) {
  //   if(strcmp(blockStorage[0],s_groupRessource[pos].ressourceName) == 0) {
  //     Serial.println("found pos");
  //     Serial.print(pos);
  //     return {pos, 255, GROUPED};
  //   }
  //   ++pos;
  // }

    Serial.println("NO RESSOURCE FOUND");
    return {255,255,NONE}; // if no ressource is found
  }

//TODO test
callbackType Rrest::ressourceParser(char** blockStorage, uint8_t blockSize) {

  //detects if a block is only made of numeric values
  //return null to allow the groupParser to parse
  //TODO TEST
  for (size_t i = 0; i < blockSize; ++i) {
    static uint8_t cv = 0;
    for (size_t j = 0; j < strlen(blockStorage[i]); ++j) {
      if(isdigit(blockStorage[i][j])) {
        cv++;
      }
      if(cv == strlen(blockStorage[i])) {
        return {255,255,NONE};
      }
    }
  }

  uint8_t resPos = 0; //found ressource position
  //search for ressouce name in the first uri block
  while (strcmp(blockStorage[0], s_ressource[resPos++].ressourceName) != 0) {
    if(resPos < MAX_RESSOURCE_COUNT) return {255,255,NONE};
  }

  uint8_t actPos = 0;//found action position
  //search for action in the second block
  while (strcmp(blockStorage[1], s_ressource[resPos].actions[actPos++].name) != 0) {
    if(actPos < MAX_URI_ACTIONS) return {255,255,NONE};
  }

  return {resPos,actPos,SIMPLE};
}

//TODO test
callbackType Rrest::groupParser(char** blockStorage, uint8_t blockSize) {

  uint8_t numPos = 0;
  //TODO test
  //function de recherche de bloc numérique même que ressource parser
  for (size_t i = 0; i < blockSize; ++i) {
    static uint8_t cv = 0;
    for (size_t j = 0; j < strlen(blockStorage[i]); ++j) {
      if(isdigit(blockStorage[i][j])) {
        cv++;
      }
      if(cv == strlen(blockStorage[i])) {
        numPos = cv;
      }
    }
  }

  uint8_t resPos = 0;

  while(strcmp(blockStorage[0], s_groupRessource[resPos++].ressourceName) != 0) {
    if(resPos < MAX_RESSOURCE_COUNT) return {255,255,NONE};
  }

  uint8_t actPos = 0;
  while (strcmp(blockStorage[numPos+1], s_groupRessource[resPos].actions[actPos++].name) != 0) {
    if(actPos < MAX_URI_ACTIONS) return {255,255,NONE};
  }

  return {resPos,actPos,GROUPED};
}

uint8_t Rrest::locateRessource(char* ressourceName, const struct ressource* st) {
  uint8_t res = 0;

  //debug
  Serial.println(),
  Serial.print("starting locateRessource with ressource ");
  Serial.println(ressourceName);
  while (strcmp(st[res++].ressourceName, ressourceName) != 0) {
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  // while (st[res].ressourceName != ressourceName) {
  //   Serial.println(st[res].ressourceName);
  //   res++;
  //   if(res == MAX_RESSOURCE_COUNT) return 255;
  // }
  return res;
}

uint8_t Rrest::locateRessource(char* ressourceName, const struct groupRessource* st) {
  uint8_t res = 0;
  while (strcmp(st[res++].ressourceName, ressourceName) != 0) {
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  // while (st[res].ressourceName != ressourceName) {
  //   res++;
  //   if(res == MAX_RESSOURCE_COUNT) return 255;
  // }
  return res;
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
