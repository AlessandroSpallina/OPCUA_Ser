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
#include "sensors.h"

#define WEATHER_STATIONS_COUNT 8

volatile UA_Boolean running = true;

//struttura per le diverse configurazionid el server - Default Settings - Disabilita cifratura, usa UDP, path del certificato, della chiave e Url Multicast 
struct applicationConfig {
        bool encryption;
        bool usingUdpUadp;
        char *certPath;
        char *keyPath;
        char *customUrl;
} appConf = {false, true, NULL, NULL, NULL}; 

//struttura custom per la specifica dei DataSetField da pubblicare 


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

//Presentazione del server e visualizzazione della configurazione attuale
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


//Shutdown del server al segnale 
void stopHandler(int sign) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received ctrl-c");
        running = false;
}

int main(int argc, char *argv[]) {
    
        //UA_NodeId settato di ritorno alla definizione del ObjecType -> Dinamico
        UA_NodeId wtype; 

        //Check degli argomenti, set dell'appConfig
        parseArgument(argc, argv);

        printWelcome();

        //Inizializza il server e ServerConfig
        UA_Server *server = UA_Server_new();
        UA_ServerConfig* config = UA_Server_getConfig(server);

        UA_StatusCode retval;

        if (appConf.encryption) {
                /* Carica certificato e chiave se esiste */
                UA_ByteString certificate = loadFile(appConf.certPath);
                UA_ByteString privateKey = loadFile(appConf.keyPath);

                retval = UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4840, &certificate, &privateKey, NULL, 0, NULL, 0, NULL, 0);
        } else {
                retval = UA_ServerConfig_setDefault(config);
        }

        if (retval != UA_STATUSCODE_GOOD) {
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Error initializating server");
                UA_Server_delete(server);
                return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        ///* Generazione nuovo ObjecType -> WeatheType. Ritorna il NodeId che identifica il tipo*/
        //wtype = defineWeatherObjectAsDataSource(server);
        ///* Definisco due istanze delle stazioni meteo. Ritorna il NodeId che identifica le due istanze*/
        //UA_NodeId first = defInstanceWeather(server, "Catania", wtype);
        //UA_NodeId second = defInstanceWeather(server, "Monciuffi", wtype);

        wtype = defineObjectTypeWeather(server);

        UA_NodeId weatherStations[WEATHER_STATIONS_COUNT] = {
            instantiateWeatherObject(server, wtype, "Catania"),
            instantiateWeatherObject(server, wtype, "Enna"),
            instantiateWeatherObject(server, wtype, "Palermo"),
            instantiateWeatherObject(server, wtype, "Agrigento"),
            instantiateWeatherObject(server, wtype, "Siracusa"),
            instantiateWeatherObject(server, wtype, "Trapani"),
            instantiateWeatherObject(server, wtype, "Ragusa"),
            instantiateWeatherObject(server, wtype, "Catanissetta"),
        };

        for (int i = 0; i < WEATHER_STATIONS_COUNT; i++) {
            UA_NodeId temperature = findNodeIdByBrowsename(server, weatherStations[i], UA_QUALIFIEDNAME(1, "temperature-variable"));
            addValueCallbackToVariable(server, temperature, beforeReadTemperature, afterWriteTemperature);

            UA_NodeId humidity = findNodeIdByBrowsename(server, weatherStations[i], UA_QUALIFIEDNAME(1, "humidity-variable"));
            addValueCallbackToVariable(server, humidity, beforeReadHumidity, afterWriteHumidity);
        }


        //PubSub abilitato con profilo UDP UADP
        if (appConf.usingUdpUadp) {
            //Set del profilo e del muticast Address
            UA_String transportProfile = UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp");
            UA_NetworkAddressUrlDataType networkAddressUrl = { UA_STRING_NULL, UA_STRING("opc.udp://224.0.0.22:4840/") };

            //Custom Url Multicast
            if (appConf.customUrl != NULL) {
                networkAddressUrl.url = UA_STRING(appConf.customUrl);
            }

           /*funzione custom per la ricerca del NodeId della variabile da pubblicare. Riceve in ingresso il NodeId del Parent Object e il QualifiedName (BrowseName) e restituisce l'id della variabile/risorsa da pubblicare  */
            fieldToPublish tmp[] = {
                { "temperatureCatania", findNodeIdByBrowsename(server, weatherStations[0], UA_QUALIFIEDNAME(1, "temperature-variable")) },
                { "temperatureEnna", findNodeIdByBrowsename(server, weatherStations[2], UA_QUALIFIEDNAME(1, "temperature-variable")) }
            };

            //custom function per config
            configurePubSub(server, config, transportProfile, networkAddressUrl, tmp, 2);
        }

        signal(SIGINT, stopHandler);
        signal(SIGTERM, stopHandler);

        retval = UA_Server_run(server, &running);

        UA_Server_delete(server);
        return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
