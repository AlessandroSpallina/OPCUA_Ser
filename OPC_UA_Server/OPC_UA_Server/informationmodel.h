#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

#define WEATHER_OBJECTTYPE_QUALIFIEDNAME UA_QUALIFIEDNAME(1, "weather-object-type")
#define CITYNAME_VARIABLE_QUALIFIEDNAME UA_QUALIFIEDNAME(1, "cityname-variable")
#define TEMPERATURE_VARIABLE_QUALIFIEDNAME UA_QUALIFIEDNAME(1, "temperature-variable")
#define HUMIDITY_VARIABLE_QUALIFIEDNAME UA_QUALIFIEDNAME(1, "humidity-variable")

typedef struct exposedNode {
    char* nodeName;
    UA_NodeId nodeId;
} exposedNode_t;

UA_NodeId defineObjectTypeWeather(UA_Server* server);

UA_NodeId instantiateWeatherObject(UA_Server* server, UA_NodeId wtype, char* locatioName);

void addValueCallbackToVariable(UA_Server* server, UA_NodeId variableToUdate,
    void (*beforeReadCallback)(UA_Server*, const UA_NodeId*, void*, const UA_NodeId*, void*, const UA_NumericRange*, const UA_DataValue*),
    void (*afterWriteCallback)(UA_Server*, const UA_NodeId*, void*, const UA_NodeId*, void*, const UA_NumericRange*, const UA_DataValue*));

UA_NodeId findNodeIdByBrowsename(UA_Server* server, UA_NodeId startingNode, UA_QualifiedName qualifiedName);