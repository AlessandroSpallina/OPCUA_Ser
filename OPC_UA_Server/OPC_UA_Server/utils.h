#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

#define DEBUG false

//struttura per le diverse configurazionid del server - Default Settings - Disabilita cifratura, usa UDP, path del certificato, della chiave e Url Multicast 
typedef struct applicationConfig {
    bool encryption;
    bool usingUdpUadp;
    char* certPath;
    char* keyPath;
    char* customUrl;
} applicationConfig_t;

UA_ByteString loadFile(const char* path);
applicationConfig_t parseArgument(int argc, char* argv[]);
void printWelcome(const applicationConfig_t appConf);
