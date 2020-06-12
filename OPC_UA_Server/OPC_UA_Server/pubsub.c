
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/pubsub_udp.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include "pubsub.h"

/*
   Pubblicazione delle informazioni provenienti da Information Model su UDP Multicast utilizzando codifica UADP
 */

// ritorna per riferimento la connectionIdent
void addPubSubConnection(UA_Server *server, UA_String *transportProfile, UA_NetworkAddressUrlDataType *networkAddressUrl, UA_NodeId *connectionIdent, char *connectionName){

    
        UA_PubSubConnectionConfig connectionConfig;

        memset(&connectionConfig, 0, sizeof(connectionConfig));
        connectionConfig.name = UA_STRING(connectionName);
        connectionConfig.transportProfileUri = *transportProfile;
        connectionConfig.enabled = UA_TRUE;
        UA_Variant_setScalar(&connectionConfig.address, networkAddressUrl, &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]);


        // @findme modifiche compatibilità publisher e subscriber
        //connectionConfig.publisherId.string = UA_STRING(connectionName);
        connectionConfig.publisherId.numeric = 2234;
        
        
        UA_Server_addPubSubConnection(server, &connectionConfig, connectionIdent);
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
        /* The PublishedDataSetConfig contains all necessary public
         * informations for the creation of a new PublishedDataSet */
        UA_PublishedDataSetConfig publishedDataSetConfig;
        memset(&publishedDataSetConfig, 0, sizeof(UA_PublishedDataSetConfig));
        publishedDataSetConfig.publishedDataSetType = UA_PUBSUB_DATASET_PUBLISHEDITEMS;
        publishedDataSetConfig.name = UA_STRING(PDSName);
        /* Create new PublishedDataSet based on the PublishedDataSetConfig. */
        UA_Server_addPublishedDataSet(server, &publishedDataSetConfig, publishedDataSetIdent);
}

/**
 * **DataSetField handling**
 *
 * The DataSetField (DSF) is part of the PDS and describes exactly one published
 * field. */

// Aggiunge come field il campo value della variableId in ingresso
void addDataSetField(UA_Server *server, UA_NodeId publishedDataSetIdent, char *fieldName, UA_NodeId variableID) {
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
}

/**
 * **WriterGroup handling**
 *
 * The WriterGroup (WG) is part of the connection and contains the primary
 * configuration parameters for the message creation. */

// Ritorna per riferimento la writerGroupIdent
void addWriterGroup(UA_Server *server, UA_NodeId connectionIdent, UA_NodeId *writerGroupIdent, char *writerGroupName) {
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
}

/**
 * **DataSetWriter handling**
 *
 * A DataSetWriter (DSW) is the glue between the WG and the PDS. The DSW is
 * linked to exactly one PDS and contains additional informations for the
 * message generation. */
void addDataSetWriter(UA_Server *server, UA_NodeId publishedDataSetIdent, UA_NodeId writerGroupIdent, char *dataSetWriter) {
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
}

int configurePubSub(UA_Server* server, UA_ServerConfig* config, UA_String transportProfile, UA_NetworkAddressUrlDataType networkAddressUrl, fieldToPublish fields[], int fieldsCount) {

    config->pubsubTransportLayers = (UA_PubSubTransportLayer*)UA_calloc(2, sizeof(UA_PubSubTransportLayer));
    if (!config->pubsubTransportLayers) {
        UA_Server_delete(server);
        return EXIT_FAILURE;
    }
    config->pubsubTransportLayers[0] = UA_PubSubTransportLayerUDPMP();
    config->pubsubTransportLayersSize++;

    UA_NodeId connectionIdent, publishedDataSetIdent, writerGroupIdent;


    addPubSubConnection(server, &transportProfile, &networkAddressUrl, &connectionIdent, "Connection1");
    addPublishedDataSet(server, &publishedDataSetIdent, "PDS1");

    for (int i = 0; i < fieldsCount; i++) {
        addDataSetField(server, publishedDataSetIdent, fields[i].fieldName, fields[i].variableId);
    }

    addWriterGroup(server, connectionIdent, &writerGroupIdent, "WriterGroup1");
    addDataSetWriter(server, publishedDataSetIdent, writerGroupIdent, "DataSetWriter1");
}