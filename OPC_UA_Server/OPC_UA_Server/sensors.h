#pragma once
#include <open62541/server.h>

void getFakeTemperature(UA_Server* server, UA_NodeId node);

void getFakeHumidity(UA_Server* server, UA_NodeId node);

void beforeReadTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeid, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data);

void afterWriteTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data);

void beforeReadHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeid, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data);

void afterWriteHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data);