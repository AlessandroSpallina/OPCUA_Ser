/*
    Publish informations from the information model over UDP multicast using UADP encoding
*/
#include <open62541/plugin/log_stdout.h>

#ifdef __linux__

#include <open62541/plugin/pubsub_ethernet.h>
#include <open62541/plugin/pubsub_udp.h>

#endif

#include "pubsub.h"


// ritorna per riferimento la connectionIdent
void addPubSubConnection(UA_Server *server, UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl, UA_NodeId *connectionIdent, char *connectionName){
    
    #ifdef __linux__
    UA_PubSubConnectionConfig connectionConfig;

    memset(&connectionConfig, 0, sizeof(connectionConfig));
    connectionConfig.name = UA_STRING(connectionName);
    connectionConfig.transportProfileUri = *transportProfile;
    connectionConfig.enabled = UA_TRUE;
    UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl,
                         &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);

    connectionConfig.publisherId.string = UA_STRING(connectionName);
    UA_Server_addPubSubConnection(server, &connectionConfig, connectionIdent);

    #elif _WIN32
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "addPubSubConnection not supported on windows");
    #endif
}

/**
 * **PublishedDataSet handling**
 *
 * The PublishedDataSet (PDS) and PubSubConnection are the toplevel entities and
 * can exist alone. The PDS contains the collection of the published fields. All
 * other PubSub elements are directly or indirectly linked with the PDS or
 * connection. */

// Ritorna "per riferimento" il publishedDataSetIdent
void addPublishedDataSet(UA_Server *server, UA_NodeId *publishedDataSetIdent, char* PDSName) {
    #ifdef __linux__
    /* The PublishedDataSetConfig contains all necessary public
    * informations for the creation of a new PublishedDataSet */
    UA_PublishedDataSetConfig publishedDataSetConfig;
    memset(&publishedDataSetConfig, 0, sizeof(UA_PublishedDataSetConfig));
    publishedDataSetConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
    publishedDataSetConfig.name = UA_STRING(PDSName);
    /* Create new PublishedDataSet based on the PublishedDataSetConfig. */
    UA_Server_addPublishedDataSet(server, &publishedDataSetConfig, publishedDataSetIdent);

    #elif _WIN32
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "addPublishedDataSet not supported on windows");
    #endif
}

/**
 * **DataSetField handling**
 *
 * The DataSetField (DSF) is part of the PDS and describes exactly one published
 * field. */

// Aggiunge come field il campo value della variableId in ingresso
void addDataSetField(UA_Server *server, UA_NodeId publishedDataSetIdent, char *fieldName, UA_NodeId variableID) {
    #ifdef __linux__
    /* Add a field to the previous created PublishedDataSet */
    UA_NodeId dataSetFieldIdent;
    UA_DataSetFieldConfig dataSetFieldConfig;
    memset(&dataSetFieldConfig, 0, sizeof(UA_DataSetFieldConfig));
    dataSetFieldConfig.dataSetFieldType = UA_PUBSUB_DATASETFIELD_VARIABLE;
    dataSetFieldConfig.field.variable.fieldNameAlias = UA_STRING(fieldName);
    dataSetFieldConfig.field.variable.promotedField = UA_FALSE;
    dataSetFieldConfig.field.variable.publishParameters.publishedVariable = variableID;
    dataSetFieldConfig.field.variable.publishParameters.attributeId = UA_ATTRIBUTEID_VALUE;
    UA_Server_addDataSetField(server, publishedDataSetIdent,
                              &dataSetFieldConfig, &dataSetFieldIdent);
    #elif _WIN32
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "addPublishedDataSet not supported on windows");
    #endif
}

/**
 * **WriterGroup handling**
 *
 * The WriterGroup (WG) is part of the connection and contains the primary
 * configuration parameters for the message creation. */

// Ritorna per riferimento la writerGroupIdent
void addWriterGroup(UA_Server *server, UA_NodeId connectionIdent, UA_NodeId *writerGroupIdent, char *writerGroupName) {
    #ifdef __linux__
    /* Now we create a new WriterGroupConfig and add the group to the existing
     * PubSubConnection. */
    UA_WriterGroupConfig writerGroupConfig;
    memset(&writerGroupConfig, 0, sizeof(UA_WriterGroupConfig));
    writerGroupConfig.name = UA_STRING(writerGroupName);
    writerGroupConfig.publishingInterval = 100;
    writerGroupConfig.enabled = UA_FALSE;
    writerGroupConfig.writerGroupId = 100;
    writerGroupConfig.encodingMimeType = UA_PUBSUB_ENCODING_UADP;
    writerGroupConfig.messageSettings.encoding             = UA_EXTENSIONOBJECT_DECODED;
    writerGroupConfig.messageSettings.content.decoded.type = &UA_TYPES[UA_TYPES_UADPWRITERGROUPMESSAGEDATATYPE];
    /* The configuration flags for the messages are encapsulated inside the
     * message- and transport settings extension objects. These extension
     * objects are defined by the standard. e.g.
     * UadpWriterGroupMessageDataType */
    UA_UadpWriterGroupMessageDataType *writerGroupMessage  = UA_UadpWriterGroupMessageDataType_new();
    /* Change message settings of writerGroup to send PublisherId,
     * WriterGroupId in GroupHeader and DataSetWriterId in PayloadHeader
     * of NetworkMessage */
    writerGroupMessage->networkMessageContentMask          = (UA_UadpNetworkMessageContentMask)(UA_UADPNETWORKMESSAGECONTENTMASK_PUBLISHERID |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_GROUPHEADER |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_WRITERGROUPID |
                                                              (UA_UadpNetworkMessageContentMask)UA_UADPNETWORKMESSAGECONTENTMASK_PAYLOADHEADER);
    writerGroupConfig.messageSettings.content.decoded.data = writerGroupMessage;
    UA_Server_addWriterGroup(server, connectionIdent, &writerGroupConfig, writerGroupIdent);
    UA_Server_setWriterGroupOperational(server, *writerGroupIdent);
    UA_UadpWriterGroupMessageDataType_delete(writerGroupMessage);

    #elif _WIN32
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "addWriterGroup not supported on windows");
    #endif

}

/**
 * **DataSetWriter handling**
 *
 * A DataSetWriter (DSW) is the glue between the WG and the PDS. The DSW is
 * linked to exactly one PDS and contains additional informations for the
 * message generation. */
void addDataSetWriter(UA_Server *server, UA_NodeId publishedDataSetIdent, UA_NodeId writerGroupIdent, char *dataSetWriter) {
    #ifdef __linux__
    /* We need now a DataSetWriter within the WriterGroup. This means we must
     * create a new DataSetWriterConfig and add call the addWriterGroup function. */
    UA_NodeId dataSetWriterIdent;
    UA_DataSetWriterConfig dataSetWriterConfig;
    memset(&dataSetWriterConfig, 0, sizeof(UA_DataSetWriterConfig));
    dataSetWriterConfig.name = UA_STRING(dataSetWriter);
    dataSetWriterConfig.dataSetWriterId = 62541;
    dataSetWriterConfig.keyFrameCount = 10;
    UA_Server_addDataSetWriter(server, writerGroupIdent, publishedDataSetIdent,
                               &dataSetWriterConfig, &dataSetWriterIdent);
    #elif _WIN32
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "addDataSetWriter not supported on windows");
    #endif
}
