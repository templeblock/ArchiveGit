#ifndef _UPGRINFO_H
#define _UPGRINFO_H

/*
// Revision History:
//
// $Log: /PM8/App/UPGRINFO.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:24   Fred
// Initial revision.
// 
//    Rev 1.1   11 Jun 1997 11:42:58   Jay
// Licensing, etc.
// 
//    Rev 1.0   19 May 1997 10:58:56   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/UPGRINFO.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   Aaron Evans(AE)  31 March 97 3:57P
//	  Design primarily taken from pmw3pc\pmw\upgrade\upgrade.h.
//    This DLL locks and unlocks files and allows user to purchase files.
*/


#define INFO_TABLE_NAME "MS.INF"
#define ENCRYPTED_FILE_NAME "MS.DAT"
#define MINDSCAPE_NOTICE "(c)1997 Mindscape. All Rights Reserved."

/*
// Event notification function.
*/

enum UpgradeEventNotifications
{
	UPGRADE_EVENT_DialogStartup			= 1,	/* A dialog is starting up. */
	UPGRADE_EVENT_DialogShutdown			= 2,	/* A dialog is shutting down. */
	UPGRADE_EVENT_DialogActivate			= 3,	/* A dialog is being activated. */
	UPGRADE_EVENT_DialogDeactivate		= 4,	/* A dialog is being deactivated. */
	UPGRADE_EVENT_Help						= 5	/* The user has pressed help. */
															/* (dwData is the help value) */
};


typedef void (FAR PASCAL* EVENT_NOTIFY_FUNCTION)(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);


/*
// Register results.
*/

enum UPGRESULT{
	UPGRESULT_Error,
	UPGRESULT_NotUpgraded,
	UPGRESULT_NowUpgraded,
	UPGRESULT_Missing_Critical_File
};

enum LICENSERESULT{
	LICENSERESULT_Not_Licensed,		//File has not been licensed
	LICENSERESULT_Success,			//File is licensed and available in .dat file
	LICENSERESULT_NA,				//Info on this file is not available; The media build did not encrypt this file.
	LICENSERESULT_Missing_Critical_File//a database or other file is missing.
};


// Cheat a little with the 'long' here. Should be 'UPGRESULT'...

#ifdef WIN32
#define UPGENTRYPOINT __declspec(dllexport) UPGRESULT FAR PASCAL
#define LIC_ENTRYPOINT __declspec(dllexport) LICENSERESULT FAR PASCAL
#define VOID_ENTRYPOINT __declspec(dllexport) void FAR PASCAL
#define STR_ENTRYPOINT __declspec(dllexport) char FAR * FAR PASCAL
#else
#define UPGENTRYPOINT UPGRESULT FAR PASCAL _export
#define LIC_ENTRYPOINT LICENSERESULT FAR PASCAL _export
#define VOID_ENTRYPOINT void FAR PASCAL _export
#define STR_ENTRYPOINT char FAR * FAR PASCAL _export
#endif

#define INI_BUF_SIZE 32000
#define ORDER_CODE_LEN 6

struct structEncryptedFileInfo
{
	char m_szCode[ORDER_CODE_LEN];
	long m_lFileOffset;
	long m_lFileLen;
	int m_iMaxRuns;
	int m_iNumRuns;
};

// DLL EXPORTS
extern "C" UPGENTRYPOINT PASCAL FAR Upgrade(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, char FAR *);
extern "C" LIC_ENTRYPOINT IsLicensed(char FAR *,structEncryptedFileInfo * info);
extern "C" LIC_ENTRYPOINT IsLicensedByCode(char FAR *);
//extern "C" LIC_ENTRYPOINT GetEncryptionInfo(char FAR *,structEncryptedFileInfo * info);
extern "C" VOID_ENTRYPOINT BuildDataFile(BOOL bReallyBuild);
extern "C" VOID_ENTRYPOINT Initialize(char FAR *,char FAR *,char FAR *,char FAR *);
extern "C" STR_ENTRYPOINT GetLastDllError();




// AE: This class stores information about files stored in the ms.dat
// file.  ms.dat is a simple linner file where hidden(non-licensed)
// content is stored.
// This class is really a hash-table where strings are the keys.
// Given a file-name key, it returns the Code of the file as well as the offset and
// length in the ms.dat file.
class CEncryptedFileInfoTable:public CMapStringToOb
{
public:
	CEncryptedFileInfoTable(){};
	~CEncryptedFileInfoTable();
	virtual void Serialize(CArchive&);

DECLARE_SERIAL(CEncryptedFileInfoTable);
};



// This is the value given a key.
class CEncryptedFileInfoValue:public CObject
{
public:
	CEncryptedFileInfoValue(){info.m_szCode[0] = '\0';info.m_lFileOffset = NULL,info.m_lFileLen = NULL;info.m_iMaxRuns = -1;info.m_iNumRuns = NULL;}
	CEncryptedFileInfoValue(char FAR * szCode, long lFileOffset, long lFileLen, int MaxRuns, int NumRuns){strcpy(info.m_szCode,szCode);info.m_lFileOffset = lFileOffset;info.m_lFileLen = lFileLen; info.m_iMaxRuns = MaxRuns; info.m_iNumRuns = NumRuns;}
	char FAR * Code(){return info.m_szCode;}
	long FileOffset(){return info.m_lFileOffset;}
	long FileLen(){return info.m_lFileLen;}
	int MaxRuns(){return info.m_iMaxRuns;}
	int NumRuns(){return info.m_iNumRuns;}
	virtual void Serialize(CArchive&);

private:
	structEncryptedFileInfo info;

DECLARE_SERIAL(CEncryptedFileInfoValue);
}; 


#endif 
#pragma hdrstop