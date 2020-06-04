#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

void updateValueCallback(UA_Server* server, void* data);
void defineWeatherObject(UA_Server* server, char* locatioName);