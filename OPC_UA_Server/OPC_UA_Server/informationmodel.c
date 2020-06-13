#include <open62541/plugin/log_stdout.h>

#include "informationmodel.h"
#include "utils.h"
#include "sensors.h"

// -------------- switch to value callback ----------------------

UA_NodeId defineObjectTypeWeather(UA_Server* server) {
	
    UA_NodeId weatherId;
    
    //Definizione dell'oggetto weatherType
    UA_ObjectTypeAttributes otAttr = UA_ObjectTypeAttributes_default;
    otAttr.description = UA_LOCALIZEDTEXT("en-US", "Weather Type");
    otAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, 0), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), 
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE), UA_QUALIFIEDNAME(1, "weather-object-type"), otAttr, NULL, &weatherId);

    //Primo attributo - Nome della localita meteo
    UA_NodeId cityNameId;
    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.description = UA_LOCALIZEDTEXT("en-US", "Name of the city for which record Temp/Hum");
    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
    vAttr.valueRank = UA_VALUERANK_SCALAR;
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 0), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
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
    tmpAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    tmpAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    tmpAttr.valueRank = UA_VALUERANK_SCALAR;
    UA_Float tmp = 0.0;
    UA_Variant_setScalar(&tmpAttr.value, &tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 0), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "temperature-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEANALOGTYPE), tmpAttr, NULL, &tempId);
    //variabile obbligatoria
    UA_Server_addReference(server, tempId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //Properties della Temperatura
    UA_NodeId temp_range;
    UA_VariableAttributes rangePropAttr = UA_VariableAttributes_default;
    rangePropAttr.displayName = UA_LOCALIZEDTEXT("en-US", "InstrumentRange");
    UA_String rangePropName = UA_STRING("-100:100");
    UA_Variant_setScalar(&rangePropAttr.value, &rangePropName, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_addVariableNode(server, UA_NODEID_NULL, tempId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "RangeTemperatureQualifiedName"), UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), rangePropAttr, NULL, &temp_range);
    //variabile obbligatoria
    UA_Server_addReference(server, temp_range, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    UA_NodeId temp_units;
    UA_VariableAttributes unitOfMisurementPropAttr = UA_VariableAttributes_default;
    unitOfMisurementPropAttr.displayName = UA_LOCALIZEDTEXT("en-US", "EngineeringUnits");
    UA_String unitOfMisurementPropName = UA_STRING("Celsius");
    UA_Variant_setScalar(&unitOfMisurementPropAttr.value, &unitOfMisurementPropName, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_addVariableNode(server, UA_NODEID_NULL, tempId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "UnitOfMisurementQualifiedName"), UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), unitOfMisurementPropAttr, NULL, &temp_units);
    //variabile obbligatoria
    UA_Server_addReference(server, temp_units, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //Terzo attributo - Umidità
    UA_NodeId humId;
    UA_VariableAttributes humAttr = UA_VariableAttributes_default;
    humAttr.description = UA_LOCALIZEDTEXT("en-US", "HumidityVariable of City");
    humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
    humAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    humAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    humAttr.valueRank = UA_VALUERANK_SCALAR;
    UA_Float hum = 0.0;
    UA_Variant_setScalar(&humAttr.value, &hum, &UA_TYPES[UA_TYPES_FLOAT]);

    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 0), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "humidity-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEANALOGTYPE), humAttr, NULL, &humId);
    //variabile obbligatoria
    UA_Server_addReference(server, humId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //Properties dell'Umidità
    UA_NodeId hum_range;
    UA_VariableAttributes unitOfMisurementHumPropAttr = UA_VariableAttributes_default;
    unitOfMisurementHumPropAttr.displayName = UA_LOCALIZEDTEXT("en-US", "InstrumentRange");
    UA_String unitOfMisurementHumPropName = UA_STRING("0:100");
    UA_Variant_setScalar(&unitOfMisurementHumPropAttr.value, &unitOfMisurementHumPropName, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_addVariableNode(server, UA_NODEID_NULL, humId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "RangeTemperatureQualifiedName"), UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), unitOfMisurementHumPropAttr, NULL, &hum_range);
    //variabile obbligatoria
    UA_Server_addReference(server, hum_range, UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    return weatherId;
}

UA_NodeId instantiateWeatherObject(UA_Server* server, UA_NodeId wtype, char* locatioName) {

            UA_NodeId istantiatedObject; 
            UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
            oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
            oAttr.description = UA_LOCALIZEDTEXT("en-US", "IstanceCity");
            UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 0), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                    UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(1, locatioName), wtype,
                                    oAttr, NULL, &istantiatedObject);

            UA_NodeId citynameVariable = findNodeIdByBrowsename(server, istantiatedObject, UA_QUALIFIEDNAME(1, "cityname-variable"));
            
            //Inizializza variabile cityName
            UA_String cityName = UA_STRING(locatioName);
            UA_Variant value;
            UA_Variant_setScalar(&value, &cityName, &UA_TYPES[UA_TYPES_STRING]);
            UA_Server_writeValue(server, citynameVariable, value);

            return istantiatedObject;
 }

void addValueCallbackToVariable(UA_Server* server, UA_NodeId variableToUdate, 
    void (*beforeReadCallback)(UA_Server *, const UA_NodeId *, void *, const UA_NodeId *, void *, const UA_NumericRange *, const UA_DataValue *),
    void (*afterWriteCallback)(UA_Server*, const UA_NodeId*, void*, const UA_NodeId*, void*, const UA_NumericRange*, const UA_DataValue*)) {
    UA_NodeId currentNodeId = variableToUdate;
    UA_ValueCallback callback;
    callback.onRead = (*beforeReadCallback);
    callback.onWrite = (*afterWriteCallback);
    UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}

// ritorna il primo nodo con reference hascomponent a partire da un nodo parent, dato il qualified name
UA_NodeId findNodeIdByBrowsename(UA_Server* server, UA_NodeId startingNode, UA_QualifiedName qualifiedName) {
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