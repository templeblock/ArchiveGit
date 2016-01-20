/*
// $Workfile: LICDLL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
//    Rev 1.0   14 Aug 1997 15:22:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:28   Fred
// Initial revision.
// 
//    Rev 1.4   13 Jun 1997 15:11:26   Jay
// New licensing return codes
// 
//    Rev 1.3   11 Jun 1997 11:42:56   Jay
// Licensing, etc.
// 
//    Rev 1.1   20 May 1997 16:41:08   johno
// 
//    Rev 1.0   20 May 1997 10:15:06   johno
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
// $Log: /PM8/App/LICDLL.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
//    Rev 1.2   07 May 1996 12:45:10   Jay
// From PMW2
// 
//    Rev 1.3   24 Apr 1996 16:44:44   JAY
// New demo support
// 
//    Rev 1.2   27 Mar 1996 14:59:56   JAY
// Fixed upgrade parent window problem
// 
//    Rev 1.1   12 Feb 1996 14:16:30   JAY
// Added ProductLevel param to Upgrade() call.
// 
//    Rev 1.0   02 Feb 1996 13:10:28   JAY
// Initial revision.
*/ 

#ifndef __LICDLL_H__
#define __LICDLL_H__

//#include "pmwdlg.h"
#include "licdll\licinfo.h"



/////////////////////////////////////////////////////////////////////////////
// CLicenseDll class

class CLicenseDll
{
	typedef LICENSERESULT (FAR PASCAL *FN_LICENSE)(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, const char far *);
	typedef LICENSERESULT (FAR PASCAL *FN_ISLICENSED)(char *,structEncryptedFileInfo *);
	typedef LICENSERESULT (FAR PASCAL *FN_ISLICENSEDBYCODE)(char *);
	typedef BOOL (FAR PASCAL *FN_CANCLOSE)();
	typedef void (FAR PASCAL *FN_BUILDDATAFILE)(BOOL);
	typedef LICENSERESULT (FAR PASCAL *FN_INITIALIZE)(LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR);
	typedef char FAR * (FAR PASCAL *FN_GETLASTDLLERROR)();


public:
	CLicenseDll();														
	virtual ~CLicenseDll();

	LICENSERESULT Startup(LPCSTR pszDllDir, LPCSTR pszLicenseDat, LPCSTR pszOrderFile, LPCSTR pszMsRegIniFile, LPCSTR pszFilters = NULL);

	BOOL CheckForCD(void);

//pure virtuals. see readme.doc.
	virtual LICENSERESULT DoLicense(const CString*, HWND) = NULL;
	virtual void RetrieveFile(const char FAR *) = NULL;

//encrypts the files.  Used only during media build.
	void BuildDataFile(BOOL);


//fns that interface to DLL.
	LICENSERESULT IsLicensed(const char FAR *,structEncryptedFileInfo *);
   LICENSERESULT IsLicensed(const char FAR * szCode);
   BOOL CanClose();
	char * GetLastDllError();

	BOOL IsValid(void) const
		{ return m_hLibrary != 0; }

protected:
	LICENSERESULT License(HWND,CString);

	void Shutdown(void);

	//CString m_csPath;
	//CString m_csOrderPath;
	CString m_csMsregIniFile;
	CString m_csDllName;

	void GetEntryPoints(void);
	
	FN_LICENSE m_fLicense;
//	FN_GETENCRYPTIONINFO m_fGetEncryptionInfo;
	FN_ISLICENSED m_fIsLicensed;
   FN_ISLICENSEDBYCODE m_fIsLicensedByCode;
    FN_CANCLOSE m_fCanClose;
	FN_BUILDDATAFILE m_fBuildDataFile;
	FN_INITIALIZE m_fInitialize;
	HINSTANCE m_hLibrary;
	FN_GETLASTDLLERROR m_fLastDllError;
};

//this class overrides the pure-virtual functions to handle the file operation and errors.
//Derive your own class from CLicenseDll and model the fns after these.
class CPrintMasterLicenseDll:public CLicenseDll
{
public:
	CPrintMasterLicenseDll();
	virtual LICENSERESULT DoLicense(const CString* pcsProductCode = NULL, HWND = NULL);
	virtual void RetrieveFile(const char FAR *);

};



#endif
