/*
// $Workfile: LICDLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 2     5/26/98 2:02p Dennis
// Added call to GetConfiguration()->ReplaceText() so app. name can be
// substituted in string.
// 
//    Rev 1.0   14 Aug 1997 15:22:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:02   Fred
// Initial revision.
// 
//    Rev 1.7   13 Jun 1997 15:11:26   Jay
// New licensing return codes
// 
//    Rev 1.6   11 Jun 1997 11:42:54   Jay
// Licensing, etc.
// 
//    Rev 1.4   21 May 1997 10:39:18   johno
// 
//    Rev 1.3   21 May 1997 09:21:12   Jay
// Commented out GetLastError nonsense.
// 
//    Rev 1.2   20 May 1997 16:41:18   johno
// 
//    Rev 1.1   20 May 1997 11:55:30   Jay
// 
//    Rev 1.0   20 May 1997 11:16:28   johno
// Initial revision.
//
//   Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 2     5/26/98 2:02p Dennis
// Added call to GetConfiguration()->ReplaceText() so app. name can be
// substituted in string.
// 
//    Rev 1.2   07 May 1996 12:45:10   Jay
// From PMW2
// 
//    Rev 1.4   24 Apr 1996 16:44:46   JAY
// New demo support
// 
//    Rev 1.3   28 Mar 1996 10:51:32   FRED
//  
// 
//    Rev 1.2   27 Mar 1996 14:59:58   JAY
// Fixed upgrade parent window problem
// 
//    Rev 1.1   12 Feb 1996 14:16:30   JAY
// Added ProductLevel param to Upgrade() call.
// 
//    Rev 1.0   02 Feb 1996 13:10:28   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "resource.h"
#include <afxdisp.h>
#include "util.h"
#include "licdll.h"
#include "pmwcfg.h"


#ifndef WIN32
#include <auxdata.h>       // for _AfxGetSafeOwner()
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef WIN32
static char BASED_CODE cbDllName[] = "LIC32.DLL";
#else                           
static char BASED_CODE cbDllName[] = "LIC16.DLL";
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseDll
CLicenseDll::CLicenseDll()
{
   m_hLibrary = NULL;
   m_fLicense = NULL;
// m_fGetEncryptionInfo = NULL;
   m_fBuildDataFile = NULL;
   m_fIsLicensed = NULL;
   m_fIsLicensed = NULL;
}

CLicenseDll::~CLicenseDll()
{
   Shutdown();
}

BOOL CLicenseDll::CheckForCD(void)
{
	while (!Util::FileExists(m_csDllName))
	{
		// Prompt for the original CD.
      CString  csResource;
      csResource.LoadString(IDS_LICENSE_INSERT_CD);
   	GetConfiguration()->ReplaceText(csResource);
		if (AfxMessageBox(csResource, MB_OKCANCEL|MB_ICONSTOP) == IDCANCEL)
		{
			return FALSE;
		}
	}
	return TRUE;
}

LICENSERESULT CLicenseDll::Startup(LPCSTR pszDllDir, LPCSTR pszLicenseDat, LPCSTR pszOrderFile, LPCSTR pszMsregIniFile, LPCSTR pszFilters /*=NULL*/)
{
   Shutdown();

   m_csMsregIniFile = pszMsregIniFile;

// Generate the library name.

   CString csLibName;

   UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);

   CString csPath;
	Util::ConstructPath(csPath, pszDllDir, cbDllName);

#ifdef WIN32
   char cbCWD [_MAX_PATH];
   GetCurrentDirectory (sizeof(cbCWD), cbCWD);

   CString csDir = pszDllDir;
   Util::RemoveBackslashFromPath(csDir);
   SetCurrentDirectory(csDir);
#endif

	m_csDllName = csPath;
	m_hLibrary = LoadLibrary(csPath);

   SetErrorMode(wOldSem);

#ifdef WIN32
   SetCurrentDirectory(cbCWD);
#endif

   if (m_hLibrary < (HINSTANCE)HINSTANCE_ERROR)
   {
      m_hLibrary = NULL;
		return LICENSERESULT_DLLNotFound;
   }

   GetEntryPoints();

   if (!IsValid())
	{
		return LICENSERESULT_InvalidDLL;
	}

	// Try to initialize.
	LICENSERESULT Result = m_fInitialize(pszDllDir, pszLicenseDat, pszOrderFile, pszMsregIniFile, pszFilters);
	if (Result != LICENSERESULT_Success)
	{
		// Failed. Clean up.
		Shutdown();
	}

   return Result;
}

void CLicenseDll::Shutdown(void)
{
   if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
   {
      FreeLibrary(m_hLibrary);
      m_hLibrary = NULL;
   }

   m_fLicense = NULL;
   m_fIsLicensed = NULL;
   m_fIsLicensedByCode = NULL;
   m_fCanClose = NULL;
   m_fBuildDataFile = NULL;
   m_fInitialize = NULL;
   m_hLibrary = NULL;
   m_fLastDllError = NULL;

}

/*
// Set up the entry points.
*/

