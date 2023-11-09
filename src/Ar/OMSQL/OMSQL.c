/*
 * File: OMSQL.c
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
/* TODO: Add your comment here */
plcbit SQLClient(struct SQLClient_typ* manager)
{
	
	if(manager->IN.CMD.Delete ||
		manager->IN.CMD.Save ||
		manager->IN.CMD.ExecuteFunction ||
		manager->IN.CMD.FetchList ||		
		manager->IN.CMD.Open		
	){
		strcpy(manager->Internal.SQLString,"");
		manager->Internal.CMD=	manager->IN.CMD;

		if(strcmp(manager->IN.PAR.Lookup.FunctionName,"")!=0){
			dbBuildFunction(manager->IN.PAR.Lookup.FunctionName,
				manager->IN.PAR.Lookup.Arguments.pMapping,
				manager->IN.PAR.Lookup.Arguments.MaxMappings,
				(UDINT)manager->Internal.SQLString);
		}
		else if(manager->IN.CMD.ExecuteFunction){
			manager->Internal.Error= 1;
			strcpy(manager->OUT.STAT.ErrorString,"No function name given");
		}
		else if(manager->IN.CMD.Save){	
			switch (manager->IN.CFG.SaveMode)
			{
				case DB_SAVE_CREATE_ONLY:
					dbCreateNewRow( manager->IN.PAR.Lookup.Table, 
						manager->IN.PAR.Lookup.Column, 
						manager->IN.PAR.Lookup.Value,
						manager->IN.PAR.DataMapping.pMapping,
						manager->IN.PAR.DataMapping.MaxMappings,
						(UDINT)manager->Internal.SQLString);       
					break;
				case DB_SAVE_UPDATE_ONLY:
					dbUpdateExistingRow( manager->IN.PAR.Lookup.Table, 
						manager->IN.PAR.Lookup.Column, 
						manager->IN.PAR.Lookup.Value,
						manager->IN.PAR.DataMapping.pMapping,
						manager->IN.PAR.DataMapping.MaxMappings,
						(UDINT)manager->Internal.SQLString);       
					break;
				
				default:
					dbCreateNewRow( manager->IN.PAR.Lookup.Table, 
						manager->IN.PAR.Lookup.Column, 
						manager->IN.PAR.Lookup.Value,
						manager->IN.PAR.DataMapping.pMapping,
						manager->IN.PAR.DataMapping.MaxMappings,
						(UDINT)manager->Internal.SQLString);       
					break;
			}
		}
		else if(manager->IN.CMD.Open || manager->IN.CMD.FetchList){
			dbSelectData( manager->IN.PAR.Lookup.Table, 
				manager->IN.PAR.Lookup.Column, 
				manager->IN.PAR.Lookup.Value,
				manager->IN.PAR.DataMapping.pMapping,
				manager->IN.PAR.DataMapping.MaxMappings,
				(UDINT)manager->Internal.SQLString);       		
		}
		else if(manager->IN.CMD.Delete){
			dbDeleteRow( manager->IN.PAR.Lookup.Table, 
				manager->IN.PAR.Lookup.Column, 
				manager->IN.PAR.Lookup.Value,
				(UDINT)manager->Internal.SQLString);       		
		}
	}	
	else if(!manager->OUT.STAT.Busy){		
		manager->Internal.CMD=	manager->IN.CMD;
		manager->Internal.Error=0;		
		strcpy(manager->OUT.STAT.ErrorString,"");
	}
	
	manager->Internal.Connect.Enable= manager->IN.CFG.Connection.StaticConnection 
		|| manager->Internal.CMD.Connect
		|| manager->Internal.CMD.Delete
		|| manager->Internal.CMD.ExecuteFunction
		|| manager->Internal.CMD.FetchList
		|| manager->Internal.CMD.Open
		|| manager->Internal.CMD.Save
		|| manager->Internal.Busy;	
	manager->Internal.Connect.Parameters = 	manager->IN.CFG.Connection;
	dbConnection(&manager->Internal.Connect);

	manager->Internal.Open.Execute=			(manager->Internal.CMD.Open ||manager->Internal.CMD.FetchList)  && manager->Internal.Connect.Connected;
	manager->Internal.Open.ConnectionIdent= manager->Internal.Connect.Ident;
	manager->Internal.Open.pSQLString=		(UDINT)&manager->Internal.SQLString;

	manager->Internal.Open.AcknowledgeData=	(manager->IN.CMD.AckNewRow);
	manager->Internal.Open.DataFull=		(manager->IN.CMD.RowFull ||manager->Internal.CMD.Open);
	manager->Internal.Open.DataMapping= 	manager->IN.PAR.DataMapping;

	dbOpen(&manager->Internal.Open);
	manager->OUT.NewRow= manager->Internal.Open.DataAvailable;
	
	manager->Internal.Save.Execute=			manager->Internal.CMD.Save && manager->Internal.Connect.Connected;
	manager->Internal.Save.ConnectionIdent=	manager->Internal.Connect.Ident;
	manager->Internal.Save.pSQLString=		(UDINT)&manager->Internal.SQLString;

	dbSave(&manager->Internal.Save);
	
	manager->Internal.Delete.Execute=			manager->Internal.CMD.Delete && manager->Internal.Connect.Connected;
	manager->Internal.Delete.ConnectionIdent=	manager->Internal.Connect.Ident;
	manager->Internal.Delete.pSQLString=		(UDINT)&manager->Internal.SQLString;

	dbDelete(&manager->Internal.Delete);
	
	manager->Internal.ExecuteFunction.Execute=			manager->Internal.CMD.ExecuteFunction && manager->Internal.Connect.Connected;
	manager->Internal.ExecuteFunction.ConnectionIdent=	manager->Internal.Connect.Ident;
	manager->Internal.ExecuteFunction.pSQLString=		(UDINT)&manager->Internal.SQLString;
	
	dbExecuteFunction(&manager->Internal.ExecuteFunction);
	
	manager->OUT.STAT.Error= manager->Internal.ExecuteFunction.Error ||
		manager->Internal.Connect.Error ||
		manager->Internal.Delete.Error ||
		manager->Internal.Open.Error ||
		manager->Internal.Save.Error ||
		manager->Internal.Error;

	manager->Internal.Busy= (manager->Internal.ExecuteFunction.Busy ||
		manager->Internal.Delete.Busy ||
		manager->Internal.Open.Busy ||
		manager->Internal.Save.Busy) &&
		!manager->OUT.STAT.Error;		

	manager->OUT.STAT.Busy= manager->Internal.Connect.Busy || manager->Internal.Busy;
	
	manager->OUT.STAT.Done= (manager->Internal.ExecuteFunction.Done ||
		manager->Internal.Delete.Done ||
		manager->Internal.Open.Done ||
		manager->Internal.Save.Done) &&
		(!manager->OUT.STAT.Busy && !manager->OUT.STAT.Error);
	
	manager->OUT.STAT.Connected=	manager->Internal.Connect.Connected;
	
	if(manager->Internal.Connect.Error){
		manager->OUT.STAT.ErrorID = manager->Internal.Connect.ErrorID;
		strcpy(manager->OUT.STAT.ErrorString,manager->Internal.Connect.ErrorString);
		strcpy(manager->OUT.STAT.ErrorDetails,manager->Internal.Connect.ErrorDetails);
	}
	else if(manager->Internal.Delete.Error){
		manager->OUT.STAT.ErrorID = manager->Internal.Delete.ErrorID;
		strcpy(manager->OUT.STAT.ErrorString,manager->Internal.Delete.ErrorString);
		strcpy(manager->OUT.STAT.ErrorDetails,manager->Internal.Delete.ErrorDetails);	
	}
	else if(manager->Internal.ExecuteFunction.Error){
		manager->OUT.STAT.ErrorID = manager->Internal.ExecuteFunction.ErrorID;
		strcpy(manager->OUT.STAT.ErrorString,manager->Internal.ExecuteFunction.ErrorString);
		strcpy(manager->OUT.STAT.ErrorDetails,manager->Internal.ExecuteFunction.ErrorDetails);	
	}
	else if(manager->Internal.Open.Error){
		manager->OUT.STAT.ErrorID = manager->Internal.Open.ErrorID;
		strcpy(manager->OUT.STAT.ErrorString,manager->Internal.Open.ErrorString);
		strcpy(manager->OUT.STAT.ErrorDetails,manager->Internal.Open.ErrorDetails);	
	}
	else if(manager->Internal.Save.Error){
		manager->OUT.STAT.ErrorID = manager->Internal.Save.ErrorID;
		strcpy(manager->OUT.STAT.ErrorString,manager->Internal.Save.ErrorString);
		strcpy(manager->OUT.STAT.ErrorDetails,manager->Internal.Save.ErrorDetails);	
	}
	
	
	return 0;


}
