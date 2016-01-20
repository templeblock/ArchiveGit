#pragma once

#define STRICT

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
#define _WIN32_IE 0x0400

#define _ATL_APARTMENT_THREADED


// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS

#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlapp.h>
#include "JMSrvrDef.h"

#include <atlsafe.h>	// CComSafeArray
#include <atldlgs.h>	// CPropertySheet, CPropertyPage
#include <atlddx.h>		// CWinDataExchange<T>
#include <atlcomtime.h>	// COleDateTime

#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor


#include "commctrl.h"	// Common controls

#include <Windows.h>
#include "Utility.h"

#include <HtmlHelp.h>	// Html help

#import "..\AgOutlookAddin\AgOutlookAddIn.tlb" named_guids
using namespace AgOutlookAddInLib;

extern CJMSrvrModule _Module;
extern CString g_szAppName;
