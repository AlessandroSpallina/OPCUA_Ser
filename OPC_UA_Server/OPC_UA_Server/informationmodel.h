#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

UA_NodeId defineObjectTypeWeather(UA_Server* server);

UA_NodeId instantiateWeatherObject(UA_Server* server, UA_NodeId wtype, char* locatioName);

void addValueCallbackToVariable(UA_Server* server, UA_NodeId variableToUdate, 
    void (*beforeReadCallback)(void), void (*afterWriteCallback)(void));

//UA_NodeId defInstanceWeather(UA_Server* server, char* locatioName, UA_NodeId wtype);
//UA_NodeId defineWeatherObjectAsDataSource(UA_Server* server);

