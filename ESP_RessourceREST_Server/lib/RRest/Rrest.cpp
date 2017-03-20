#include <Rrest.h>

Rrest::Rrest() {}

//TODO Support uri argument ex:/light/on?arg=2
//TODO Add return statements
//TODO Test

bool Rrest::handleRest() {
  Serial.println("CALLING TEST RESSOURCE CALLBACK");
  s_ressource[0].defaultCallback;


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
   c = sizeof(bPos);//powerful naming convention
   for (size_t k = 0; k < b; ++k) {
     if(k == c-1){
       blockStorage[k] = strExtr(_uri, bPos[k]+1, uriLength);
     } else {
       blockStorage[k] = strExtr(_uri,bPos[k]+1,bPos[k+1]);
     }
   }

  if(directParser(blockStorage, b) == NULL) {
    if(ressourceParser(blockStorage, b) == NULL) {
      if(groupParser(blockStorage) == NULL) {
        return false;
      }
    }
  }

  return true;
}

/*PARSERS*/
//TODO
Callback Rrest::directParser(char** blockStorage, uint8_t blockSize) {
  //if there is only one uri block we are accessing the ressource directly
  if(blockSize == 1) {
    for (size_t j = 0; j < MAX_RESSOURCE_COUNT; ++j) {
      if(strcmp(blockStorage[0],s_ressource[j].ressourceName) == 0) {
        return s_ressource[j].defaultCallback;
      }
    }
  }
  return NULL;
}

//TODO
Callback Rrest::ressourceParser(char** blockStorage, uint8_t blockSize) {

  //detects if a block is only made of numeric values
  //return null to allow the groupParser to parse
  //TODO TEST
  for (size_t i = 0; i < blockSize; ++i) {
    static uint8_t cv = 0;
    for (size_t j = 0; j < strlen(blockStorage[i]); j++) {
      if(isdigit(blockStorage[i][j])) {
        cv++;
      }
      if(cv == strlen(blockStorage[i])) {
        return NULL;
      }
    }
  }

  uint8_t resPos = 0;
  while (strcmp(blockStorage[0],s_ressource[resPos++].ressourceName) != 0) {
    if(resPos < MAX_RESSOURCE_COUNT) return NULL;
  }
}

//TODO
Gcallback Rrest::groupParser(char** blockStorage) {

}

uint8_t Rrest::locateRessource(char* ressourceName,struct ressource* st) {
  uint8_t res = 0;
  while (st[res].ressourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
  return res;
}

uint8_t Rrest::locateRessource(char* ressourceName,struct groupRessource* st) {
  uint8_t res = 0;
  while (st[res].ressourceName != ressourceName) {
    res++;
    if(res == MAX_RESSOURCE_COUNT) return 255;
  }
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
