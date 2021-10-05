(********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Library: DBLib
 * File: DBLib.typ
 * Author: Josh
 * Created: August 18, 2015
 ********************************************************************
 * Data types of library DBLib
 ********************************************************************)

TYPE
	SQL_DATATYPE : 
		(
		SQL_DATATYPE_USE_PV,
		SQL_DATATYPE_STRING,
		SQL_DATATYPE_NUMBER
		);
	SQL_COLUMN_TYPE : 
		(
		SQL_COLUMN_NAME,
		SQL_COLUMN_VALUE,
		SQL_COLUMN_NAME_VALUE
		);
	SQLClient_SaveMode : 
		(
		DB_SAVE_UPDATE_ONLY,
		DB_SAVE_CREATE_ONLY,
		DB_SAVE_UPDATE_OR_CREATE
		);
	SQLClient_STATE : 
		(
		DB_IDLE,
		DB_LOAD,
		DB_READ_LIST,
		DB_FETCH_DATA,
		DB_FETCH_WAIT_ROW,
		DB_GET_COLUMN_COUNT,
		DB_READ_DATA,
		DB_CREATE_ROW,
		DB_SAVE,
		DB_SAVE_CHECK,
		DB_DELETE,
		DB_DELETE_CHECK,
		DB_EXECUTE_FUNCTION
		);
	SQLClient_VariableMapping_typ :  	STRUCT 
		SQLName :  STRING[80];
		PLCVariableName :  STRING[80];
		DefaultValue :  STRING[80];
		SQLDatatype :  SQL_DATATYPE;
	END_STRUCT;
	SQLClient_typ : 	STRUCT 
		IN : SQLClient_IN_typ;
		OUT : SQLClient_OUT_typ;
		Internal : SQLClient_Internal_typ;
	END_STRUCT;
	SQLClient_IN_typ : 	STRUCT 
		PAR : SQLClient_PAR_typ;
		CMD : SQLClient_CMD_typ;
		CFG : SQLClient_CFG_typ;
	END_STRUCT;
	SQLClient_PAR_typ : 	STRUCT 
		Lookup : SQLClient_SELECT_typ;
		DataMapping : SQLClient_Columns_typ; (*Setup where the data should go*)
	END_STRUCT;
	SQLClient_SELECT_typ : 	STRUCT 
		Table : STRING[80];
		Column : STRING[80]; (*When doing simple SELECT with RecipeName for Open/Save/Delete/FetchList*)
		Value : STRING[80]; (*When doing simple SELECT with RecipeName for Open/Save/Delete/FetchList*)
		FunctionName : STRING[80];
		Arguments : SQLClient_Columns_typ; (*Function to call if "UseFunction" or "ExecuteFunction"*)
	END_STRUCT;
	SQLClient_CFG_Connection_typ : 	STRUCT 
		StaticConnection : BOOL;
		HostName : STRING[80];
		UserName : STRING[80];
		Password : STRING[80];
		DatabaseName : STRING[80];
	END_STRUCT;
	SQLClient_CFG_typ : 	STRUCT 
		Connection : SQLClient_CFG_Connection_typ;
		SaveMode : SQLClient_SaveMode;
	END_STRUCT;
	SQLClient_CMD_typ : 	STRUCT 
		Connect : BOOL;
		ExecuteFunction : BOOL;
		Open : BOOL;
		FetchList : BOOL;
		Save : BOOL;
		Delete : BOOL;
		AckNewRow : BOOL;
		RowFull : BOOL;
		AcknowledgeError : BOOL;
	END_STRUCT;
	SQLClient_OUT_typ : 	STRUCT 
		STAT : SQLClient_STAT_typ;
		NewRow : BOOL;
	END_STRUCT;
	SQLClient_STAT_typ : 	STRUCT 
		Connected : BOOL;
		Done : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[320];
		ErrorDetails : STRING[320];
	END_STRUCT;
	SQLClient_Internal_typ : 	STRUCT 
		SQLString : STRING[320];
		CMD : SQLClient_CMD_typ;
		Busy : BOOL;
		Error : BOOL;
		Connect : dbConnection;
		Open : dbOpen;
		Save : dbSave;
		Delete : dbDelete;
		ExecuteFunction : dbExecuteFunction;
	END_STRUCT;
	SQLClient_Function_typ : 	STRUCT 
		MaxArguments : USINT;
		pArguments : UDINT;
	END_STRUCT;
	SQLClient_Columns_typ : 	STRUCT 
		MaxMappings : USINT;
		pMapping : UDINT;
	END_STRUCT;
	DB_FunctionDef_A10_R10_typ : 	STRUCT 
		Lookup : DB_SelectorDef_10_typ;
		DataMapping : ARRAY[0..9]OF SQLClient_VariableMapping_typ;
	END_STRUCT;
	DB_FunctionDef_A10_R15_typ : 	STRUCT 
		Lookup : DB_SelectorDef_10_typ;
		DataMapping : ARRAY[0..14]OF SQLClient_VariableMapping_typ;
	END_STRUCT;
	DB_FunctionDef_A10_R50_typ : 	STRUCT 
		Lookup : DB_SelectorDef_10_typ;
		DataMapping : ARRAY[0..49]OF SQLClient_VariableMapping_typ;
	END_STRUCT;
	DB_SelectorDef_10_typ : 	STRUCT 
		Table : STRING[80];
		Column : STRING[80]; (*When doing simple SELECT with RecipeName for Open/Save/Delete/FetchList*)
		Value : STRING[80]; (*When doing simple SELECT with RecipeName for Open/Save/Delete/FetchList*)
		FunctionName : STRING[80];
		Arguments :  ARRAY[0..9]OF SQLClient_VariableMapping_typ;
	END_STRUCT;
	SQL_CacheItem_typ : 	STRUCT 
		Declaration : SQL_CacheItem_Wrapping_typ;
		Value : SQL_CacheItem_Wrapping_typ;
		variable : varVariable_typ;
	END_STRUCT;
	SQL_CacheItem_Wrapping_typ : 	STRUCT 
		prefix : STRING[SQL_STRLEN_PREFIX];
		suffix : STRING[SQL_STRLEN_SUFFIX];
	END_STRUCT;
