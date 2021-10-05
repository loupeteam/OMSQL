
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
void dbSave(struct dbSave* inst)
{

	if( ((inst->ConnectionIdent == 0)
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
			inst->internal._ConnectionIdent=0;			
			if( inst->Execute && !inst->internal._Execute){
				inst->internal._Execute=	1;			
				inst->Busy= 1;
				inst->Done= 0;
				inst->internal.State= DB_SAVE;
				inst->internal._ConnectionIdent=inst->ConnectionIdent;			
			}
			break;

		case DB_SAVE:

			if( inst->internal.Execute.status == ERR_OK ){

				inst->internal.Execute.enable = 0;
			
				inst->internal.State= DB_SAVE_CHECK;

			}else if( inst->internal.Execute.status == ERR_FUB_ENABLE_FALSE ){

				inst->internal.Execute.enable = 1;

			}else if( inst->internal.Execute.status != ERR_FUB_BUSY ){

				inst->internal.State = DB_IDLE;
				inst->internal.Execute.enable = 0;

				inst->Done= 0;

				inst->internal.ReadError.enable= 1;					

			}
			break;

		case DB_SAVE_CHECK:

			if( inst->internal.GetAffectedRows.status == ERR_OK ){

				inst->internal.GetAffectedRows.enable = 0;

				if( inst->internal.GetAffectedRows.affectedRows == 1 ){

					inst->internal.State = DB_IDLE;						

					inst->Busy= 0;
					inst->Done= 1;
				}
				else if( inst->internal.GetAffectedRows.affectedRows == 0 ){


						inst->Busy= 0;
						inst->Done= 0;

						inst->internal.State = DB_IDLE;						
						inst->Error= 1;
						inst->ErrorID= 1;
						strcpy(inst->ErrorString,"No rows were changes.");									
				}
				else{

					inst->Busy= 0;
					inst->Done= 0;

					inst->internal.State = DB_IDLE;				
					//OOPS!						
					inst->Error= 1;
					inst->ErrorID= 1;
					strcpy(inst->ErrorString, "Multiple rows were changed, check database!");		
				}
			}
			else if( inst->internal.GetAffectedRows.status == ERR_FUB_ENABLE_FALSE ){

				inst->internal.GetAffectedRows.enable = 1;
			}
			else if( inst->internal.GetAffectedRows.status != ERR_FUB_BUSY ){

				inst->Done= 0;

				inst->internal.State = DB_IDLE;

				inst->internal.GetAffectedRows.enable = 0;

				if( inst->internal.GetAffectedRows.status == dbERR_NOT_AVAILABLE ){
					inst->Error= 1;
					inst->ErrorID= 1;
					strcpy(inst->ErrorString, "No rows were affected during save.");
				}

				inst->internal.ReadError.enable= 1;					

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
