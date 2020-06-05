#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdio.h>

#include "utils.h"
#include "informationmodel.h"

static volatile UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received ctrl-c");
    running = false;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        UA_LOG_FATAL(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
            "Missing arguments.\n"
            "Usage <server-certificate.der> <private-key.der>");
        return EXIT_FAILURE;
    }

    /* Load certificate and private key */
    UA_ByteString certificate = loadFile(argv[1]);
    UA_ByteString privateKey = loadFile(argv[2]);

    // cityTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {50510} };

    UA_Server* server = UA_Server_new();
    UA_ServerConfig* config = UA_Server_getConfig(server);

    UA_StatusCode retval = UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4040, 
        &certificate, &privateKey, NULL, 0, NULL, 0, NULL, 0);

    if (retval != UA_STATUSCODE_GOOD) {
        UA_Server_delete(server);
        return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    
    /*Prova con ObjectTypeCustom*/

    defineObjectTypeWeather(server);
    defInstanceWeather(server, "Catania"); 
   
    /* Prova con l'istanza diretta di oggetti */

    //defineWeatherObject(server, "Catania"); 
    //defineWeatherObject(server, "Enna"); 
    //defineWeatherObject(server, "Monciuffi");
    //UA_Server_addRepeatedCallback(server, updateValueCallback, NULL, 2000, NULL);

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    retval = UA_Server_run(server, &running);

    return retval;
}
