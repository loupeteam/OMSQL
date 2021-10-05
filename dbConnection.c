
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

void dbConnection(struct dbConnection* inst)
{
	if (inst->Enable){
		if( inst->Ident == 0 && !inst->Error ){

			inst->internal.Connect.databaseSystem = DB_SYSTEM_MS_SQL;
			inst->internal.Connect.pUserName= 	(UDINT)&(inst->Parameters.UserName);
			inst->internal.Connect.pPassword= 	(UDINT)&(inst->Parameters.Password);
			inst->internal.Connect.pServerName= 	(UDINT)&(inst->Parameters.HostName);
			inst->internal.Connect.pDatabaseName=(UDINT)&(inst->Parameters.DatabaseName);

			if( inst->internal.Connect.status == 0 && inst->internal.Connect.enable == 1 ){

				inst->internal.Connect.enable= 	0;
				inst->Busy= 					0;
				inst->Connected= 				1;				
				inst->Ident= 					inst->internal.Connect.connectionIdent;
			}
			else if(inst->internal.Connect.status == ERR_FUB_ENABLE_FALSE){

				inst->internal.Connect.enable = 1;
				inst->Busy= 					1;

			}				
			else if( inst->internal.Connect.status != ERR_FUB_BUSY ){					

				inst->internal.Connect.enable = 0;

				strcpy(inst->ErrorString,"Error connecting to database");

				if( inst->internal.Connect.status == dbERR_DATABASE ){
					inst->Ident= 						inst->internal.Connect.connectionIdent;
					inst->internal.ReadError.enable= 	1;					
				}
				else{
					inst->Error= 	1;
					inst->Busy= 	0;
					inst->ErrorID= inst->internal.Connect.status;
				}
			}	
		}
	}
	else{
		if( inst->Ident != 0 ){
			inst->Busy= 	1;
			if( inst->internal.Disconnect.status == 0 
			|| inst->internal.Disconnect.status == 34932 ){

				inst->Connected= 0;				
				inst->Ident= 0;
				inst->internal.Disconnect.enable= 0;				
			}
			else if( inst->internal.Disconnect.status == ERR_FUB_ENABLE_FALSE){

				inst->internal.Disconnect.enable= 1;				
			}
			else if( inst->internal.Disconnect.status != ERR_FUB_BUSY ){

				inst->Connected= 0;

				inst->internal.Disconnect.enable= 0;				
				strcpy(inst->ErrorString,"Error Disconnecting");

				if( inst->internal.Disconnect.status == 34931 ){
					inst->Ident = 0;
					inst->internal.ReadError.enable= 1;					
				}
				else{

					inst->Ident = 0;
					inst->Error= 1;
					inst->ErrorID= inst->internal.Disconnect.status;				
				}

				inst->Ident= 0;
			}
		}
		else{
			inst->Busy= 	0;
			inst->Error= 	0;
			inst->ErrorID= 	0;				
			strcpy(inst->ErrorString,"");			
		}
	}


	dbConnect(&inst->internal.Connect);

	inst->internal.Disconnect.connectionIdent=	inst->Ident;
	dbDisconnect(&inst->internal.Disconnect);

	inst->internal.ReadError.errorMessageSize=	sizeof( inst->ErrorDetails);
	inst->internal.ReadError.pErrorMessage=		(UDINT)&( inst->ErrorDetails);
	inst->internal.ReadError.connectionIdent=inst->Ident;
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

	if( !inst->Enable){
		inst->Error= 0;
		inst->ErrorID= 0;
		strcpy(inst->ErrorString, "");		
		strcpy(inst->ErrorDetails, "");		
	}

}
