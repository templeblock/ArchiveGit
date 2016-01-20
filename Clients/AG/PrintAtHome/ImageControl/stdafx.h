#pragma once

//_WIN32_WINNT>=0x0502		Windows Server 2003 family
//_WIN32_WINNT>=0x0501		Windows XP
//_WIN32_WINNT>=0x0500		Windows 2000
//_WIN32_WINDOWS>=0x0500	Windows Me
//_WIN32_WINDOWS>=0x0410	Windows 98
//_WIN32_IE>=0x0600			Internet Explorer 6.0
//_WIN32_IE>=0x0501			Internet Explorer 5.01, 5.5

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0600

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>

#include <atlcom.h>
#include <atltypes.h>
#include <atlctl.h>

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_CSTRING
//j#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime
#include <atluser.h> // for CMenu
