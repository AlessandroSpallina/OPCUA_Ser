#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/pubsub_udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "informationmodel.h"
#include "pubsub.h"

volatile UA_Boolean running = true;

struct applicationConfig {
        bool encryption;
        bool usingUdpUadp;
        char *certPath;
        char *keyPath;
        char *customUrl;
} appConf = {false, true, NULL, NULL, NULL}; // default appConfig

struct fieldToPublish {
    char* fieldName;
    UA_NodeId variableId;
};

// controlla che l'usage sia del tipo: server.exe [--cert <pathCertificato> --key <pathChiave>] [--url <customUdpUadpUrl>] 
void parseArgument(int argc, char* argv[]) {
        if (argc == 1)
                return;

        if (strncmp(argv[1], "-h", 2) == 0) {
                fprintf(stderr, "Usage: server.exe [--cert <pathCertificato> --key <pathChiave>] [--url <customUdpUadpUrl>]\n");
                exit(EXIT_SUCCESS);
        }

        for (int i = 1; i < argc-1; i++) {
                if (strncmp(argv[i], "--cert", 6) == 0) {
                        if (strncmp(argv[i+2], "--key", 5) == 0) {
                                appConf.encryption = true;
                                appConf.certPath = argv[i+1];
                                appConf.keyPath = argv[i+3];
                                i += 3;
                        }
                } else if (strncmp(argv[i], "--url", 5) == 0) {
                        if (strncmp(argv[i+1], "opc.udp://", 10) == 0) {
                                appConf.usingUdpUadp = true;
                                appConf.customUrl = argv[i+1];
                                i += 1;
                        }
                }
        }
}

void printWelcome() {
        printf("Welcome in OPC UA Server\n");
        printf("You are running this process with following config:\n");
        if (appConf.encryption) {
                printf("* Using Encryption\n");
                printf("    - Cert at %s\n", appConf.certPath);
                printf("    - Key at %s\n", appConf.keyPath);
        }
        if (appConf.usingUdpUadp) {
                printf("* Enabled PubSub with UDP UADP\n");
                if (appConf.customUrl != NULL) {
                        printf("* UDP UADP custom url enabled\n");
                        printf("    - Custom url %s\n", appConf.customUrl);
                }
                else {
                        printf("    - Default url opc.udp://224.0.0.22:4840/\n");
                }
        }
        printf("\n");
}

void stopHandler(int sign) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received ctrl-c");
        running = false;
}



int configurePubSub(UA_Server *server, UA_ServerConfig *config, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, struct fieldToPublish fields[]) {
    
        config->pubsubTransportLayers = (UA_PubSubTransportLayer *) UA_calloc(2, sizeof(UA_PubSubTransportLayer));
        if(!config->pubsubTransportLayers) {
                UA_Server_delete(server);
                return EXIT_FAILURE;
        }
        config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
        config->pubsubTransportLayersSize++;    

        UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;

 
        addPubSubConnection(server, &transportProfile, &networkAddressUrl, &connectionIdent, "Connection1");
        addPublishedDataSet(server, &publishedDataSetIdent, "PDS1");

        addDataSetField(server, publishedDataSetIdent, fields[0].fieldName, fields[0].variableId);
        addDataSetField(server, publishedDataSetIdent, fields[1].fieldName, fields[1].variableId);

        //int fieldsCount = sizeof(*fields) / sizeof(struct fieldToPublish);

        //for (int i = 0; i < fieldsCount; i++) {
        //    addDataSetField(server, publishedDataSetIdent, fields[i].fieldName, fields[i].variableId);
        //}

        addWriterGroup(server, connectionIdent, &writerGroupIdent, "WriterGroup1");
        addDataSetWriter(server, publishedDataSetIdent, writerGroupIdent, "DataSetWriter1");
}

int main(int argc, char *argv[]) {
    
        UA_NodeId wtype; 

        parseArgument(argc, argv);

        printWelcome();

        UA_Server *server = UA_Server_new();
        UA_ServerConfig* config = UA_Server_getConfig(server);

        UA_StatusCode retval;

        if (appConf.encryption) {
                /* Load certificate and private key */
                UA_ByteString certificate = loadFile(appConf.certPath);
                UA_ByteString privateKey = loadFile(appConf.keyPath);

                retval = UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4840, &certificate, &privateKey, NULL, 0, NULL, 0, NULL, 0);
        } else {
                retval = UA_ServerConfig_setDefault(config);
        }

        if (retval != UA_STATUSCODE_GOOD) {
                UA_Server_delete(server);
                return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
        }


        wtype = defineWeatherObjectAsDataSource(server);
        UA_NodeId first = defInstanceWeather(server, "Catania", wtype);
        UA_NodeId second = defInstanceWeather(server, "Monciuffi", wtype);

        if (appConf.usingUdpUadp) {

            UA_String transportProfile = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
            UA_NetworkAddressUrlDataType networkAddressUrl = { UA_STRING_NULL, UA_STRING("opc.udp://224.0.0.22:4840/") };

            if (appConf.customUrl != NULL) {
                networkAddressUrl.url = UA_STRING(appConf.customUrl);
            }

            // find node to publish

            struct fieldToPublish tmp[] = {
                { "temperatureCatania", findNodeIdByBrowsename(server, first, UA_QUALIFIEDNAME(1, "temperature-variable")) },
                { "temperatureMonciuffi", findNodeIdByBrowsename(server, second, UA_QUALIFIEDNAME(1, "temperature-variable")) }
            };

            configurePubSub(server, config, transportProfile, networkAddressUrl, tmp);
        }

        

        signal(SIGINT, stopHandler);
        signal(SIGTERM, stopHandler);

        retval = UA_Server_run(server, &running);

        UA_Server_delete(server);
        return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
