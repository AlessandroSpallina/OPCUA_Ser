#include <open62541/plugin/log_stdout.h>/
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include<open62541/plugin/securitypolicy_default.h>
#include<open62541/plugin/accesscontrol_default.h>
#include <signal.h>
#include <stdlib.h>

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


int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    UA_Server* server = UA_Server_new();
    UA_ServerConfig* config = UA_Server_getConfig(server);
   // UA_ServerConfig_setDefault(config);
   UA_ServerConfig_setMinimal(config,4040,NULL);
    retval = UA_AccessControl_default(config, true,
            &config->securityPolicies[config->securityPoliciesSize - 1].policyUri, 2, logins);
        if (retval != UA_STATUSCODE_GOOD)
            goto cleanup;

   //addCurrentTimeVariable(server);
   //addValueCallbackToCurrentTimeVariable(server);
   //addCurrentTimeDataSourceVariable(server);

    retval = UA_Server_run(server, &running);

cleanup:
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}