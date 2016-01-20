#include "monacopkey.h"



const static RB_BYTE AccessCode[] = 
{
	8, 16, 24, 32, 40, 48, 56
};

const static RB_BYTE Answer[MAX_PROTECTED_APP][4] = 
{
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}
};

const static unsigned short Pversion[] = 
{
	0x3478, 0x5545, 0x3789, 0x3457, 0x9878, 0x4234, 0x9075	
};

CMonacoPKey::CMonacoPKey(): CMonacoKey()
{
	_overwritePassword1 = 0xE4EA;
	_overwritePassword2 = 0x370A;
}

CMonacoPKey::~CMonacoPKey()
{

}

BOOL CMonacoPKey::programKey()
{
	SP_STATUS status;

	status = _initPacket( _apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	return TRUE;
}

//programmed to work in the field
BOOL CMonacoPKey::programApp(AppLock *applock, int which)
{
	SP_STATUS status;

	status = _initPacket( _apiPacket);
	if(status != SP_SUCCESS) return FALSE;

/*
	//program option	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address, applock->option, 1 );
	if(status != SP_SUCCESS) return FALSE;

	//program dcount	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address+1, applock->dcount, 1 );
	if(status != SP_SUCCESS) return FALSE;
*/

	//program count	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address+2, applock->count, 0 );
	if(status != SP_SUCCESS) return FALSE;

	unsigned short version = Pversion[which];
	if(!applock->program)
		version = 0x0000;
	//program algo1	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address+3, version, 0 );
	if(status != SP_SUCCESS) return FALSE;

	//program algo2	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address+4, applock->algo2, 0 );
	if(status != SP_SUCCESS) return FALSE;

/*
	//program apass1	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address, applock->apass1, 3 );
	if(status != SP_SUCCESS) return FALSE;

	//program apass2	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, applock->address, applock->apass2, 3 );
	if(status != SP_SUCCESS) return FALSE;

	//get answer
	RB_DWORD	response32 = 0;
	int i;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	for(i = 0; i < 5; i++){
	//	memset(response, 0, SPRO_MAX_QUERY_SIZE+1);
		status = RNBOsproQuery( _apiPacket, 8,
								  (RBP_VOID)applock->query,
								  (RBP_VOID)applock->answer,
								  (RBP_DWORD)&response32,
								  4);
		if(status == SP_SUCCESS) break;
	}

	if(status != SP_SUCCESS)	return FALSE;
*/

	return TRUE;
}

//programmed, but need to activate in the field 
BOOL CMonacoPKey::inactivateApp()
{
	SP_STATUS status;

	status = _initPacket( _apiPacket);
	if(status != SP_SUCCESS) return FALSE;

//	status = RNBOsproWrite( packet,writePassword, address, data, accessCode );
//	if(status != SP_SUCCESS) return FALSE;
	
	status = RNBOsproOverwrite( _apiPacket, _writePassword, _overwritePassword1,
								_overwritePassword2, _appLock->address, _appLock->option, _appLock->acode );
	if(status != SP_SUCCESS) return FALSE;

	return TRUE;
}