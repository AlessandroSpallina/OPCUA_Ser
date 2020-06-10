#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

UA_NodeId defineObjectTypeWeather(UA_Server* server);
UA_NodeId defInstanceWeather(UA_Server* server, char* locatioName, UA_NodeId wtype);
UA_NodeId defineWeatherObjectAsDataSource(UA_Server* server);

