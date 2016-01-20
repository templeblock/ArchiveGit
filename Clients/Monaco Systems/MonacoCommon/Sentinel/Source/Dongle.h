#ifndef Dongle_H
#define Dongle_H

#include "afxwin.h"
#include "spromeps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//	Example code showing how to check the dongle in an napplication
//
//	WORD wVersions, wFeatures;
//	bool bOK;
//	bOK = CDongle::IsMonacoPRINT(&wVersions, &wFeatures);
//	bOK = CDongle::IsMonacoSCAN();
//	bOK = CDongle::IsMonacoVIEW(&wVersions, &wFeatures);
//	bOK = CDongle::IsPrestoProof();

typedef struct{
	bool assigned;		//set if assigned to an application
	bool active;		//set if activated
	int number;
	WORD versions;
	WORD features;
	WORD date;
	WORD count;		//demo count
} AppLock;

//the max # of app can be protected by a key
#define MAX_PROTECTED_APP	7
#define APP_MonacoPRINT		0
#define APP_MonacoSCAN		1
#define APP_MonacoVIEW		2
#define APP_PrestoProof		3

#define OVERWRITE_CODE 0

#define OFFSET_VERSIONS		0
#define OFFSET_FEATURES		1
#define OFFSET_DATE			2
#define OFFSET_COUNT		3
#define OFFSET_ALGORITHM1	4
#define OFFSET_ALGORITHM2	5
#define OFFSET_PASSWORD1	6
#define OFFSET_PASSWORD2	7

const static unsigned short AppAddress[] = 
{
	8, 16, 24, 32, 40, 48, 56,
};

const static DWORD AppAlgorithmDescriptors[] =
{
	0xDC1BCA00,
	0xF278C07B,
	0xE48EE65A,
	0xFEFCCE28,
	0x739DF91A,
	0x4C62EC5A,
	0x0494E0FD,
};

const static DWORD AppAlgorithmPasswords[] =
{
	0x11111111,
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
};

const static DWORD AppAlgorithmQueries[] =
{
	0x12345678,
	0x12345678,
	0x12345678,
	0x12345678,
	0x12345678,
	0x12345678,
	0x12345678,
};

const static DWORD AppAlgorithmResponses[] =
{
	0x17DB53AF,
	0x68209F68,
	0x32C28ACB,
	0xFB263963,
	0xB9F7457C,
	0xFAE6D4A8,
	0x10FD89CC,
};

static char* AppNames[MAX_PROTECTED_APP] = 
{
	/*01*/	"MonacoPRINT",
	/*02*/	"MonacoSCAN",
	/*03*/	"MonacoVIEW",
	/*04*/	"PresstoPROOF",
	/*05*/	"",
	/*06*/	"",
	/*07*/	"",
};

static char* AppVersions[MAX_PROTECTED_APP][16] =
{
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"1.0", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},
};
 
static char* AppFeatures[MAX_PROTECTED_APP][16] =
{
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},	      
	{	/*01*/"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""/*16*/, 
	},
};
 
class CDongle
{
	private:
	WORD	m_devID;
	WORD	m_writePassword;
	WORD	m_overwritePassword1;
	WORD	m_overwritePassword2;

	protected:
	long m_numApps;	//number of locks
	AppLock m_appLock[MAX_PROTECTED_APP];

	public:
	CDongle();
	~CDongle();

	static bool IsMonacoPRINT(LPWORD pVersions = NULL, LPWORD pFeatures = NULL);
	static bool IsMonacoSCAN(LPWORD pVersions = NULL, LPWORD pFeatures = NULL);
	static bool IsMonacoVIEW(LPWORD pVersions = NULL, LPWORD pFeatures = NULL);
	static bool IsPrestoProof(LPWORD pVersions = NULL, LPWORD pFeatures = NULL);

	bool IsApplication(int which, LPWORD pVersions = NULL, LPWORD pFeatures = NULL);
	void UpdateDateAndCount(AppLock* applock);
	WORD GetToday();
	SP_STATUS InitPacket(RB_SPRO_APIPACKET ApiPacket);
	AppLock* GetAppLock(int which);
	bool GetSerialNumber(WORD* sn);
	DWORD GetAlgorithm(AppLock* applock);
	DWORD GetPassword(AppLock* applock);
	bool ReadApp(AppLock* applock);
	bool SetDateAndCount(AppLock* applock);
	bool ProgramApp(AppLock* applock);
};

#endif Dongle_H
