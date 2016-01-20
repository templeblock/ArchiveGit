#include "monacokey.h"
#include "crypt_eve.h"

const static unsigned short CellAddr[] = 
{
	0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x39
};

const static char* AppNames[MAX_PROTECTED_APP] = 
{
	"MonacoPRINT",
	"MonacoSCAN",
	"MonacoVIEW",
	"MonacoCOLOR",
	"PresstoPROOF",
	"",
	""
};

const static unsigned short Version[] =
{
	0x3478, 0x5545, 0x3789, 0x3457, 0x9878, 0x4234, 0x9075	
};

const static unsigned char Codes[] = 
{
	1, 2, 4, 8, 16, 32, 64
};
 
const static char* AppSubNames[MAX_PROTECTED_APP][16] =
{
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	//"4x5",
	//"35mm",
	//"Reflective",
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
	{
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", ""
	},
};
 
CMonacoKey::CMonacoKey()
{
	_numApps = 5;
	_devID = 0x4779;
	_writePassword = 0x18E2;
	_initKey();
}

CMonacoKey::~CMonacoKey()
{

}

void CMonacoKey::_initKey()
{
	int i;

	for( i = 0; i < MAX_PROTECTED_APP; i++)
		_initApp(&_appLock[i]);	

	for( i = 0; i < _numApps; i++)
		_setApp(&_appLock[i], i);
}

void CMonacoKey::_initApp(AppLock *applock)
{
	int i;
	const static RB_BYTE query[] = 
	{0xB5, 0xED, 0xD6, 0xAD};

	applock->assigned = 0;	
	applock->program = 0;	
	applock->activate = 0;
	applock->found = 0;
	applock->option = 0x0000;
	applock->dcount = 0;	
	applock->count = 0;
	applock->algo1 = 0x0000;	
	applock->algo2 = 0x0000;
	applock->apass1 = 0x0000;	
	applock->apass2 = 0x0000;
	applock->address = 0x00000000;
	for(i = 0; i < 4; i++)
	{
		applock->query[i] = query[i];
		applock->answer[i] = 0x00;
	}
	applock->acode = 0x00;
}

void CMonacoKey::_setApp(AppLock *applock, int which)
{

	applock->assigned = 1;	
	applock->address = CellAddr[which];
	memcpy(applock->name, AppNames[which], strlen(AppNames[which])+1);
	for(int i = 0; i < 16; i++)
	{
		if(strlen(AppSubNames[which][i]))
		{
			memcpy(applock->subnames[i], AppSubNames[which][i], strlen(AppSubNames[which][i])+1);
			applock->option += ( 1 << i);
		}

	}
}

//get the serial number
BOOL CMonacoKey::getSn(unsigned short *sn)
{
	SP_STATUS status;
	RB_WORD	data;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproRead(_apiPacket, 0, &data);
	if(status != SP_SUCCESS) return FALSE;
	
	*sn = data;
	return TRUE;
}

SP_STATUS CMonacoKey::_initPacket(RB_SPRO_APIPACKET ApiPacket)
{
	SP_STATUS status;

	status = RNBOsproFormatPacket( ApiPacket, sizeof(RB_SPRO_APIPACKET) );
	if(status != SP_SUCCESS) return status;

	status = RNBOsproInitialize( ApiPacket );
	if(status != SP_SUCCESS) return status;

	status = RNBOsproFindFirstUnit(ApiPacket, _devID);
	if(status != SP_SUCCESS) return FALSE;

	return status;
}

BOOL CMonacoKey::connectKey()
{
	SP_STATUS status;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	return TRUE;
}

BOOL CMonacoKey::readKey()
{
	SP_STATUS status;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	//just read the data, don't concern about the errors
	for(int i = 0; i < _numApps; i++)
		readApp(&_appLock[i], i);	 

	return TRUE;
}

BOOL CMonacoKey::readApp(AppLock *applock, int which)
{
	SP_STATUS status;

	//count
	status = RNBOsproRead(_apiPacket, applock->address+2, &applock->count);
	if(status != SP_SUCCESS) return FALSE;

	//version
	status = RNBOsproRead(_apiPacket, applock->address+3, &applock->algo1);
	if(status != SP_SUCCESS) return FALSE;

	//demo flag
	status = RNBOsproRead(_apiPacket, applock->address+4, &applock->algo2);
	if(status != SP_SUCCESS) return FALSE;

	if(applock->algo1 != Version[which] || applock->count == 0)
		applock->found = 0;
	else
		applock->found = 1;
		
	return TRUE;
}

