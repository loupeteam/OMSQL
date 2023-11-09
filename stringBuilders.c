/*
 * File: stringBuilders.c
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of OMSQL, licensed under the MIT License.
 */

#include <bur/plctypes.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <string.h>
#include "OMSQL.h"
#ifdef __cplusplus
};
#endif

///////////////////////////////////////
//High Level functions
///////////////////////////////////////
unsigned long dbSelectData(plcstring* Table, plcstring* SQLName, plcstring* Value,unsigned long Arguments, unsigned char maxArgs, unsigned long pOutput){


	dbBuildSelect( Arguments, maxArgs, pOutput);
	dbBuildCriteria( Table, SQLName, Value, pOutput);
	return pOutput;	
}
unsigned long dbUpdateExistingRow(plcstring* Table, plcstring* SQLName, plcstring* Value,unsigned long Arguments, unsigned char maxArgs, unsigned long pOutput){

	dbBuildUpdate(Table,pOutput);
	dbBuildSet(Arguments,maxArgs,pOutput);
	dbBuildCriteria("",SQLName,Value,pOutput);
	return pOutput;	
}
unsigned long dbCreateNewRow(plcstring* Table, plcstring* SQLName, plcstring* Value,unsigned long Arguments, unsigned char maxArgs, unsigned long pOutput){

	dbBuildCreateRow(Table,SQLName,Value,pOutput);
	dbBuildInsert(Table,Arguments,maxArgs,pOutput);
	return pOutput;	
}
unsigned long dbDeleteRow(plcstring* Table, plcstring* SQLName, plcstring* Value, unsigned long pOutput){

	strcat((void*)pOutput, "DELETE ");
	dbBuildCriteria(Table, SQLName, Value, pOutput);
	strcat((void*)pOutput, "\n");
	return pOutput;	
}
unsigned long dbBuildFunction(plcstring* Name, unsigned long Arguments, unsigned char maxArgs, unsigned long pOutput){

	////////////////////////
	//Generate Select String	
	strcpy( (void*)pOutput, "EXECUTE ");
	strcat( (void*)pOutput, Name);
	strcat(	(void*)pOutput, " ");

	dbBuildColumnList(Arguments,maxArgs,SQL_COLUMN_VALUE,pOutput);
	strcat((void*)pOutput, "\n");
	
	return pOutput;
}

///////////////////////////////////////
//Medium? Level functions
///////////////////////////////////////
unsigned long dbCacheTable(plcstring* Name, plcstring* SQLVariableName, unsigned long pVariable, unsigned long pStructureCache){
	

	varVariable_typ *ipVariable = (varVariable_typ*)pVariable;
		
	memset(ipVariable,0,sizeof(varVariable_typ));
	
	strcpy( ipVariable->name, Name );

	STRING structureName[120];
	UINT status = varGetInfo((UDINT)ipVariable);
		
	if( status ==0 ){
		strcpy(structureName,ipVariable->name);		
		if(ipVariable->dataType==VAR_TYPE_ARRAY_OF_STRUCT ){
			appendArrayIndex(0,(UDINT)structureName);
		}		
		dbCacheVariable(structureName,SQLVariableName,pStructureCache);		
	}	

	return status;
	
}
unsigned long dbBuildTable(unsigned long pVariable,unsigned long pCache ,unsigned long pOutput){

	varVariable_typ *ipVariable = (varVariable_typ*)pVariable;

	dbBuildDeclaration( pCache, pOutput);
	strcat((void*)pOutput,"\n");		

	int index;
	if(ipVariable->dataType==VAR_TYPE_ARRAY_OF_STRUCT){
		for(index=0;index<ipVariable->dimension;index++){
			dbBuildInsertRow((pCache),ipVariable->length/ipVariable->dimension,index,pOutput);
			strcat((void*)pOutput,"\n");		
		}	
	}
	else{
		dbBuildInsertRow((pCache),ipVariable->length,0,pOutput);	
	}

	return pOutput;
	
}