END_TYPE

(*Fub internals*)

TYPE
	SQLConnection_Internal_typ : 	STRUCT 
		Connect : dbConnect;
		Disconnect : dbDisconnect;
		ReadError : dbGetErrorMessage;
	END_STRUCT;
	SQLExecute_Internal_typ : 	STRUCT 
		State : SQLClient_STATE;
		Execute : dbExecuteSql;
		ReadError : dbGetErrorMessage;
		_Execute : BOOL;
		_ConnectionIdent : UDINT;
	END_STRUCT;
	SQLOpen_Internal_typ : 	STRUCT 
		State : SQLClient_STATE;
		Execute : dbExecuteSql;
		ReadError : dbGetErrorMessage;
		GetColumnCount : dbGetColumnCount;
		GetData : dbGetData;
		GetAffectedRows : dbGetAffectedRows;
		GetNextRow : dbFetchNextRow;
		ColumnCount : INT;
		PV : varVariable_typ;
		_Execute : BOOL;
		_ConnectionIdent : UDINT;
	END_STRUCT;
	SQLSave_Internal_typ : 	STRUCT 
		State : SQLClient_STATE;
		Execute : dbExecuteSql;
		ReadError : dbGetErrorMessage;
		GetAffectedRows : dbGetAffectedRows;
		_Execute : BOOL;
		_ConnectionIdent : UDINT;
	END_STRUCT;
	SQLDelete_Internal_typ : 	STRUCT 
		State : SQLClient_STATE;
		Execute : dbExecuteSql;
		ReadError : dbGetErrorMessage;
		GetAffectedRows : dbGetAffectedRows;
		_Execute : BOOL;
		_ConnectionIdent : UDINT;
	END_STRUCT;
END_TYPE
