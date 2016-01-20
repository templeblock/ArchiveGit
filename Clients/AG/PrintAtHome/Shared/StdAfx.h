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
#define _WIN32_IE 0x0600

#define _ATL_APARTMENT_THREADED

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>

//j extern CComModule _Module;
//j#define _Module (*_pModule)

#include <atlcom.h>
#include <atltypes.h>
#include <atlctl.h>

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
//j#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime
#include <atluser.h> // for CMenu

#include <map>
#include <string>

#import "progid:MSXML2" named_guids


//j#undef _Module
