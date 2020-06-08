#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdio.h>

#ifdef __linux__
#include <open62541/plugin/pubsub_ethernet.h>
#include <open62541/plugin/pubsub_udp.h>
#endif

#include "utils.h"
#include "informationmodel.h"
#include "pubsub.h"

static volatile UA_Boolean running = true;

void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received ctrl-c");
    running = false;
}

int configurePubSub(UA_Server *server, UA_ServerConfig *config) {
    #ifdef __linux__
    UA_String transportProfile =
        UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
    UA_NetworkAddressUrlDataType networkAddressUrl =
        {UA_STRING_NULL , UA_STRING("opc.udp://224.0.0.22:4840/")};

    config->pubsubTransportLayers =
        (UA_PubSubTransportLayer *) UA_calloc(2, sizeof(UA_PubSubTransportLayer));
    if(!config->pubsubTransportLayers) {
        UA_Server_delete(server);
        return EXIT_FAILURE;
    }
    config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
    config->pubsubTransportLayersSize++;
    
    #ifdef UA_ENABLE_PUBSUB_ETH_UADP
    config->pubsubTransportLayers[1] = UA_PubSubTransportLayerEthernet();
    config->pubsubTransportLayersSize++;
    #endif

    #endif

    UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;

    addPubSubConnection(server, &transportProfile, &networkAddressUrl, &connectionIdent, "Connection1");
    addPublishedDataSet(server, &publishedDataSetIdent, "PDS1");

    addDataSetField(server, publishedDataSetIdent, "temperatureCatania", UA_NODEID_STRING(1, "weather-Catania-temperature"));
    addDataSetField(server, publishedDataSetIdent, "temperatureCatania", UA_NODEID_STRING(1, "weather-Enna-temperature"));
    addDataSetField(server, publishedDataSetIdent, "temperatureCatania", UA_NODEID_STRING(1, "weather-Monciuffi-temperature"));
    
    addWriterGroup(server, connectionIdent, &writerGroupIdent, "WriterGroup1");
    addDataSetWriter(server, publishedDataSetIdent, writerGroupIdent, "DataSetWriter1");

    
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
    
    configurePubSub(server, config);

    /*Prova con ObjectTypeCustom*/

    //defineObjectTypeWeather(server);
    //defInstanceWeather(server, "Catania"); 
   
    /* Prova con l'istanza diretta di oggetti */
    defineWeatherObject(server, "Catania"); 
    defineWeatherObject(server, "Enna"); 
    defineWeatherObject(server, "Monciuffi");

    UA_Server_addRepeatedCallback(server, updateValueCallback, (void *) "Catania", 2000, NULL);
    UA_Server_addRepeatedCallback(server, updateValueCallback, (void*) "Enna", 2000, NULL);
    UA_Server_addRepeatedCallback(server, updateValueCallback, (void*) "Monciuffi", 2000, NULL);

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
