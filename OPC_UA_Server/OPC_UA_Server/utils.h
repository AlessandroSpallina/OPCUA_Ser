#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

#define DEBUG false

UA_ByteString loadFile(const char* path);
UA_NodeId findNodeIdByBrowsename(UA_Server* server, UA_NodeId startingNode, UA_QualifiedName qualifiedName);

//void updateValueCallback(UA_Server* server, void* data);
