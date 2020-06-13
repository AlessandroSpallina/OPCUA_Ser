#include <open62541/plugin/log_stdout.h>
#include <math.h>

#include "sensors.h"

void getFakeTemperature(UA_Server* server, UA_NodeId node) {
    static UA_Float fakeTemp = 20.0;
    UA_Variant value;

    UA_Float deltaTemperature = (UA_Float)(rand() % 10 / 10.0);
    rand() % 2 ? fmodf((fakeTemp -= deltaTemperature), 100.0) : fmodf((fakeTemp += deltaTemperature), 100.0);

    UA_Variant_setScalar(&value, &fakeTemp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, node, value);
}

void getFakeHumidity(UA_Server* server, UA_NodeId node) {
    static UA_Float fakeHum = 70.0;
    UA_Variant value;

    UA_Float deltaHumidity = (UA_Float)(rand() % 10 / 10.0);
    rand() % 2 ? fabs(fmodf((fakeHum -= deltaHumidity), 100.0)) : fabs(fmodf((fakeHum += deltaHumidity), 100.0));

    UA_Variant_setScalar(&value, &fakeHum, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_writeValue(server, node, value);
}

void beforeReadTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeid, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {

    getFakeTemperature(server, *nodeid);
}

void afterWriteTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Temperature updated");
}

void beforeReadHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeid, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {

    getFakeHumidity(server, *nodeid);
}

void afterWriteHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Humidity updated");
}