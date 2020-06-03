#include <open62541/plugin/log_stdout.h>/
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/securitypolicy.h>
#include<open62541/plugin/securitypolicy_default.h>
#include<open62541/plugin/accesscontrol_default.h>
#include <signal.h>
#include <stdlib.h>
#include "common.h"

#define UA_ENABLE_ENCRYPTION
static volatile UA_Boolean running = true;//
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}
static UA_UsernamePasswordLogin logins[2] = {
    {UA_STRING_STATIC("Alessandro"), UA_STRING_STATIC("Spallina")},
    {UA_STRING_STATIC("Massimo"), UA_STRING_STATIC("Gollo")}
};

UA_StatusCode retval;
static void
updateCurrentTime(UA_Server* server) {
    UA_DateTime now = UA_DateTime_now();
    UA_Variant value;
    UA_Variant_setScalar(&value, &now, &UA_TYPES[UA_TYPES_DATETIME]);
    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "current-time-value-callback");
    UA_Server_writeValue(server, currentNodeId, value);
}

static void
addCurrentTimeVariable(UA_Server* server) {
    UA_DateTime now = 0;
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Current time - value callback");
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Variant_setScalar(&attr.value, &now, &UA_TYPES[UA_TYPES_DATETIME]);

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "current-time-value-callback");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "current-time-value-callback");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_Server_addVariableNode(server, currentNodeId, parentNodeId,
        parentReferenceNodeId, currentName,
        variableTypeNodeId, attr, NULL, NULL);

    updateCurrentTime(server);
}

/**
 * Variable Value Callback
 * ^^^^^^^^^^^^^^^^^^^^^^^
 *
 * When a value changes continuously, such as the system time, updating the
 * value in a tight loop would take up a lot of resources. Value callbacks allow
 * to synchronize a variable value with an external representation. They attach
 * callbacks to the variable that are executed before every read and after every
 * write operation. */

static void
beforeReadTime(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeid, void* nodeContext,
    const UA_NumericRange* range, const UA_DataValue* data) {
    updateCurrentTime(server);
}

static void
afterWriteTime(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, const UA_DataValue* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        "The variable was updated");
}

static void
addValueCallbackToCurrentTimeVariable(UA_Server* server) {
    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "current-time-value-callback");
    UA_ValueCallback callback;
    callback.onRead = beforeReadTime;
    callback.onWrite = afterWriteTime;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}

/**
 * Variable Data Sources
 * ^^^^^^^^^^^^^^^^^^^^^
 *
 * With value callbacks, the value is still stored in the variable node.
 * So-called data sources go one step further. The server redirects every read
 * and write request to a callback function. Upon reading, the callback provides
 * copy of the current value. Internally, the data source needs to implement its
 * own memory management. */

static UA_StatusCode
readCurrentTime(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext,
    UA_Boolean sourceTimeStamp, const UA_NumericRange* range,
    UA_DataValue* dataValue) {
    UA_DateTime now = UA_DateTime_now();
    UA_Variant_setScalarCopy(&dataValue->value, &now,
        &UA_TYPES[UA_TYPES_DATETIME]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
writeCurrentTime(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, const UA_DataValue* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        "Changing the system time is not implemented");
    return UA_STATUSCODE_BADINTERNALERROR;
}

static void
addCurrentTimeDataSourceVariable(UA_Server* server) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Current time - data source");
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "current-time-datasource");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "current-time-datasource");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);

    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentTime;
    timeDataSource.write = writeCurrentTime;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
        parentReferenceNodeId, currentName,
        variableTypeNodeId, attr,
        timeDataSource, NULL, NULL);
}


int main(int argc, char* argv[]) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    if (argc < 3) {
        UA_LOG_FATAL(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
            "Missing arguments. Arguments are "
            "<server-certificate.der> <private-key.der> "
            "[<trustlist1.crl>, ...]");
        return EXIT_FAILURE;
    }
    /* Load certificate and private key */
    UA_ByteString certificate = loadFile(argv[1]);
    UA_ByteString privateKey = loadFile(argv[2]);

    /* Load the trustlist */
    size_t trustListSize = 0;
    if (argc > 3)
        trustListSize = (size_t)argc - 3;
    UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
    for (size_t i = 0; i < trustListSize; i++)
        trustList[i] = loadFile(argv[i + 3]);

    /* Loading of a issuer list, not used in this application */
    size_t issuerListSize = 0;
    UA_ByteString* issuerList = NULL;

    /* Loading of a revocation list currently unsupported */
    UA_ByteString* revocationList = NULL;
    size_t revocationListSize = 0;

    UA_Server* server = UA_Server_new();
    UA_ServerConfig* config = UA_Server_getConfig(server);

    UA_StatusCode retval =
        UA_ServerConfig_setDefaultWithSecurityPolicies(config, 4840,
            &certificate, &privateKey,
            trustList, trustListSize,
            issuerList, issuerListSize,
            revocationList, revocationListSize);
    UA_ByteString_clear(&certificate);
    UA_ByteString_clear(&privateKey);
    for (size_t i = 0; i < trustListSize; i++)
        UA_ByteString_clear(&trustList[i]);
    if (retval != UA_STATUSCODE_GOOD)
        goto cleanup;


    addCurrentTimeVariable(server);
    addValueCallbackToCurrentTimeVariable(server);
    addCurrentTimeDataSourceVariable(server);

    retval = UA_Server_run(server, &running);

cleanup:
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}