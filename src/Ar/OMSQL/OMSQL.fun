(*
 * File: OMSQL.fun
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of OMSQL, licensed under the MIT License.
 *)
 
 (*User Manager function*)

FUNCTION SQLClient : BOOL (*TODO: Add your comment here*)
	VAR_INPUT
		manager : SQLClient_typ;
	END_VAR
END_FUNCTION
(*User Function Blocks*)

FUNCTION_BLOCK dbConnection (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		Parameters : SQLClient_CFG_Connection_typ;
	END_VAR
	VAR_OUTPUT
		Connected : BOOL;
		Busy : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[80];
		ErrorDetails : STRING[320];
		Ident : UDINT;
	END_VAR
	VAR
		internal : SQLConnection_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK dbExecuteFunction (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : BOOL;
		ConnectionIdent : UDINT;
		pSQLString : UDINT;
	END_VAR
	VAR_OUTPUT
		Busy : BOOL;
		Done : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[80];
		ErrorDetails : STRING[320];
	END_VAR
	VAR
		internal : SQLOpen_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK dbOpen (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : BOOL;
		ConnectionIdent : UDINT;
		pSQLString : UDINT;
		DataMapping : SQLClient_Columns_typ;
		DataFull : BOOL;
		AcknowledgeData : BOOL;
	END_VAR
	VAR_OUTPUT
		DataAvailable : BOOL;
		Busy : BOOL;
		Done : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[80];
		ErrorDetails : STRING[320];
	END_VAR
	VAR
		internal : SQLOpen_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK dbSave (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : BOOL;
		ConnectionIdent : UDINT;
		pSQLString : UDINT;
	END_VAR
	VAR_OUTPUT
		Busy : BOOL;
		Done : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[80];
		ErrorDetails : STRING[320];
	END_VAR
	VAR
		internal : SQLSave_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK dbDelete (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : BOOL;
		ConnectionIdent : UDINT;
		pSQLString : UDINT;
	END_VAR
	VAR_OUTPUT
		Busy : BOOL;
		Done : BOOL;
		Error : BOOL;
		ErrorID : UINT;
		ErrorString : STRING[80];
		ErrorDetails : STRING[320];
	END_VAR
	VAR
		internal : SQLDelete_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK
(*High level string builders*)

FUNCTION dbBuildFunction : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Name : STRING[80];
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbSelectData : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		ColumnValue : STRING[80];
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbUpdateExistingRow : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		ColumnValue : STRING[80];
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbCreateNewRow : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		ColumnValue : STRING[80];
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbDeleteRow : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		ColumnValue : STRING[80];
		pOutput : UDINT;
	END_VAR
END_FUNCTION
(*User Functions*)

FUNCTION InitSqlFunction : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		VariableName : STRING[80];
		SQLParameters : SQLClient_PAR_typ;
	END_VAR
END_FUNCTION

FUNCTION dbCacheTable : UDINT
	VAR_INPUT
		Name : STRING[80];
		SQLVariableName : STRING[80];
		pVariable : UDINT;
		pCache : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildTable : UDINT
	VAR_INPUT
		pVariable : UDINT;
		pCache : UDINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildSet : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildSelect : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildInsert : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		Arguments : UDINT;
		maxArgs : USINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildUpdate : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildCreateRow : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		Value : STRING[80];
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildCriteria : UDINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Table : STRING[80];
		ColumnName : STRING[80];
		Value : STRING[80];
		pOutput : UDINT;
	END_VAR
END_FUNCTION
(*Low level string builders*)

FUNCTION dbBuildDeclaration : UDINT
	VAR_INPUT
		pCache : UDINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildInsertRow : UDINT
	VAR_INPUT
		pCache : UDINT;
		structureSize : UDINT;
		index : UDINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbBuildColumnList : UDINT
	VAR_INPUT
		Arguments : UDINT;
		maxArgs : USINT;
		Mode : UDINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION
(*Internal Functions*)

FUNCTION dbCacheSimpleVariable : UDINT
	VAR_INPUT
		Name : STRING[80];
		requireComma : BOOL;
		pCache : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbCacheVariable : UDINT
	VAR_INPUT
		Name : STRING[80];
		SQLVariableName : STRING[80];
		pCache : UDINT;
	END_VAR
END_FUNCTION

FUNCTION dbGetSQLType : UDINT
	VAR_INPUT
		Type : UDINT;
		pOutput : UDINT;
	END_VAR
END_FUNCTION
