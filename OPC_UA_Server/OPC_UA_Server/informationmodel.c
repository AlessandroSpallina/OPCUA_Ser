
#include <open62541/plugin/log_stdout.h>
#include <math.h>

#include "informationmodel.h"
#include "utils.h"

//Define Directly object


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

void defineWeatherObject(UA_Server* server, char* locatioName) {

    /* get the nodeid assigned by the server */
    UA_NodeId weatherId;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "weatherQualifiedName"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        oAttr, NULL, &weatherId);

    //Primo attributo - Nome della località meteo

    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    UA_String cityName = UA_STRING(locatioName);
    UA_Variant_setScalar(&mnAttr.value, &cityName, &UA_TYPES[UA_TYPES_STRING]);
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
    UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "CityNameQualified"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, NULL);

    //Secondo attributo - Temperatura della città

    UA_VariableAttributes tmpAttr = UA_VariableAttributes_default;
    char tmp[50];
    sprintf(tmp, "weather-%s-temperature", locatioName);

    UA_Float Tmp = 0.0;
    UA_Variant_setScalar(&tmpAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, tmp), weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "TempQUalifiedName"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, NULL);

    //Terzo attributo - Umidità della città 
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "weather-%s-humidity", locatioName);

    UA_VariableAttributes  humAttr = UA_VariableAttributes_default;
    UA_Float hum = 0.0;
    UA_Variant_setScalar(&humAttr.value, &hum, &UA_TYPES[UA_TYPES_FLOAT]);
    humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, tmp), weatherId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "Status"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humAttr, NULL, NULL);
}
//Define Type
void defineObjectTypeWeather(UA_Server* server) {

    //Definizione dell'oggetto weather. Dentro contiene variabli builtin
    UA_ObjectTypeAttributes otAttr = UA_ObjectTypeAttributes_default;
    otAttr.description = UA_LOCALIZEDTEXT("en-US", "Weather Type");
    otAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_STRING(1, "weather-type"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1, "WeatherType"), otAttr, NULL, NULL);

    //Primo attributo - Nome della località meteo

    UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.description = UA_LOCALIZEDTEXT("en-US", "Name of the city for which record Temp/Hum");
    vAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityName");
    //inizialize to zero 
    UA_String nameInizialize = UA_STRING(" ");
    UA_Variant_setScalar(&vAttr.value, &nameInizialize, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, "cityname-variable"),
        UA_NODEID_STRING(1, "weather-type"), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "cityname-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), vAttr, NULL, NULL);

    UA_Server_addReference(server, UA_NODEID_STRING(1, "cityname-variable"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);


    //Secondo attributo - Temperatura della città

    UA_VariableAttributes tmpAttr = UA_VariableAttributes_default;
    tmpAttr.description = UA_LOCALIZEDTEXT("en-US", "TemperatureVariable of City");
    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    UA_Float Tmp = 0.0;
    UA_Variant_setScalar(&tmpAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, "temperature-variable"),
        UA_NODEID_STRING(1, "weather-type"), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "temperature-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, NULL);

    UA_Server_addReference(server, UA_NODEID_STRING(1, "temperature-variable"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    UA_VariableAttributes humAttr = UA_VariableAttributes_default;
    tmpAttr.description = UA_LOCALIZEDTEXT("en-US", "HumidityVariable of City");
    tmpAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
    UA_Float Hum = 0.0;
    UA_Variant_setScalar(&humAttr.value, &Tmp, &UA_TYPES[UA_TYPES_FLOAT]);
    UA_Server_addVariableNode(server, UA_NODEID_STRING(1, "humidity-variable"),
        UA_NODEID_STRING(1, "weather-type"), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
        UA_QUALIFIEDNAME(1, "humidity-variable"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tmpAttr, NULL, NULL);

    UA_Server_addReference(server, UA_NODEID_STRING(1, "humidity-variable"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
        UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
}
void defInstanceWeather(UA_Server* server, char* locatioName) {
    /* get the nodeid assigned by the server */
    //UA_NodeId weatherObjectNodeId = UA_NODEID_STRING(1, "weather-type");
    char c[120];
    sprintf(c, "%s-instance", locatioName);
    //nostro 30100 - id dell'istanza
    UA_NodeId attrNodeId = UA_NODEID_STRING(1, c);
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", locatioName);
    oAttr.description = UA_LOCALIZEDTEXT("en-US", "IstanceCity");
    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "weatherQualifiedName"), UA_NODEID_STRING(1, "weather-type"),
        oAttr, NULL, NULL);

    //TODO capire come accedere alle variabili di un Object istanziato partendo da BaseObjectType proprietario (da questa istanza dice ale)


}

