#include <open62541/plugin/log_stdout.h>

#include "informationmodel.h"
#include "utils.h"
#include "sensors.h"


//--------------- datasource --------------

//define callback fore dataSource Humidity and Temp 
//
//UA_StatusCode readCurrentTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
//    const UA_NodeId* nodeId, void* nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange* range, UA_DataValue* dataValue) {
//
//    static UA_Float currentTemperature = 20.0;
//    UA_Float deltaTemperature = (UA_Float)(rand() % 10 / 10.0);
//    rand() % 2 ? fmodf((currentTemperature -= deltaTemperature), 50.0) : fmodf((currentTemperature += deltaTemperature), 50.0);
//    UA_Variant_setScalarCopy(&dataValue->value, &currentTemperature,
//        &UA_TYPES[UA_TYPES_FLOAT]);
//    dataValue->hasValue = true;
//    return UA_STATUSCODE_GOOD;
//}
//
//UA_StatusCode writeCurrentTemperature(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
//    const UA_NodeId* nodeId, void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {
//    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
//        "Changing the system time is not implemented");
//    return UA_STATUSCODE_BADINTERNALERROR;
//}
//
//UA_StatusCode readCurrentHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext,
//    const UA_NodeId* nodeId, void* nodeContext, UA_Boolean sourceTimeStamp, const UA_NumericRange* range, UA_DataValue* dataValue) {
//
//    static UA_Float currentHumidity = 50.0;
//    UA_Float deltaHumidity = (UA_Float)(rand() % 10 / 10.0);
//    rand() % 2 ? fabs(fmodf((currentHumidity -= deltaHumidity), 100.0)) : fabs(fmodf((currentHumidity += deltaHumidity), 100.0));
//    UA_Variant_setScalarCopy(&dataValue->value, &currentHumidity, &UA_TYPES[UA_TYPES_FLOAT]);
//    dataValue->hasValue = true;
//
//    //printf("ciao\n");
//    return UA_STATUSCODE_GOOD;
//}
//
//UA_StatusCode writeCurrentHumidity(UA_Server* server, const UA_NodeId* sessionId, void* sessionContext, const UA_NodeId* nodeId, 
//    void* nodeContext, const UA_NumericRange* range, const UA_DataValue* data) {
//    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Changing the system time is not implemented");
//    return UA_STATUSCODE_BADINTERNALERROR;
//}
//
//// <------ Define ObjectType Weather --> Variable Humidity and Temp Are DataSourceVariable --> they call callbacks to update 
////         variables ---> 
//UA_NodeId defineWeatherObjectAsDataSource(UA_Server* server) {
//    UA_NodeId weatherId;
//    //Definizione dell'oggetto weatherType
//
//    UA_ObjectTypeAttributes otAttr = UA_ObjectTypeAttributes_default;
//    otAttr.description = UA_LOCALIZEDTEXT("en-US", "Weather Type");
//    otAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
//    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, NULL),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
//        UA_QUALIFIEDNAME(1, "WeatherType"), otAttr, NULL, &weatherId);
//
//    //Primo attributo - Nome della localita meteo
//
//    UA_NodeId cityNameId;
//    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
//    vAttr.description = UA_LOCALIZEDTEXT("en-US", "Name of the city for which record Temp/Hum");
//    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
//    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, NULL),
//        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "cityname-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), vAttr, NULL, &cityNameId);
//    //variabile obbligatoria
//    UA_Server_addReference(server, cityNameId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//    //Secondo attributo - Temperatura della citta
//
//    UA_NodeId tempId;
//    UA_VariableAttributes tmpAttr = UA_VariableAttributes_default;
//    tmpAttr.description = UA_LOCALIZEDTEXT("en-US", "TemperatureVariable of City");
//    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
//    tmpAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
//    tmpAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
//    UA_Float Tmp = 0.0;
//    UA_Variant_setScalar(&tmpAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
//    /*UA_Server_addVariableNode(server, UA_NODEID_NULL,
//        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "temperature-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, &tempId);*/
//    UA_DataSource tempDataSource;
//    tempDataSource.read = readCurrentTemperature;
//    tempDataSource.write = writeCurrentTemperature;    
//    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL,
//        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "temperature-variable"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr,
//        tempDataSource, NULL, &tempId);
//    //variabile obbligatoria
//    UA_Server_addReference(server, tempId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//
//    //Terzo attributo - Umidità
//    UA_NodeId humId;
//    UA_VariableAttributes humAttr = UA_VariableAttributes_default;
//    humAttr.description = UA_LOCALIZEDTEXT("en-US", "HumidityVariable of City");
//    humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
//    humAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
//    humAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
//    UA_Float Hum = 0.0;
//    UA_Variant_setScalar(&humAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
//    UA_DataSource humDataSource;
//    humDataSource.read = readCurrentHumidity;
//    humDataSource.write = writeCurrentHumidity;
//    UA_Server_addDataSourceVariableNode(server, UA_NODEID_NULL,
//        weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
//        UA_QUALIFIEDNAME(1, "humidity-variable"),
//        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humAttr,
//        humDataSource, NULL, &humId);
//    //variabile obbligatoria
//    UA_Server_addReference(server, humId,
//        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
//        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
//    return weatherId;
//}
//
////Define Instance of WeatherObject -> set runtime CityName Variable
//UA_NodeId defInstanceWeather(UA_Server* server, char* locatioName, UA_NodeId wtype) {
//            UA_NodeId parentNodeIstance; 
//            UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
//            oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
//            oAttr.description = UA_LOCALIZEDTEXT("en-US", "IstanceCity");
//            UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1,NULL),
//                                    UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
//                                    UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
//                                    UA_QUALIFIEDNAME(1, locatioName), wtype,
//                                    oAttr, NULL, &parentNodeIstance);
//
//            UA_RelativePathElement rpe;
//            UA_RelativePathElement_init(&rpe);
//            rpe.referenceTypeId = UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT);
//            rpe.isInverse = false;
//            rpe.includeSubtypes = false;
//            rpe.targetName = UA_QUALIFIEDNAME(1, "cityname-variable");
//
//            UA_BrowsePath bp;
//            UA_BrowsePath_init(&bp);
//            bp.startingNode = parentNodeIstance;
//            bp.relativePath.elementsSize = 1;
//            bp.relativePath.elements = &rpe;
//
//            UA_BrowsePathResult bpr =
//                UA_Server_translateBrowsePathToNodeIds(server, &bp);
//
//            if (bpr.statusCode != UA_STATUSCODE_GOOD ||
//                bpr.targetsSize < 1)
//                return UA_NODEID_NULL;
//
//            //Inizializza variabile cityName
//            UA_String cityName = UA_STRING(locatioName);
//            UA_Variant value;
//            UA_Variant_setScalar(&value, &cityName, &UA_TYPES[UA_TYPES_STRING]);
//            UA_Server_writeValue(server, bpr.targets[0].targetId.nodeId, value);
//            UA_BrowsePathResult_clear(&bpr); 
//            return parentNodeIstance; 
// }


