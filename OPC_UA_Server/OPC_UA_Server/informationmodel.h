#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

UA_NodeId defineObjectTypeWeather(UA_Server* server);

UA_NodeId instantiateWeatherObject(UA_Server* server, UA_NodeId wtype, char* locatioName);

void addValueCallbackToVariable(UA_Server* server, UA_NodeId variableToUdate,
    void (*beforeReadCallback)(UA_Server*, const UA_NodeId*, void*, const UA_NodeId*, void*, const UA_NumericRange*, const UA_DataValue*),
    void (*afterWriteCallback)(UA_Server*, const UA_NodeId*, void*, const UA_NodeId*, void*, const UA_NumericRange*, const UA_DataValue*));

UA_NodeId findNodeIdByBrowsename(UA_Server* server, UA_NodeId startingNode, UA_QualifiedName qualifiedName);