void CLicenseDll::GetEntryPoints(void)
{
   if (m_hLibrary != NULL)
   {

#ifdef WIN32
      m_fLicense = (FN_LICENSE)GetProcAddress(m_hLibrary, "License");
#else
      m_fLicense = (FN_LICENSE)GetProcAddress(m_hLibrary, "LICENSE");
#endif


/*#ifdef WIN32
      m_fGetEncryptionInfo = (FN_GETENCRYPTIONINFO)GetProcAddress(m_hLibrary, "GetEncryptionInfo");
#else
      m_fGetEncryptionInfo = (FN_GETENCRYPTIONINFO)GetProcAddress(m_hLibrary, "GETENCRYPTIONINFO");
#endif */

#ifdef WIN32
      m_fIsLicensed = (FN_ISLICENSED)GetProcAddress(m_hLibrary, "IsLicensed");
#else
      m_fIsLicensed = (FN_ISLICENSED)GetProcAddress(m_hLibrary, "ISLICENSED");
#endif

#ifdef WIN32
      m_fIsLicensedByCode = (FN_ISLICENSEDBYCODE)GetProcAddress(m_hLibrary, "IsLicensedByCode");
#else
      m_fIsLicensedByCode = (FN_ISLICENSEDBYCODE)GetProcAddress(m_hLibrary, "ISLICENSEDBYCODE");
#endif

#ifdef WIN32
      m_fCanClose = (FN_CANCLOSE)GetProcAddress(m_hLibrary, "CanClose");
#else
      m_fCanClose = (FN_CANCLOSE)GetProcAddress(m_hLibrary, "CANCLOSE");
#endif


#ifdef WIN32
      m_fBuildDataFile = (FN_BUILDDATAFILE)GetProcAddress(m_hLibrary, "BuildDataFile");
#else
      m_fBuildDataFile = (FN_BUILDDATAFILE)GetProcAddress(m_hLibrary, "BUILDDATAFILE");
#endif

#ifdef WIN32
      m_fInitialize = (FN_INITIALIZE)GetProcAddress(m_hLibrary, "Initialize");
#else
      m_fInitialize = (FN_INITIALIZE)GetProcAddress(m_hLibrary, "INITIALIZE");
#endif

#ifdef WIN32
      m_fLastDllError = (FN_GETLASTDLLERROR)GetProcAddress(m_hLibrary, "GetLastDllError");
#else
      m_fLastDllError = (FN_GETLASTDLLERROR)GetProcAddress(m_hLibrary, "GETLASTDLLERROR");
#endif


      if (m_fLicense == NULL || m_fLastDllError == NULL || m_fIsLicensed == NULL || m_fBuildDataFile == NULL || m_fInitialize == NULL || m_fLastDllError == NULL || m_fIsLicensedByCode == NULL || m_fCanClose == NULL)
      {
         Shutdown();
      }
   }

}

LICENSERESULT CLicenseDll::License(HWND hParentWnd,CString cstrCodeToMark)
{
   CString inifile = m_csMsregIniFile;
   char buf[50];
   CWnd * pWnd = NULL;

   if (!IsValid() || m_fLicense == NULL)
   {
      return LICENSERESULT_Error;
   }

   if (!CheckForCD())
	{
		return LICENSERESULT_Error;
	}

   // Do the upgrade.

#pragma message("product level hard-coded here.")//don't think the upgrade mechanism even cares.
   int val = 1;


   //inifile += "MSREG.INI";
   GetPrivateProfileString("Configuration","ParentPartNumber","",buf,50,inifile);
   if(!strlen(buf)){
      AfxMessageBox("Could not find ParentPartNumber in MSREG.INI");
      return LICENSERESULT_Error;
   }

   return m_fLicense(hParentWnd, NULL, val, strtol(buf,NULL,10), cstrCodeToMark);
}

/*LICENSERESULT CLicenseDll::GetEncryptionInfo(char * FileName,structEncryptedFileInfo * info)
{  
   return m_fGetEncryptionInfo(FileName,info);//calls fn in upgrade.dll
} */


BOOL CLicenseDll::CanClose()
{
	if (!CheckForCD())
	{
		return FALSE;
	}
   return m_fCanClose();
}

LICENSERESULT CLicenseDll::IsLicensed(const char FAR * szCode)
{  
	if (!CheckForCD())
	{
		return LICENSERESULT_Error;
	}
   return m_fIsLicensedByCode((char FAR *)szCode);//calls fn in upgrade.dll
}


LICENSERESULT CLicenseDll::IsLicensed(const char FAR * FileName,structEncryptedFileInfo * info)
{  
	if (!CheckForCD())
	{
		return LICENSERESULT_Error;
	}
   return m_fIsLicensed((char FAR *)FileName,info);//calls fn in upgrade.dll
}

void CLicenseDll::BuildDataFile(BOOL bReallyBuild)
{  
   m_fBuildDataFile(bReallyBuild);//calls fn in upgrade.dll
}

// Who uses this?
char FAR * CLicenseDll::GetLastDllError()
{
	if (!CheckForCD())
	{
		ASSERT(FALSE);
		return "";
	}
   return m_fLastDllError();
}


CPrintMasterLicenseDll::CPrintMasterLicenseDll()
{
}

LICENSERESULT CPrintMasterLicenseDll::DoLicense(const CString* pcsProductCode, HWND hParentWnd)
{
   CString csVal;
   if(pcsProductCode)
      csVal = *pcsProductCode;
   LICENSERESULT result = License(hParentWnd,csVal);
   switch(result)
   {
      case LICENSERESULT_Success:      
         break;
 
      case LICENSERESULT_Missing_Critical_File:
        AfxMessageBox("Error: A required file for upgrading your product is missing.");
// What?  AfxMessageBox(GetLastError());
#ifdef _DEBUG
        AfxMessageBox("Might need to prompt for the CD.");
#endif
        return LICENSERESULT_Not_Licensed;

      case LICENSERESULT_Error:
//        AfxMessageBox("Unknown error upgrading your product.");
// What?  AfxMessageBox(GetLastError());
        return LICENSERESULT_Not_Licensed;
   }

   return result;
}

void CPrintMasterLicenseDll::RetrieveFile(const char FAR *)
{
#ifdef _DEBUG
   AfxMessageBox("Error: CPrintMasterLicenseDll::RetrieveFile should never get called.");//because PMW has its own file encryption/retrieve mechanism
#endif
}
