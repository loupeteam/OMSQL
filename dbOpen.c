/*
 * File: dbOpen.c
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
void dbOpen(struct dbOpen* inst)
{	
	
	if( ((inst->DataMapping.pMapping == 0)
		|| (inst->ConnectionIdent == 0)
		|| (inst->pSQLString == 0))
		&& inst->Execute
		)
	{
		inst->Error= 1;
		inst->ErrorID= 0;
		strcpy(inst->ErrorString, "Invalid Inputs");		
		strcpy(inst->ErrorDetails, "");		
		inst->internal.State=	DB_IDLE;
		return;
	}

	if( !inst->Execute){
		inst->Error= 0;
		inst->ErrorID= 0;
		inst->Done= 0;
		strcpy(inst->ErrorString, "");		
		strcpy(inst->ErrorDetails, "");		
		inst->internal._Execute=0;
	}

	switch(inst->internal.State) {

		case DB_IDLE:

			inst->Busy= 0;

			inst->internal._ConnectionIdent=	0;

			if( inst->Execute && !inst->internal._Execute){
				inst->internal._Execute=	1;			
				inst->Busy= 1;
				inst->Done= 0;
				inst->internal.State= DB_LOAD;
				inst->internal._ConnectionIdent= inst->ConnectionIdent;
			}
			
			break;
			
		case DB_LOAD:

			if( inst->internal.Execute.status == ERR_OK ){

				inst->internal.Execute.enable = 0;
				inst->internal.State = DB_FETCH_DATA;

			}
			else if( inst->internal.Execute.status == ERR_FUB_ENABLE_FALSE ){

				inst->internal.Execute.enable = 1;

			}else if( inst->internal.Execute.status != ERR_FUB_BUSY ){

				inst->internal.State= 			DB_IDLE;
				inst->internal.Execute.enable=	0;

				if( inst->internal.Execute.dbError == DB_SQL_NO_DATA 
				){
					inst->Busy= 		0;
					inst->Error= 		1;
					inst->ErrorID= 		DB_SQL_NO_DATA;
					strcpy(inst->ErrorString,"Recipe not found");										
				}else{
					inst->internal.ReadError.enable= 1;					
				}
			}
			break;

		case DB_FETCH_DATA:

			if( inst->internal.GetNextRow.status == ERR_OK ){

				inst->internal.GetNextRow.enable = 0;
				inst->internal.State = DB_GET_COLUMN_COUNT;

			}else if( inst->internal.GetNextRow.status == ERR_FUB_ENABLE_FALSE ){

				inst->internal.GetNextRow.enable = 1;

			}else if( inst->internal.GetNextRow.status != ERR_FUB_BUSY ){

				inst->internal.GetNextRow.enable = 0;
				inst->internal.State = DB_IDLE;

				if( inst->internal.GetNextRow.dbError == DB_SQL_NO_DATA ){
						
					inst->Done= 1;

				}else{

					inst->internal.ReadError.enable= 1;
				}

			}	
			break;

		case DB_GET_COLUMN_COUNT:

			if( inst->internal.GetColumnCount.status == ERR_OK ){

				inst->internal.ColumnCount= inst->internal.GetColumnCount.columnCount;

				inst->internal.State = DB_READ_DATA;
			}
			else if( inst->internal.GetColumnCount.status == ERR_FUB_ENABLE_FALSE ){		

				inst->internal.GetColumnCount.enable= 1;
			}
			else if( inst->internal.GetColumnCount.status != ERR_FUB_BUSY ){		

				inst->internal.GetColumnCount.enable= 0;

				inst->internal.ReadError.enable= 1;					

				inst->internal.State = DB_IDLE;

			}	
			break;

		case DB_READ_DATA:

			if( inst->internal.GetData.status == ERR_OK ){

				strcpy(inst->internal.PV.name,((SQLClient_VariableMapping_typ*)(inst->DataMapping.pMapping))[inst->internal.GetData.columnIdx-1].PLCVariableName);
				varSetValue((UDINT)&(inst->internal.PV));
				//reset before reading the next value
				memset(&(inst->internal.PV),0,sizeof(inst->internal.PV));

				inst->internal.GetData.enable = 0;

				if( inst->internal.GetData.columnIdx < inst->DataMapping.MaxMappings &&
					inst->internal.GetData.columnIdx < inst->internal.ColumnCount
				){
					inst->internal.State = DB_READ_DATA;
					inst->internal.GetData.enable = 1;
					inst->internal.GetData.columnIdx= inst->internal.GetData.columnIdx+1;
				}
				else{

					inst->internal.State = DB_FETCH_WAIT_ROW;

				}
			}					
			else if( inst->internal.GetData.status == ERR_FUB_ENABLE_FALSE ){

				inst->internal.GetData.columnIdx= 1;
				inst->internal.GetData.enable=1;
				memset(&(inst->internal.PV),0,sizeof(inst->internal.PV));

			}
			else if( inst->internal.GetData.status != ERR_FUB_BUSY ){

				inst->internal.GetData.enable = 0;
				inst->internal.State = DB_IDLE;


				inst->Done= 0;

				if( inst->internal.GetData.dbError == DB_SQL_NO_DATA ){

					inst->Busy= 0;
					inst->Error= 		1;
					inst->ErrorID= 		DB_SQL_NO_DATA;
					strcpy( inst->ErrorString,"Recipe not found");

				}
				else{

					inst->internal.ReadError.enable= 1;					
				}
			}	

			inst->internal.GetData.pData		=	(UDINT)&(inst->internal.PV.value);
			inst->internal.GetData.dataSize	= 	sizeof(inst->internal.PV.value);
			inst->internal.GetData.dataType	= 	DB_SQL_CHAR;
			break;

		case DB_FETCH_WAIT_ROW:

			inst->DataAvailable = 1;
				
			if( inst->AcknowledgeData ){
				inst->internal.State = DB_FETCH_DATA;
				inst->DataAvailable= 0;
			}
			else if( inst->DataFull ){
				inst->internal.State = DB_IDLE;
				inst->DataAvailable= 0;
				inst->Done= 1;			
			}
			break;
		default:
			break;

	}

	inst->internal.Execute.pSqlStatement = (inst->pSQLString);
	inst->internal.Execute.connectionIdent= inst->internal._ConnectionIdent;
	dbExecuteSql(&inst->internal.Execute);
	
	inst->internal.GetAffectedRows.connectionIdent= inst->internal._ConnectionIdent;
	dbGetAffectedRows(& inst->internal.GetAffectedRows);

	inst->internal.GetData.connectionIdent= inst->internal._ConnectionIdent;
	dbGetData(& inst->internal.GetData);

	inst->internal.GetColumnCount.connectionIdent= inst->internal._ConnectionIdent;
	dbGetColumnCount(& inst->internal.GetColumnCount);

	inst->internal.GetNextRow.connectionIdent= inst->internal._ConnectionIdent;
	dbFetchNextRow(&inst->internal.GetNextRow);
	
	inst->internal.ReadError.errorMessageSize=	sizeof( inst->ErrorDetails);
	inst->internal.ReadError.pErrorMessage=		(UDINT)&( inst->ErrorDetails);
	inst->internal.ReadError.connectionIdent=	inst->internal._ConnectionIdent;
	dbGetErrorMessage(&inst->internal.ReadError);

	if( inst->internal.ReadError.status != ERR_FUB_BUSY 
		&&	inst->internal.ReadError.status != ERR_FUB_ENABLE_FALSE		
		&& inst->internal.ReadError.connectionIdent!=0
	){
		inst->Busy=	0;
		inst->Error= 	1;
		if( inst->internal.ReadError.dbError != 0 ){
			inst->ErrorID=inst->internal.ReadError.dbError;
		}
		else if( inst->internal.ReadError.nativeError != 0 ){
			inst->ErrorID=inst->internal.ReadError.nativeError;
		}	
		inst->internal.ReadError.enable= 0;

	}		
}
