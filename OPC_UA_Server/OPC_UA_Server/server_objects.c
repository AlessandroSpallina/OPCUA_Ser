//#include <open62541/plugin/log_stdout.h>
//#include <open62541/server.h>
//#include <open62541/server_config_default.h>
//
//#include <signal.h>
//
///* predefined identifier for later use */
//UA_NodeId weatherTypeId = {1, UA_NODEIDTYPE_NUMERIC, {100991}};
//
//static void defineObjectTypes(UA_Server *server) {
//    /* Define the object type for "City" */
//    UA_NodeId cityTypeId = UA_NODEID_NUMERIC(1, 1); /* get the nodeid assigned by the server */
//    UA_ObjectTypeAttributes ctAttr = UA_ObjectTypeAttributes_default;
//    ctAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityType");
//    UA_Server_addObjectTypeNode(server, cityTypeId,
//                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
//                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//                                UA_QUALIFIEDNAME(1, "CityType"), ctAttr,
//                                NULL, NULL);    
//
//    UA_VariableAttributes nameAttr = UA_VariableAttributes_default;
//    nameAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Name");
//    UA_NodeId nameId;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
//                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                              UA_QUALIFIEDNAME(1, "Name"),
//                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nameAttr, NULL, &nameId);
//    /* Make the manufacturer name mandatory */
//    UA_Server_addReference(server, nameId,
//                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//    UA_VariableAttributes longitudeAttr = UA_VariableAttributes_default;
//    longitudeAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Longitude");
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Latitude"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), longitudeAttr, NULL, NULL);
//
//
//    UA_VariableAttributes latitudeAttr = UA_VariableAttributes_default;
//    latitudeAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Latitude");
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
//                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                              UA_QUALIFIEDNAME(1, "Latitude"),
//                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), latitudeAttr, NULL, NULL);   
//
//
//    // Define the object type for "Weather"
//    UA_ObjectTypeAttributes wtAttr = UA_ObjectTypeAttributes_default;
//    wtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
//    UA_Server_addObjectTypeNode(server, weatherTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//        UA_QUALIFIEDNAME(1, "WeatherType"), wtAttr,
//        NULL, NULL);    
//
//    UA_Server_addObjectTypeNode(server, weatherTypeId,
//        cityTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "WeatherType"), wtAttr,
//        NULL, NULL);
//    
//    UA_VariableAttributes temperatureAttr = UA_VariableAttributes_default;
//    temperatureAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
//    temperatureAttr.valueRank = UA_VALUERANK_SCALAR;
//    UA_NodeId temperatureId;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
//                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//                          UA_QUALIFIEDNAME(1, "Temperature"),
//                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), temperatureAttr, NULL, &temperatureId);
//
//    UA_VariableAttributes humidityAttr = UA_VariableAttributes_default;
//    humidityAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
//    humidityAttr.valueRank = UA_VALUERANK_SCALAR;
//    UA_NodeId humidityId;
//    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "Humidity"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humidityAttr, NULL, &humidityId);
//
//
//    ///* Define the object type for "Pump" */
//    //UA_ObjectTypeAttributes ptAttr = UA_ObjectTypeAttributes_default;
//    //ptAttr.displayName = UA_LOCALIZEDTEXT("en-US", "PumpType");
//    //UA_Server_addObjectTypeNode(server, pumpTypeId,
//    //                            deviceTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//    //                            UA_QUALIFIEDNAME(1, "PumpType"), ptAttr,
//    //                            NULL, NULL);
//
//    //UA_VariableAttributes statusAttr = UA_VariableAttributes_default;
//    //statusAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Status");
//    //statusAttr.valueRank = UA_VALUERANK_SCALAR;
//    //UA_NodeId statusId;
//    //UA_Server_addVariableNode(server, UA_NODEID_NULL, pumpTypeId,
//    //                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//    //                          UA_QUALIFIEDNAME(1, "Status"),
//    //                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), statusAttr, NULL, &statusId);
//    ///* Make the status variable mandatory */
//    //UA_Server_addReference(server, statusId,
//    //                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//    //                       UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//    //UA_VariableAttributes rpmAttr = UA_VariableAttributes_default;
//    //rpmAttr.displayName = UA_LOCALIZEDTEXT("en-US", "MotorRPM");
//    //rpmAttr.valueRank = UA_VALUERANK_SCALAR;
//    //UA_Server_addVariableNode(server, UA_NODEID_NULL, pumpTypeId,
//    //                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//    //                          UA_QUALIFIEDNAME(1, "MotorRPMs"),
//    //                          UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), rpmAttr, NULL, NULL);
//}
//
///**
// * Now we add the derived ObjectType for the pump that inherits from the device
// * object type. The resulting object contains all mandatory child variables.
// * These are simply copied over from the object type. The object has a reference
// * of type ``hasTypeDefinition`` to the object type, so that clients can detect
// * the type-instance relation at runtime.
// */
//
//static void addObjectInstance(UA_Server* server, char* name) {
//    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
//    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
//   
//    UA_Server_addObjectNode(server, UA_NODEID_NULL, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),UA_QUALIFIEDNAME(1, name),
//        cityTypeId, /* this refers to the object type
//                       identifier */
//        oAttr, NULL, NULL);
//}
//
////static void addPumpObjectInstance(UA_Server *server, char *name) {
////    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
////    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
////    UA_Server_addObjectNode(server, UA_NODEID_NULL,
////                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
////                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
////                            UA_QUALIFIEDNAME(1, name),
////                            pumpTypeId, /* this refers to the object type
////                                           identifier */
////                            oAttr, NULL, NULL);
////}
//
///**
// * Often times, we want to run a constructor function on a new object. This is
// * especially useful when an object is instantiated at runtime (with the
// * AddNodes service) and the integration with an underlying process canot be
// * manually defined. In the following constructor example, we simply set the
// * pump status to on.
// */
//
////static UA_StatusCode
////pumpTypeConstructor(UA_Server *server,
////                    const UA_NodeId *sessionId, void *sessionContext,
////                    const UA_NodeId *typeId, void *typeContext,
////                    const UA_NodeId *nodeId, void **nodeContext) {
////    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "New pump created");
////
////    /* Find the NodeId of the status child variable */
////    UA_RelativePathElement rpe;
////    UA_RelativePathElement_init(&rpe);
////    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
////    rpe.isInverse = false;
////    rpe.includeSubtypes = false;
////    rpe.targetName = UA_QUALIFIEDNAME(1, "Status");
////
////    UA_BrowsePath bp;
////    UA_BrowsePath_init(&bp);
////    bp.startingNode = *nodeId;
////    bp.relativePath.elementsSize = 1;
////    bp.relativePath.elements = &rpe;
////
////    UA_BrowsePathResult bpr =
////        UA_Server_translateBrowsePathToNodeIds(server, &bp);
////    if(bpr.statusCode != UA_STATUSCODE_GOOD ||
////       bpr.targetsSize < 1)
////        return bpr.statusCode;
////
////    /* Set the status value */
////    UA_Boolean status = true;
////    UA_Variant value;
////    UA_Variant_setScalar(&value, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
////    UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
////    UA_BrowsePathResult_clear(&bpr);
////
////    /* At this point we could replace the node context .. */
////
////    return UA_STATUSCODE_GOOD;
////}
//
////static void
////addPumpTypeConstructor(UA_Server *server) {
////    UA_NodeTypeLifecycle lifecycle;
////    lifecycle.constructor = pumpTypeConstructor;
////    lifecycle.destructor = NULL;
////    UA_Server_setNodeTypeLifecycle(server, pumpTypeId, lifecycle);
////}
//
///** It follows the main server code, making use of the above definitions. */
//
//static volatile UA_Boolean running = true;
//static void stopHandler(int sign) {
//    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
//    running = false;
//}
//
//int main(void) {
//    signal(SIGINT, stopHandler);
//    signal(SIGTERM, stopHandler);
//
//    UA_Server *server = UA_Server_new();
//    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
//
//    defineObjectTypes(server);
//    addObjectInstance(server, "Catania");
//    //addPumpObjectInstance(server, "pump3");
//    //addPumpTypeConstructor(server);
//    //addPumpObjectInstance(server, "pump4");
//    //addPumpObjectInstance(server, "pump5");
//
//    UA_StatusCode retval = UA_Server_run(server, &running);
//
//    UA_Server_delete(server);
//    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
//}