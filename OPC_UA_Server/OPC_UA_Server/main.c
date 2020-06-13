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

//Shutdown del server al segnale 
void stopHandler(int sign) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received ctrl-c");
        running = false;
}

int main(int argc, char *argv[]) {
    
        //UA_NodeId settato di ritorno alla definizione del ObjecType -> Dinamico
        UA_NodeId wtype; 

        //Check degli argomenti, set dell'appConfig
        const applicationConfig_t appConf = parseArgument(argc, argv);

        printWelcome(appConf);

        //Inizializza il server e ServerConfig
        UA_Server *server = UA_Server_new();
        UA_ServerConfig *config = UA_Server_getConfig(server);

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

        // Generazione nuovo ObjecType -> WeatheType. Ritorna il NodeId che identifica il tipo
        wtype = defineObjectTypeWeather(server);

        // Definisco N istanze delle stazioni meteo. Ritorna il NodeId che identifica le istanze
        const exposedNode_t weatherStations[WEATHER_STATIONS_COUNT] = {
            {"Catania",      instantiateWeatherObject(server, wtype, "Catania")},
            {"Enna",         instantiateWeatherObject(server, wtype, "Enna")},
            {"Palermo",      instantiateWeatherObject(server, wtype, "Palermo")},
            {"Agrigento",    instantiateWeatherObject(server, wtype, "Agrigento")},
            {"Siracusa",     instantiateWeatherObject(server, wtype, "Siracusa")},
            {"Trapani",      instantiateWeatherObject(server, wtype, "Trapani")},
            {"Ragusa",       instantiateWeatherObject(server, wtype, "Ragusa")},
            {"Catanissetta", instantiateWeatherObject(server, wtype, "Catanissetta")},
        };

        // Setto le callback per le letture e scritture sulle variabili temperature e humididty
        for (int i = 0; i < WEATHER_STATIONS_COUNT; i++) {
            UA_NodeId temperature = findNodeIdByBrowsename(server, weatherStations[i].nodeId, TEMPERATURE_VARIABLE_QUALIFIEDNAME);
            addValueCallbackToVariable(server, temperature, beforeReadTemperature, afterWriteTemperature);

            UA_NodeId humidity = findNodeIdByBrowsename(server, weatherStations[i].nodeId, HUMIDITY_VARIABLE_QUALIFIEDNAME);
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

            // array che detiene i campi da publicare nel dataset message => decidiamo di pubblicare le temperature di tutte le stazioni meteo
            exposedNode_t fieldsToPublish[WEATHER_STATIONS_COUNT];

            for (int i = 0; i < WEATHER_STATIONS_COUNT; i++) {
                char tmp[120];
                sprintf(tmp, "%sTemperature", weatherStations[i].nodeName);
                fieldsToPublish[i].nodeName = tmp;
                fieldsToPublish[i].nodeId = findNodeIdByBrowsename(server, weatherStations[i].nodeId, TEMPERATURE_VARIABLE_QUALIFIEDNAME);
            }

            // configura udp uadp
            addPubSubUdpToServerConfig(server, config);
            configurePubSubNetworkMessage(server, transportProfile, networkAddressUrl, WEATHER_STATIONS_COUNT, fieldsToPublish);
        }

        signal(SIGINT, stopHandler);
        signal(SIGTERM, stopHandler);

        retval = UA_Server_run(server, &running);

        UA_Server_delete(server);
        return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}
