/*
 * File: InitSqlFunction.c
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

plcbit InitSqlFunction(plcstring* VariableName, struct SQLClient_PAR_typ* SQLParameters)
{
	
	varVariable_typ	PV;

	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".Lookup.Column");
	if(varGetInfo((UDINT)&PV)==0){
		strcpy(SQLParameters->Lookup.Column,(void*)PV.address);
	}
	else{
		return varGetInfo((UDINT)&PV);
	}
	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".Lookup.Table");
	if(varGetInfo((UDINT)&PV)==0){
		strcpy(SQLParameters->Lookup.Table,(void*)PV.address);
	}
	else{
		return varGetInfo((UDINT)&PV);
	}
	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".Lookup.Value");
	if(varGetInfo((UDINT)&PV)==0){
		strcpy(SQLParameters->Lookup.Value,(void*)PV.address);
	}
	else{
		return varGetInfo((UDINT)&PV);
	}
	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".Lookup.FunctionName");
	if(varGetInfo((UDINT)&PV)==0){
		strcpy(SQLParameters->Lookup.FunctionName,(void*)PV.address);
	}
	else{
		return varGetInfo((UDINT)&PV);
	}
	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".Lookup.Arguments");
	if(varGetInfo((UDINT)&PV)==0){
		SQLParameters->Lookup.Arguments.pMapping=		PV.address;
		SQLParameters->Lookup.Arguments.MaxMappings=	PV.dimension;
	}
	memset(&PV,0,sizeof(PV));
	strcpy(	PV.name ,VariableName);
	strcat( PV.name ,".DataMapping");
	if(varGetInfo((UDINT)&PV)==0){
		SQLParameters->DataMapping.pMapping=	PV.address;
		SQLParameters->DataMapping.MaxMappings=	PV.dimension;
	}
	else{
		return varGetInfo((UDINT)&PV);
	}

	return 0;
		
}
