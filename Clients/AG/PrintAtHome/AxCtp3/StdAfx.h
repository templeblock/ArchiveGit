#pragma once

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define STRICT
#define ISOLATION_AWARE_ENABLED 1 // For Visual Themes

//_WIN32_WINNT>=0x0502		Windows Server 2003 family
//_WIN32_WINNT>=0x0501		Windows XP
//_WIN32_WINNT>=0x0500		Windows 2000
//_WIN32_WINNT>=0x0400		Windows NT 4.0
//_WIN32_WINDOWS=0x0490		Windows Me
//_WIN32_WINDOWS>=0x0410	Windows 98
//_WIN32_IE>=0x0600			Internet Explorer 6.0
//_WIN32_IE>=0x0501			Internet Explorer 5.01, 5.5
//_WIN32_IE>=0x0500			Internet Explorer 5.0, 5.0a, 5.0b
//_WIN32_IE>=0x0401			Internet Explorer 4.01
//_WIN32_IE>=0x0400			Internet Explorer 4.0

#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410
#define _WIN32_IE 0x0501

#define _ATL_APARTMENT_THREADED

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#ifdef _DEBUG
//j	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define DEBUG_NEW new
//j	#include "atldbgmem.h"
#endif _DEBUG

/* Secure Template Overloads for VS 2005 CRT security update */
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1 
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT  1

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>

//j extern CComModule _Module;
#define _Module (*_pModule)

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#include <atlmisc.h>	// CSize, CPoint, CRect, CString, CRecentDocumentList
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime

#include "GdiClasses.h"
#include "Utility.h"

extern CString g_szAppName;

#undef _Module
