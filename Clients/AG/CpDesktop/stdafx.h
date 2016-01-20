// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxdhtml.h>        // HTML Dialogs

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
inline bool IsAg()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("ag.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline bool IsBm()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("bm.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline bool IsBr()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("br.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline bool IsMs()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("ms.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline bool IsUk()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("uk.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline bool IsTr()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
	CString strAppPath = szAppPath;
	strAppPath.MakeLower();
	return (strAppPath.Find("tr.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
inline UINT GetFolderId()
{
	if (IsAg())
		return IDS_APPAG_FOLDER;
	if (IsBm())
		return IDS_APPBM_FOLDER;
	if (IsBr())
		return IDS_APPBR_FOLDER;
	if (IsMs())
		return IDS_APPMS_FOLDER;
	if (IsUk())
		return IDS_APPUK_FOLDER;
	if (IsTr())
		return IDS_APPTR_FOLDER;

	return IDS_APPAG_FOLDER;
}

/////////////////////////////////////////////////////////////////////////////
inline UINT GetTitleId()
{
	if (IsBm() || IsUk())
		return IDS_TITLEBM;

	return IDS_TITLEAG;
}

/////////////////////////////////////////////////////////////////////////////
inline UINT GetIconId()
{
	if (IsBm() || IsUk())
		return IDI_LOGOBM;

	return IDI_LOGOAG;
}

/////////////////////////////////////////////////////////////////////////////
inline UINT GetSplashId()
{
	if (IsBm() || IsUk())
		return IDB_SPLASHBM;

	if (IsBr())
		return IDB_SPLASHBR;

	return IDB_SPLASHAG;
}