unsigned long dbBuildCriteria(plcstring* Table, plcstring* SQLName, plcstring* Value, unsigned long pOutput)
{

	if(Table!=0){	
		if( strcmp(Table,"") != 0 ){
			strcat((void*)pOutput, "FROM ");
			strcat((void*)pOutput, Table );
			strcat((void*)pOutput, "\n");
		}
	}
	strcat((void*)pOutput, "WHERE ");
	strcat((void*)pOutput, SQLName);
	strcat((void*)pOutput, " = \'");
	strcat((void*)pOutput, Value);
	strcat((void*)pOutput, "\'");
	strcat((void*)pOutput, "\n");
	return pOutput;
}
unsigned long dbBuildUpdate(plcstring* Table, unsigned long pOutput)
{
	strcat((void*)pOutput, "UPDATE ");
	strcat((void*)pOutput, Table);
	strcat((void*)pOutput, "\n");
	return pOutput;
}
unsigned long dbBuildCreateRow(plcstring* Table, plcstring* SQLName, plcstring* Value , unsigned long pOutput)
{
	strcpy((void*)pOutput, "if( ! EXISTS (SELECT * ");							
	dbBuildCriteria(Table, SQLName, Value, (UDINT)pOutput);
	strcat((void*)pOutput, ") ");
	return pOutput;						
}
unsigned long dbBuildInsert(plcstring* Table, unsigned long Arguments, unsigned char maxArgs , unsigned long pOutput)
{
	strcat((void*)pOutput, "INSERT INTO ");
	strcat((void*)pOutput, Table);

	strcat((void*)pOutput, " ( ");
	dbBuildColumnList(Arguments,maxArgs,SQL_COLUMN_NAME,(UDINT)pOutput);
	strcat((void*)pOutput, " )");

	strcat((void*)pOutput, " VALUES( ");
	dbBuildColumnList(Arguments,maxArgs,SQL_COLUMN_VALUE,(UDINT)pOutput);
	strcat((void*)pOutput, " )");
	return pOutput;						
}
unsigned long dbBuildSet(unsigned long Arguments, unsigned char maxArgs , unsigned long pOutput)
{
	strcat((void*)pOutput, "SET ");
	dbBuildColumnList(Arguments,maxArgs,SQL_COLUMN_NAME_VALUE,(UDINT)pOutput);
	strcat((void*)pOutput, "\n");
	return pOutput;						

}

unsigned long dbBuildSelect(unsigned long Arguments, unsigned char maxArgs , unsigned long pOutput)
{
	strcat((void*)pOutput, "SELECT ");
	dbBuildColumnList(Arguments,maxArgs,SQL_COLUMN_NAME,(UDINT)pOutput);
	strcat((void*)pOutput, "\n");
	return pOutput;						

}

