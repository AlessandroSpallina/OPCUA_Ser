#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdio.h>

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



