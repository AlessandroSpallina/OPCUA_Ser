#pragma once
#include <open62541/types.h>
#include <open62541/server.h>

void addPubSubConnection(UA_Server *server, UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl, UA_NodeId *connectionIdent, char *connectionName);
void addPublishedDataSet(UA_Server *server, UA_NodeId *publishedDataSetIdent, char* PDSName);
void addDataSetField(UA_Server *server, UA_NodeId publishedDataSetIdent, char *fieldName, UA_NodeId variableID);
void addWriterGroup(UA_Server *server, UA_NodeId connectionIdent, UA_NodeId *writerGroupIdent, char *writerGroupName);
void addDataSetWriter(UA_Server *server, UA_NodeId publishedDataSetIdent, UA_NodeId writerGroupIdent, char *dataSetWriter);