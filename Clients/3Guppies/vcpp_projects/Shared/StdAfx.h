#pragma once

#define ISOLATION_AWARE_ENABLED 1 // For Visual Themes

//_WIN32_WINNT>=0x0502		Windows Server 2003
//_WIN32_WINNT>=0x0501		Windows XP
//_WIN32_WINNT>=0x0500		Windows 2000
//_WIN32_WINDOWS>=0x0500	Windows Me
//_WIN32_WINDOWS>=0x0410	Windows 98
//_WIN32_WINDOWS>=0x0400	Windows 95
//_WIN32_IE>=0x0600			Internet Explorer 6.0
//_WIN32_IE>=0x0501			Internet Explorer 5.01, 5.5

#define WINVER 0x0410
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0600

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS
#define _ATL_APARTMENT_THREADED

#ifdef _DEBUG
	#include "windows.h"
	#include "atldbgmem.h"
	// For some reason, the 'new' debug macro gets redefined during imports and some includes
	#pragma push_macro("new")
#endif _DEBUG

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>

#include <atlcom.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlfile.h>

// WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_WTYPES	// examined in atlmisc.h
#define _WTL_NO_CSTRING	// examined in atlmisc.h
#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime
#include <atluser.h> // for CMenu
#include <atlddx.h>		// For DDX support
#include <atlcrack.h>	// For enhanced msg map macros
#include <atlctrlw.h>	// For CCommandBarCtrl
#include <atldlgs.h>	// For common dialogs: CFileDialog, CColorDialog
#include <atlframe.h>	// For CFrameWindowImpl

// For some reason, the 'new' debug macro gets redefined during imports and some includes
#include "msxml.h"
//#import "msxml4.dll" named_guids
//using namespace MSXML2;

#ifdef _DEBUG
	#pragma pop_macro("new")
//	#define _MAKE_STR_HELPER(x) #x
//	#define _MAKE_STR(x) _MAKE_STR_HELPER(x)
//	#pragma message ("'new' is defined as: '" _MAKE_STR(new) "'")
#endif _DEBUG
