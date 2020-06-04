#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include "TypeDefinition.c"
#include <signal.h>

 /* predefined identifier for later use */
//UA_NodeId cityTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {50510} };
//UA_NodeId weatherTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} }; 

//static void
//defineObjectTypes(UA_Server* server) {
//    /* Define the object type for "Device" */
//    UA_ObjectTypeAttributes dtAttr = UA_ObjectTypeAttributes_default;
//    dtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityType");
//    UA_Server_addObjectTypeNode(server, cityTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//        UA_QUALIFIEDNAME(1, "CityType"), dtAttr,
//        NULL, NULL);
//    
//
//    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
//    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Name");   
//    UA_NodeId manufacturerNameId;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Name"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, &manufacturerNameId);
//    /* Make the manufacturer name mandatory */
//    UA_Server_addReference(server, manufacturerNameId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//
//    UA_VariableAttributes modelAttr = UA_VariableAttributes_default;
//    modelAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Latitude");
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Latitude"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), modelAttr, NULL, NULL);
//
//    /* Define the object type for "Pump" */
//    UA_ObjectTypeAttributes ptAttr = UA_ObjectTypeAttributes_default;
//    ptAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
//    UA_Server_addObjectTypeNode(server, weatherTypeId,
//        //deviceTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//        UA_QUALIFIEDNAME(1, "WeatherType"), ptAttr,
//        NULL, NULL);
//
//    UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
//    statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
//    statusAttr.valueRank = UA_VALUERANK_SCALAR;
//    UA_NodeId statusId;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Temperature"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), statusAttr, NULL, &statusId);
//    /* Make the status variable mandatory */
//    UA_Server_addReference(server, statusId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//    UA_VariableAttributes rpmAttr = UA_VariableAttributes_default;
//    rpmAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
//    rpmAttr.valueRank = UA_VALUERANK_SCALAR;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Humidity"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), rpmAttr, NULL, NULL);
//}

/**
 * Now we add the derived ObjectType for the pump that inherits from the device
 * object type. The resulting object contains all mandatory child variables.
 * These are simply copied over from the object type. The object has a reference
 * of type ``hasTypeDefinition`` to the object type, so that clients can detect
 * the type-instance relation at runtime.
 */
//
//static void
//addObjectInstance(UA_Server* server, char* name, UA_NodeId NodeIdType) {
//    UA_NodeId weatherTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} };
//    UA_NodeId cityId;
//    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
//    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
//    UA_Server_addObjectNode(server, UA_NODEID_NULL,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
//        UA_QUALIFIEDNAME(1, name),
//        NodeIdType, /* this refers to the object type
//                       identifier */
//        oAttr, NULL, &cityId);
//    UA_Server_addReference(server, cityId, UA_NODEID_NUMERIC(0, weatherTypeId),
//        , true);
//    // see https://github.com/open62541/open62541/blob/master/examples/tutorial_server_object.c
//
//    //UA_VariableAttributes vAtt = UA_VariableAttributes_default;
//    //vAtt.displayName = UA_LOCALIZEDTEXT("en-US", name);
//    //UA_Server_addVariableNode(server,UA_NODEID_NULL,)
//}

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

    defineCityObjectType(server); 
    defineWeatherObjectType(server); 
    addCityInstance(server, "Catania", cityTypeId);
    //defineObjectTypes(server);
    //dObjectInstance(server, "Catania", cityTypeId);
    //addObjectInstance(server, "Enna",cityTypeId);
    
    
    // TODO Fix instance of weather and city
    
    
    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}