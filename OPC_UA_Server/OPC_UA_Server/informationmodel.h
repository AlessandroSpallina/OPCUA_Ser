#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

//Define Directly object
void updateValueCallback(UA_Server* server, void* data);
void defineWeatherObject(UA_Server* server, char* locatioName);
//Define Type
void defineObjectTypeWeather(UA_Server* server);
void defInstanceWeather(UA_Server* server, char* locatioName);
