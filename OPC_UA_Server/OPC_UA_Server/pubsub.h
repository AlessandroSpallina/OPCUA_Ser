#pragma once
#include <open62541/types.h>
#include <open62541/server.h>
#include <open62541/types_generated.h>

void addPubSubConnection(UA_Server *server, UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl, UA_NodeId *connectionIdent, char *connectionName);
void addPublishedDataSet(UA_Server *server, UA_NodeId *publishedDataSetIdent, char* PDSName);
void addDataSetField(UA_Server *server, UA_NodeId publishedDataSetIdent, char *fieldName, UA_NodeId variableID);
void addWriterGroup(UA_Server *server, UA_NodeId connectionIdent, UA_NodeId *writerGroupIdent, char *writerGroupName);
void addDataSetWriter(UA_Server *server, UA_NodeId publishedDataSetIdent, UA_NodeId writerGroupIdent, char *dataSetWriter);

void configurePubSubUdp(UA_Server* server, UA_ServerConfig* config);
void runPubSub(UA_Server* server, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, const int fieldsCount, exposedNode_t fieldsToPublish[]);

//void configurePubSub(UA_Server* server, UA_ServerConfig* config, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, fieldToPublish_t fields[], int fieldsCount);