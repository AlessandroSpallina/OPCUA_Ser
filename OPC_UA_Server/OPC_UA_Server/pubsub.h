#pragma once
#include <open62541/types.h>
#include <open62541/server.h>
#include <open62541/types_generated.h>

#include "informationmodel.h"

void addPubSubConnection(UA_Server *server, UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl, UA_NodeId *connectionIdent, char *connectionName, int publisherId);
void addPublishedDataSet(UA_Server *server, UA_NodeId *publishedDataSetIdent, char* PDSName);
void addDataSetField(UA_Server *server, UA_NodeId publishedDataSetIdent, char *fieldName, UA_NodeId variableID);
void addWriterGroup(UA_Server* server, UA_NodeId connectionIdent, UA_NodeId* writerGroupIdent, char* writerGroupName, int writerGroupId, int publishingInterval);
void addDataSetWriter(UA_Server* server, UA_NodeId publishedDataSetIdent, UA_NodeId writerGroupIdent, char* dataSetWriterName, int dataSetWriterId);

void addPubSubUdpToServerConfig(UA_Server *server, UA_ServerConfig *config);
void configurePubSubNetworkMessage(UA_Server* server, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, const int fieldsCount, exposedNode_t fieldsToPublish[]);

//void configurePubSub(UA_Server* server, UA_ServerConfig* config, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, fieldToPublish_t fields[], int fieldsCount);