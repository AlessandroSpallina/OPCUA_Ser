#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <signal.h>
#include <stdio.h>



static void defineTempVariable(UA_Server* server) {

}



static void defineWeatherObjType(UA_Server* server) {
	UA_NodeId weatherTypeId = UA_NODEID_STRING(1, "Weather-Obj-Type");
	UA_ObjectTypeAttributes wtAttr = UA_ObjectTypeAttributes_default;
	wtAttr.description = UA_LOCALIZEDTEXT("en-US", "Description of WeatherType");
	wtAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
	
	UA_Server_addObjectTypeNode(server, weatherTypeId,		
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
		UA_QUALIFIEDNAME(1, "WeatherType"), wtAttr,
		NULL, NULL);
}

static void
defineWeatherObjectType(UA_Server* server) {
	//definisce il tipo di oggetto Weather
	UA_NodeId weatherTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} };
	UA_ObjectTypeAttributes ptAttr = UA_ObjectTypeAttributes_default;
	ptAttr.displayName = UA_LOCALIZEDTEXT("en-US", "WeatherType");
	UA_Server_addObjectTypeNode(server, weatherTypeId,
		//deviceTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
		UA_QUALIFIEDNAME(1, "WeatherType"), ptAttr,
		NULL, NULL);

	UA_VariableAttributes tempAttr = UA_VariableAttributes_default;
	tempAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
	tempAttr.valueRank = UA_VALUERANK_SCALAR;
	tempAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
	UA_Float tempv = 0.0; //inizialize temp
	UA_Variant_setScalar(&tempAttr.value, &tempv, &UA_TYPES[UA_TYPES_FLOAT]);
	UA_NodeId TempId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
		UA_QUALIFIEDNAME(1, "Temperature"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEANALOGTYPE), tempAttr, NULL, &TempId);
	/* Make the status variable mandatory */
	UA_Server_addReference(server, TempId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	//UA_VariableAttributes humAttr = UA_VariableAttributes_default;
	//humAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Humidity");
	//humAttr.valueRank = UA_VALUERANK_SCALAR;
	//humAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
	//UA_Float humv = 0.0; //inizialize temp
	//UA_Variant_setScalar(&humAttr.value, &humv, &UA_TYPES[UA_TYPES_FLOAT]);
	//UA_NodeId humId;
	//UA_Server_addVariableNode(server, UA_NODEID_NULL, weatherTypeId,
	//	UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
	//	UA_QUALIFIEDNAME(1, "Humidity"),
	//	UA_NODEID_NUMERIC(0, UA_NS0ID_BASEANALOGTYPE), humAttr, NULL, &humId);
	///* Make the status variable mandatory */
	//UA_Server_addReference(server, humId,
	//	UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
	//	UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
}

static void 
defineCityObjectType(UA_Server* server) {
	UA_NodeId weatherTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} };
	UA_NodeId cityTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {50510} };
	//definisce il tipo di oggetto Città
	UA_ObjectTypeAttributes ctAttr = UA_ObjectTypeAttributes_default;
	ctAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CityType");
	UA_Server_addObjectTypeNode(server, cityTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
		UA_QUALIFIEDNAME(1, "CityType"), ctAttr,
		NULL, NULL);
	ctAttr.description = UA_LOCALIZEDTEXT("en-US", "Object Definition of City");

	UA_VariableAttributes nmAttr = UA_VariableAttributes_default; 
	nmAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Name");
	nmAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId; 
	//UA_String namec = UA_STRING_ALLOC(nameCity);
	//UA_Variant_setScalar(&nmAttr.value, &namec, &UA_TYPES[UA_TYPES_STRING]); 
	UA_NodeId cityNameId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "Name"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), nmAttr, NULL, &cityNameId);
	/* Make the manufacturer name mandatory */
	UA_Server_addReference(server, cityNameId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
		UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

	UA_VariableAttributes latAttr = UA_VariableAttributes_default;
	latAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Latitude");
	latAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "Latitude"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), latAttr, NULL, NULL);

	UA_VariableAttributes logAttr = UA_VariableAttributes_default;
	logAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Longitude");
	logAttr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
	UA_Server_addVariableNode(server, UA_NODEID_NULL, cityTypeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_QUALIFIEDNAME(1, "Longitude"),
		UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), latAttr, NULL, NULL); 

	defineWeatherObjectType(server); 

	UA_Server_addReference(server, cityTypeId, UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
		UA_EXPANDEDNODEID_NUMERIC(weatherTypeId.namespaceIndex, weatherTypeId.identifier.numeric), true);
}



static void addCityInstance(UA_Server* server, char* name, UA_NodeId CityTypeId) {

	char* nameObw = "Weather"; 
	UA_NodeId weatherTypeId = { 1, UA_NODEIDTYPE_NUMERIC, {1001} };
	UA_NodeId weatherInstanceId;
	//istanciate wheater
	UA_ObjectAttributes wAttr = UA_ObjectAttributes_default;
	wAttr.displayName = UA_LOCALIZEDTEXT("en-US", nameObw);	
	UA_Server_addObjectNode(server, UA_NODEID_NULL,
		UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
		UA_QUALIFIEDNAME(1, name),
		weatherTypeId, /* this refers to the object typeidentifier -> identificatore del tipo da creare */
		wAttr, NULL, &weatherInstanceId);

	//instanciate city and link weather	
	UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
	oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);	
	UA_NodeId cityInstanceId;		
	    UA_Server_addObjectNode(server, UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, name),
			CityTypeId, /* this refers to the object typeidentifier -> identificatore del tipo da creare */
        oAttr, NULL, &cityInstanceId);	
		
		UA_Variant value; 
		UA_Variant_setScalar(&value, &name, &UA_TYPES[UA_TYPES_STRING]);
		UA_Server_writeValue(server, cityInstanceId, value);
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Ho scritto su value");
		





}