///////////////////////////////////////
//Low Level functions
///////////////////////////////////////
unsigned long dbBuildColumnList(unsigned long Arguments, unsigned char maxArgs, unsigned long Mode, unsigned long pOutput)
{
	int ItemIndex=0;
	varVariable_typ PV;
	BOOL requireComma = 0;
	for( ItemIndex=0; ItemIndex < maxArgs  ; ItemIndex++){
		if(strcmp(((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLName,"") !=0 ){								
			if( requireComma ){ 				
				strcat((void*)pOutput, ", ");
			}
			requireComma= 1;		
			
			if(Mode== SQL_COLUMN_NAME || Mode==SQL_COLUMN_NAME_VALUE){
				strcat((void*)pOutput, ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLName);			
			}
			if(Mode==SQL_COLUMN_NAME_VALUE){
				strcat((void*)pOutput, "=");
			}
			if(Mode==SQL_COLUMN_NAME_VALUE || Mode==SQL_COLUMN_VALUE){				
				memset(&(PV),0,sizeof(PV));								
				strcpy(PV.name,((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].PLCVariableName);
				if( varGetValue((UDINT)&(PV)) == 0 ){
					if( (((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_USE_PV 
						&& PV.dataType == VAR_TYPE_STRING)
						|| ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_STRING
					){
							strcat((void*)pOutput, "\'");					
					}
					strcat((void*)pOutput, PV.value);										
					if( (((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_USE_PV 
						&& PV.dataType == VAR_TYPE_STRING)
						|| ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_STRING
					){
						strcat((void*)pOutput, "\'");					
					}

				}
				else{
					if( ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_STRING ){
						strcat((void*)pOutput, "\'");					
					}
					strcat((void*)pOutput, ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].DefaultValue);													
					if( ((SQLClient_VariableMapping_typ*)(Arguments))[ItemIndex].SQLDatatype == SQL_DATATYPE_STRING ){
						strcat((void*)pOutput, "\'");					
					}
				}
			}	
		}			
	}
	return pOutput;
}
unsigned long dbBuildDeclaration(unsigned long pCache, unsigned long pOutput){

	int cacheIndex = 0;
		
	while( ((SQL_CacheItem_typ*)pCache)[cacheIndex].variable.address != 0 ){	
		strcat((void*)pOutput,((SQL_CacheItem_typ*)pCache)[cacheIndex].Declaration.prefix);

		char *pch = strrchr(((SQL_CacheItem_typ*)pCache)[cacheIndex].variable.name,'.');
		if(pch != 0){
			strcat((void*)pOutput, pch + 1);
		}
		else{
			pch = strrchr(((SQL_CacheItem_typ*)pCache)[cacheIndex].variable.name,':');
			if(pch != 0){
				strcat((void*)pOutput, pch + 1);
			}
			else{
				strcat((void*)pOutput, ((SQL_CacheItem_typ*)pCache)[cacheIndex].variable.name);			
			}
		}
		strcat((void*)pOutput,((SQL_CacheItem_typ*)pCache)[cacheIndex].Declaration.suffix);
		cacheIndex++;
	}

	return pOutput;
}
unsigned long dbBuildInsertRow(unsigned long pCache, unsigned long structureSize, unsigned long index ,unsigned long pOutput){

	int cacheIndex = 0;
	
	while( ((SQL_CacheItem_typ*)pCache)[cacheIndex].variable.address != 0 ){	

		varVariable_typ member = ((SQL_CacheItem_typ*)pCache)[cacheIndex].variable;
		
		member.address = member.address + structureSize*index;
		varGetValue((UDINT)&member);
		strcat((void*)pOutput,((SQL_CacheItem_typ*)pCache)[cacheIndex].Value.prefix);		
		if(member.dataType == VAR_TYPE_STRING){
			strcat((void*)pOutput, "'");		
		}
		strcat((void*)pOutput, member.value);		
		if(member.dataType == VAR_TYPE_STRING){
			strcat((void*)pOutput, "'");		
		}
		strcat((void*)pOutput,((SQL_CacheItem_typ*)pCache)[cacheIndex].Value.suffix);

		cacheIndex++;

	}

	return pOutput;
}

unsigned long dbCacheVariable(plcstring* Name, plcstring* SQLVariableName, unsigned long pCache){
		
		
	varVariable_typ PV;

	strcpy(PV.name,Name);
	
	UINT status = varGetInfo((UDINT)&PV);
	int memberIndex = 0;
	int cacheIndex = 0;
	STRING memberName[30];
	STRING structureName[60];
	
	BOOL requireComma = 0;
		
	if( status == 0){		

		strcpy(((SQL_CacheItem_typ*)pCache)[cacheIndex].Declaration.prefix, "DECLARE ");	
		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex].Declaration.prefix,  SQLVariableName);
		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex].Declaration.prefix," TABLE(");
			
		strcpy(((SQL_CacheItem_typ*)pCache)[cacheIndex].Value.prefix, "INSERT INTO ");	
		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex].Value.prefix,  SQLVariableName);
		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex].Value.prefix," VALUES(");

		//***********************************************
		// If this is a structure get all the members	*
		//***********************************************
		if(PV.dataType == VAR_TYPE_STRUCT){			

			while( PV_item(PV.name, memberIndex, memberName) == 0 ){

				memset(&(((SQL_CacheItem_typ*)pCache)[cacheIndex].variable),0,sizeof(varVariable_typ));

				strcpy( structureName, Name);				
				strcat( structureName, ".");
				strcat( structureName, memberName);					
				if(dbCacheSimpleVariable(structureName,requireComma,(UDINT)&(((SQL_CacheItem_typ*)pCache)[cacheIndex]))==0){
					requireComma=1;
					cacheIndex++;
				}
				memberIndex++;
			}	
		}
		else{	
			dbCacheSimpleVariable(Name,0,(UDINT)&((SQL_CacheItem_typ*)pCache)[cacheIndex]);
			cacheIndex++;
		}

		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex-1].Declaration.suffix," );");
		strcat(((SQL_CacheItem_typ*)pCache)[cacheIndex-1].Value.suffix," );");
	}

	return status;
	
}	
unsigned long dbCacheSimpleVariable(plcstring* Name, BOOL requireComma, unsigned long pCache){

	memset(&(((SQL_CacheItem_typ*)pCache)->variable),0,sizeof(varVariable_typ));

	strcpy( ((SQL_CacheItem_typ*)pCache)->variable.name, Name);				

	unsigned long status = varGetValue((UDINT)&(((SQL_CacheItem_typ*)pCache)->variable));

	if(  status == 0 ){

		if(requireComma){
			strcpy(((SQL_CacheItem_typ*)pCache)->Declaration.prefix,",");					
			strcpy(((SQL_CacheItem_typ*)pCache)->Value.prefix,",");			
		}

		strcat(((SQL_CacheItem_typ*)pCache)->Declaration.prefix," ");					
		strcat(((SQL_CacheItem_typ*)pCache)->Value.prefix," ");

		strcpy(((SQL_CacheItem_typ*)pCache)->Declaration.suffix," ");										
		dbGetSQLType(((SQL_CacheItem_typ*)pCache)->variable.dataType,(UDINT)((SQL_CacheItem_typ*)pCache)->Declaration.suffix);					
		strcpy(((SQL_CacheItem_typ*)pCache)->Value.suffix," ");					
	}				
	return status;	
}