// -------------- switch to value callback ----------------------

UA_NodeId defineObjectTypeWeather(UA_Server* server) {
	
    UA_NodeId weatherId;
    
    //Definizione dell'oggetto weatherType
    UA_ObjectTypeAttributes otAttr = UA_ObjectTypeAttributes_default;
    otAttr.description = UA_LOCALIZEDTEXT("en-US", "Weather Type");
    otAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, NULL), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), 
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE), UA_QUALIFIEDNAME(1, "weather-object-type"), otAttr, NULL, &weatherId);

    //Primo attributo - Nome della localita meteo
    UA_NodeId cityNameId;
    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.description = UA_LOCALIZEDTEXT("en-US", "Name of the city for which record Temp/Hum");
    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
    vAttr.valueRank = UA_VALUERANK_SCALAR;
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, NULL), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
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
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, NULL), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
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

    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, NULL), weatherId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
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
            UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, NULL), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
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

void addValueCallbackToVariable(UA_Server* server, UA_NodeId variableToUdate, void (*beforeReadCallback)(void), void (*afterWriteCallback)(void)) {
    UA_NodeId currentNodeId = variableToUdate;
    UA_ValueCallback callback;
    //callback.onRead = (*beforeReadCallback);
    //callback.onWrite = (*afterWriteCallback);
    callback.onRead = beforeReadTemperature;
    callback.onWrite = afterWriteTemperature;
    UA_Server_setVariableNode_valueCallback(server, currentNodeId, callback);
}