BOOL CMonacoKey::CheckApp(AppLock *applock, int which)
{
	if(!readApp(applock, which))
		return FALSE;

	if(applock->algo1 != Version[which] || applock->count == 0)
		return FALSE;

	return TRUE;
}

BOOL CMonacoKey::DecApp(AppLock *applock)
{
	if(applock->count == 0)
		return FALSE;

	if(applock->count <= 1500)
		applock->count--;

	return TRUE;
}

//check if the string is good or not
BOOL CMonacoKey::queryApp(AppLock *applock)
{
	SP_STATUS	status;
	RB_BYTE		response[SPRO_MAX_QUERY_SIZE+1];
	RB_DWORD	response32 = 0;
	int i;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	for(i = 0; i < _numApps; i++){
	//	memset(response, 0, SPRO_MAX_QUERY_SIZE+1);
		status = RNBOsproQuery( _apiPacket, 8,
								  (RBP_VOID)applock->query,
								  (RBP_VOID)response,
								  (RBP_DWORD)&response32,
								  4);
		//if(status != SP_SUCCESS) return FALSE;
		if(!memcmp((void*)response, (void*)applock->answer, 4))	return TRUE;
	}

	return FALSE;
}

//activate by Monaco Algorithm, not by Rainbow algorithm
BOOL CMonacoKey::activateKey(unsigned long p1, unsigned long p2)
{
	long	sn; 
	unsigned short ssn;
	unsigned char  codes1, codes2;
	unsigned char  count1, count2;

	if(!getSn(&ssn))	return FALSE;
	sn = (long)ssn;

	if(!decrypt(sn, p1, p2, codes1, codes2, count1, count2)) return FALSE;


	for(int i = 0; i < _numApps; i++)
	{
		if(codes1 & ( 1 << i))
		{
			_appLock[i].program = 1;
			if(count1 == 255)
				_appLock[i].count =  2000;
			else if(count1 == 254)
				_appLock[i].count = 1500;
			else
				_appLock[i].count += 253;
		}
		else
		{
			_appLock[i].program = 0;
		}

		if(!programApp(&_appLock[i], i))
			return FALSE;
	}

	return TRUE;
}

//using password to program, so not compromise the overwritepassword in the field
BOOL CMonacoKey::programApp(AppLock *applock, int which)
{
	SP_STATUS status;

	status = _initPacket( _apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	//program count	
	status = RNBOsproWrite( _apiPacket, _writePassword,
								applock->address+2, applock->count, 0 );
	if(status != SP_SUCCESS) return FALSE;

	unsigned short version = Version[which];
	if(!applock->program)
		version = 0x0000;
	//program algo1	
	status = RNBOsproWrite( _apiPacket, _writePassword, 
								applock->address+3, version, 0 );
	if(status != SP_SUCCESS) return FALSE;

	//program algo2	
	status = RNBOsproWrite( _apiPacket, _writePassword, 
								applock->address+4, applock->algo2, 0 );
	if(status != SP_SUCCESS) return FALSE;

	return TRUE;
}

//activate by Monaco Algorithm, not by Rainbow algorithm
BOOL CMonacoKey::GeneratePassword(long sn,unsigned long* p1,unsigned long* p2)
{
	unsigned char codes1, codes2;
	unsigned char count1, count2;

	codes1 = 0x00;
	codes2 = 0x00;
	count1 = 0x00;
	count2 = 0x00;	
	for(int i = 0; i < _numApps; i++)
	{
		if(_appLock[i].program)
		{
			codes1 += Codes[i];	
			
			if( i == MONACO_COLOR_KEY)
			{
				if(_appLock[i].count <= 253)
					count1 = (unsigned char)_appLock[i].count;
				else if(_appLock[i].count == 1500)
					count1 = 254;
				else
					count1 = 255;
			}
			
			if( i == MONACO_PRINT_KEY)
			{
				if(_appLock[i].count <= 253)
					count2 = (unsigned char)_appLock[i].count;
				else if(_appLock[i].count == 1500)
					count2 = 254;
				else
					count2 = 255;
			}
		}
	}

	encrypt(sn, *p1, *p2, codes1, codes2, count1, count2);

	return TRUE;
}

//if negative number is returned, error occured
long CMonacoKey::decrementCounter()
{
	SP_STATUS status;

	status = _initPacket(_apiPacket);
	if(status != SP_SUCCESS) return FALSE;

	return -1;
}
