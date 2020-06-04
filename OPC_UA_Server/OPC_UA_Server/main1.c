#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdio.h>

static void
updateValueCallback(UA_Server* server, void* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "updateValueCallback");
    UA_Float now = (UA_Float)rand() / 10; 
    UA_Variant value;
    UA_Variant_setScalar(&value, &now, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "weather-Catania-tmp");
    UA_Server_writeValue(server, currentNodeId, value);

}

static void
defineWeatherObject(UA_Server* server,char *locatioName) {

    /* get the nodeid assigned by the server */
    UA_NodeId weatherId;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "weatherQualifiedName"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        oAttr, NULL, &weatherId);
    //Primo attributo - Nome della località meteo

    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    UA_String cityName = UA_STRING(locatioName);    
    UA_Variant_setScalar(&mnAttr.value, &cityName, &UA_TYPES[UA_TYPES_STRING]);
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "City Name");
    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "CityNameQualified"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, NULL);
    //Secondo attributo - Temperatura della città

    UA_VariableAttributes tmpAttr = UA_VariableAttributes_default;
    char c[20];
    sprintf(c, "weather-%s-tmp", locatioName); 
    UA_NodeId attrNodeId = UA_NODEID_STRING(1, c);
    UA_Float Tmp = 0.0; 
    UA_Variant_setScalar(&tmpAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    UA_Server_addVariableNode(server, attrNodeId, weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "TempQUalifiedName"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, NULL);
    //Terzo attributo - Umidità della città 
    UA_VariableAttributes  humAttr = UA_VariableAttributes_default;
    UA_Float hum = 0.0; 
    UA_Variant_setScalar(&humAttr.value, &hum, &UA_TYPES[UA_TYPES_FLOAT]);
    humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Status"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humAttr, NULL, NULL);
}

static volatile UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    UA_NodeId cityTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {50510} };
    UA_Server* server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    
    defineWeatherObject(server, "Catania"); 
    defineWeatherObject(server, "Enna"); 
    defineWeatherObject(server, "Monciuffi"); 

    UA_Server_addRepeatedCallback(server, updateValueCallback, NULL, 2000, NULL);

    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}