#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

////Define Directly object
//void updateValueCallback(UA_Server* server, void* data);
//void defineWeatherObject(UA_Server* server, char* locatioName);
////Define Type
UA_NodeId defineObjectTypeWeather(UA_Server* server);
//void defInstanceWeather(UA_Server* server, char* locatioName);
 void addWeatherTypeConstructor(UA_Server* server, UA_NodeId typeId);
void defInstanceWeather(UA_Server* server, char* locatioName, UA_NodeId wtype);
 UA_StatusCode WeatherTypeConstructor(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* typeId, void* typeContext,
    const UA_NodeId* nodeId, void** nodeContext, char* cname);


