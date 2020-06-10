
#include <open62541/plugin/log_stdout.h>
#include <math.h>
#include "informationmodel.h"
#include "utils.h"

// This function generates fake temperature and humidity
void updateValueCallback(UA_Server* server, void* data) {
        static UA_Float currentTemperature = 20.0, currentHumidity = 50.0;
        char temperatureNodeName[120], humidityNodeName[120];
        UA_Variant value;
        UA_NodeId currentNodeId;

        UA_Float deltaTemperature = (UA_Float) (rand() % 10 / 10.0);
        rand() % 2 ? fmodf((currentTemperature -= deltaTemperature), 50.0) : fmodf((currentTemperature += deltaTemperature), 50.0);

        UA_Float deltaHumidity = (UA_Float) (rand() % 10 / 10.0);
        rand() % 2 ? fabs(fmodf((currentHumidity -= deltaHumidity), 100.0)) : fabs(fmodf((currentHumidity += deltaHumidity), 100.0));

        sprintf(temperatureNodeName, "weather-%s-temperature", (char*)data);
        sprintf(humidityNodeName, "weather-%s-humidity", (char*)data);

        UA_Variant_setScalar(&value, &currentTemperature, &UA_TYPES[UA_TYPES_FLOAT]);
        currentNodeId = UA_NODEID_STRING(1, temperatureNodeName);
        UA_Server_writeValue(server, currentNodeId, value);

        UA_Variant_setScalar(&value, &currentHumidity, &UA_TYPES[UA_TYPES_FLOAT]);
        currentNodeId = UA_NODEID_STRING(1, humidityNodeName);
        UA_Server_writeValue(server, currentNodeId, value);

#if DEBUG
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "updateValueCallback");
#endif
}




UA_NodeId defineObjectTypeWeather(UA_Server* server) {

    UA_NodeId weatherId;
    //Definizione dell'oggetto weatherType

    UA_ObjectTypeAttributes otAttr = UA_ObjectTypeAttributes_default;
    otAttr.description = UA_LOCALIZEDTEXT("en-US", "Weather Type");
    otAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "WeatherType"), otAttr, NULL, &weatherId);

    //Primo attributo - Nome della localita meteo

    UA_NodeId cityNameId;
    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.description = UA_LOCALIZEDTEXT("en-US", "Name of the city for which record Temp/Hum");
    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
    UA_Server_addVariableNode(server, UA_NODEID_NULL,
        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "cityname-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), vAttr, NULL, &cityNameId);
    //variabile obbligatoria
    UA_Server_addReference(server, cityNameId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //Secondo attributo - Temperatura della citta

    UA_NodeId tempId;
    UA_VariableAttributes tmpAttr = UA_VariableAttributes_default;
    tmpAttr.description = UA_LOCALIZEDTEXT("en-US", "TemperatureVariable of City");
    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    UA_Float Tmp = 0.0;
    UA_Variant_setScalar(&tmpAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_addVariableNode(server, UA_NODEID_NULL,
        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "temperature-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, &tempId);
    //variabile obbligatoria
    UA_Server_addReference(server, tempId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //Terzo attributo - Umidità

    UA_NodeId humId;
    UA_VariableAttributes humAttr = UA_VariableAttributes_default;
    humAttr.description = UA_LOCALIZEDTEXT("en-US", "HumidityVariable of City");
    humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
    UA_Float Hum = 0.0;
    UA_Variant_setScalar(&humAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_addVariableNode(server, UA_NODEID_NULL,
        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "humidity-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humAttr, NULL, &humId);
    //variabile obbligatoria
    UA_Server_addReference(server, humId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    return weatherId;
}

UA_NodeId defInstanceWeather(UA_Server* server, char* locatioName, UA_NodeId wtype) {

            UA_NodeId parentNodeIstance; 
            UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
            oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
            oAttr.description = UA_LOCALIZEDTEXT("en-US", "IstanceCity");
            UA_Server_addObjectNode(server, UA_NODEID_NULL,
                                    UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                    UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                    UA_QUALIFIEDNAME(1, locatioName), wtype,
                                    oAttr, NULL, &parentNodeIstance);

            UA_RelativePathElement rpe;
            UA_RelativePathElement_init(&rpe);
            rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
            rpe.isInverse = false;
            rpe.includeSubtypes = false;
            rpe.targetName = UA_QUALIFIEDNAME(1, "cityname-variable");

            UA_BrowsePath bp;
            UA_BrowsePath_init(&bp);
            bp.startingNode = parentNodeIstance;
            bp.relativePath.elementsSize = 1;
            bp.relativePath.elements = &rpe;

            UA_BrowsePathResult bpr =
                UA_Server_translateBrowsePathToNodeIds(server, &bp);

            if (bpr.statusCode != UA_STATUSCODE_GOOD ||
                bpr.targetsSize < 1)
                return UA_NODEID_NULL;

            //Inizializza variabile cityName
            UA_String cityName = UA_STRING(locatioName);
            UA_Variant value;
            UA_Variant_setScalar(&value, &cityName, &UA_TYPES[UA_TYPES_STRING]);
            UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
            UA_BrowsePathResult_clear(&bpr); 
            return parentNodeIstance; 
       }

static UA_StatusCode
readCurrentTemperature(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext,
    UA_Boolean sourceTimeStamp, const UA_NumericRange* range,
    UA_DataValue* dataValue) {

    static UA_Float currentTemperature = 20.0;
    UA_Float deltaTemperature = (UA_Float)(rand() % 10 / 10.0);
    rand() % 2 ? fmodf((currentTemperature -= deltaTemperature), 50.0) : fmodf((currentTemperature += deltaTemperature), 50.0);
    UA_Variant_setScalarCopy(&dataValue->value, &currentTemperature,
        &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD; 
}

static UA_StatusCode
writeCurrentTemperature(UA_Server* server,
    const UA_NodeId* sessionId, void* sessionContext,
    const UA_NodeId* nodeId, void* nodeContext,
    const UA_NumericRange* range, const UA_DataValue* data) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        "Changing the system time is not implemented");
    return UA_STATUSCODE_BADINTERNALERROR;
}

 void addCurrentTemperatureSourceVariable(UA_Server* server, UA_NodeId parentId, char *locatioName) {
    UA_RelativePathElement rpe;
    UA_RelativePathElement_init(&rpe);
    rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
    rpe.isInverse = false;
    rpe.includeSubtypes = false;
    rpe.targetName = UA_QUALIFIEDNAME(1, "temperature-variable");

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = parentId;
    bp.relativePath.elementsSize = 1;
    bp.relativePath.elements = &rpe;

    UA_BrowsePathResult bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);

    if (bpr.statusCode != UA_STATUSCODE_GOOD ||
        bpr.targetsSize < 1)
        return bpr.statusCode;

    //prendi NodeId del nodo variabile temp 
    UA_NodeId tempnodevariabile =  bpr.targets[0].targetId.nodeId; 
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Current time - data source");
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "temp-datasource");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "temp-datasource");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);

    UA_DataSource tempDataSource;
    tempDataSource.read = readCurrentTemperature;
    tempDataSource.write = writeCurrentTemperature;
    UA_Server_addDataSourceVariableNode(server, tempnodevariabile, parentId,
        parentReferenceNodeId, currentName,
        variableTypeNodeId, attr,
        tempDataSource, NULL, NULL);
}