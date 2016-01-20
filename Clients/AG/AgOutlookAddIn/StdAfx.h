#pragma once

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

#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>

//You may derive from CComModule, but do not change the name of _Module
extern CComModule _Module;
//j extern CComModule& _Module;
//j #define _Module (*_pModule)

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_CSTRING
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS

#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime
#include <atldlgs.h>	// CPropertySheet, CPropertyPage
#include <atlddx.h>		// CWinDataExchange<T>

//#import "C:\Program Files\Common Files\Designer\MSADDNDR.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "progid:MSAddnDr.AddInDesigner" named_guids, raw_interfaces_only, no_namespace

//#import "C:\Program Files\Microsoft Office\Office\mso9.dll" named_guids
//#import "progid:OfficeCompatible.Application" named_guids
#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" named_guids rename("RGB", "_RGB") rename("DocumentProperties", "_DocumentProperties")
using namespace Office;

//#import "C:\Program Files\Microsoft Office\Office\MSOUTL9.olb" named_guids
//#import "progid:Outlook.Application" named_guids
#import "libid:00062FFF-0000-0000-C000-000000000046" named_guids rename("CopyFile", "_CopyFile") 
using namespace Outlook;

#import "progid:JMSrvr.JMBrkr"
using namespace JMSrvrLib;

#include "Utility.h"

extern CString g_szAppName;

#define OCM_COMMAND_CODE_HANDLER(code, func) \
	if(uMsg == OCM_COMMAND && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_COMMAND_ID_HANDLER(id, func) \
	if(uMsg == OCM_COMMAND && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_CODE_HANDLER(cd, func) \
	if(uMsg == OCM_NOTIFY && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_ID_HANDLER(id, func) \
	if(uMsg == OCM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define OCM_NOTIFY_HANDLER(id, cd, func) \
	if(uMsg == OCM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}
