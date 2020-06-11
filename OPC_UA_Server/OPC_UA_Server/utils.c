#include "utils.h"

#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <open62541/server.h>

/* loadFile parses the certificate file.
 *
 * @param  path               specifies the file name given in argv[]
 * @return Returns the file content after parsing */
UA_ByteString loadFile(const char *path) {

        UA_ByteString fileContents = UA_STRING_NULL;

        /* Open the file */
        FILE* fp = fopen(path, "rb");
        if (!fp) {
                errno = 0; /* We read errno also from the tcp layer... */
                return fileContents;
        }

        /* Get the file length, allocate the data and read */
        fseek(fp, 0, SEEK_END);
        fileContents.length = (size_t)ftell(fp);
        fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
        if (fileContents.data) {
                fseek(fp, 0, SEEK_SET);
                size_t read = fread(fileContents.data, sizeof(UA_Byte), fileContents.length, fp);
                if (read != fileContents.length)
                        UA_ByteString_clear(&fileContents);
        }
        else {
                fileContents.length = 0;
        }
        fclose(fp);

        return fileContents;
}

// This function generates fake temperature and humidity
//void updateValueCallback(UA_Server* server, void* data) {
//    static UA_Float currentTemperature = 20.0, currentHumidity = 50.0;
//    char temperatureNodeName[120], humidityNodeName[120];
//    UA_Variant value;
//    UA_NodeId currentNodeId;
//
//    UA_Float deltaTemperature = (UA_Float)(rand() % 10 / 10.0);
//    rand() % 2 ? fmodf((currentTemperature -= deltaTemperature), 50.0) : fmodf((currentTemperature += deltaTemperature), 50.0);
//
//    UA_Float deltaHumidity = (UA_Float)(rand() % 10 / 10.0);
//    rand() % 2 ? fabs(fmodf((currentHumidity -= deltaHumidity), 100.0)) : fabs(fmodf((currentHumidity += deltaHumidity), 100.0));
//
//    sprintf(temperatureNodeName, "weather-%s-temperature", (char*)data);
//    sprintf(humidityNodeName, "weather-%s-humidity", (char*)data);
//
//    UA_Variant_setScalar(&value, &currentTemperature, &UA_TYPES[UA_TYPES_FLOAT]);
//    currentNodeId = UA_NODEID_STRING(1, temperatureNodeName);
//    UA_Server_writeValue(server, currentNodeId, value);
//
//    UA_Variant_setScalar(&value, &currentHumidity, &UA_TYPES[UA_TYPES_FLOAT]);
//    currentNodeId = UA_NODEID_STRING(1, humidityNodeName);
//    UA_Server_writeValue(server, currentNodeId, value);
//
//#if DEBUG
//    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "updateValueCallback");
//#endif
//}


UA_NodeId findNodeIdByBrowsename(UA_Server *server, UA_NodeId startingNode, UA_QualifiedName qualifiedName) {
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = qualifiedName;

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = startingNode;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr = UA_Server_translateBrowsePathToNodeIds(server, &bp);

    if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
        return UA_NODEID_NULL;

    UA_NodeId toReturn = bpr.targets[0].targetId.nodeId;
    UA_BrowsePathResult_clear(&bpr);

    return toReturn;
}