unsigned long dbGetSQLType(unsigned long type, unsigned long sqltype){
	
	switch( type ){
	
	
		//*******************************************************************************
		// BOOL																			*
		//*******************************************************************************
	
		case VAR_TYPE_BOOL:
			strcat((void*)sqltype,"BOOLEAN");		
			break;
	
	
		//*******************************************************************************
		// Integer types																*
		//*******************************************************************************
	
		case VAR_TYPE_SINT:				
		case VAR_TYPE_INT:
		case VAR_TYPE_USINT:
		case VAR_TYPE_UINT:
			strcat((void*)sqltype,"INTEGER");
			break;

		case VAR_TYPE_DINT:
		case VAR_TYPE_UDINT:
			strcat((void*)sqltype,"BIGINT");
			break;
		
		case VAR_TYPE_TIME:
			strcat((void*)sqltype,"TIME");			
			break;
		
		case VAR_TYPE_DATE_AND_TIME:
			strcat((void*)sqltype,"TIMESTAMP");			
			break;
	
	
		//*******************************************************************************
		// REAL																			*
		//*******************************************************************************
	
		case VAR_TYPE_REAL:
			strcat((void*)sqltype,"REAL");								
			break;
	
	
		//*******************************************************************************
		// STRING																		*
		//*******************************************************************************
	
		case VAR_TYPE_STRING:		
			strcat((void*)sqltype,"VARCHAR(80)");						
			break;

		case VAR_TYPE_LREAL: // maybe do something here //
			strcat((void*)sqltype,"DOUBLE");								
			break;			
		//*******************************************************************************
		// Unsupported and Invalid types												*
		//*******************************************************************************
	
		case VAR_TYPE_STRUCT:
		case VAR_TYPE_ULINT:
		case VAR_TYPE_DATE:
		case VAR_TYPE_ARRAY_OF_STRUCT:
		case VAR_TYPE_TIME_OF_DAY:
		case VAR_TYPE_BYTE:
		case VAR_TYPE_WORD:
		case VAR_TYPE_DWORD:
		case VAR_TYPE_LWORD:
		case VAR_TYPE_WSTRING: // maybe do something here //
		case VAR_TYPE_LINT: 
			strcat((void*)sqltype,"UNSUPPORTED");						
			break;

		default: 
			strcat((void*)sqltype,"UNSUPPORTED");						
			break;	
		
	} // switch(DataType) //	
	
	return (UDINT)strcat;
		
}
