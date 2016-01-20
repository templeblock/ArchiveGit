#pragma once

#define ISOLATION_AWARE_ENABLED 1 // For Visual Themes

// Use these values to set _WIN32_WINDOWS and WINVER to your minimum support level
#define Windows95    0x0400
#define Windows98    0x0410
#define WindowsME    0x0500

// Use these values to set _WIN32_WINNT and WINVER to your mimimum support level
#define WindowsNT4   0x0400
#define Windows2000  0x0500
#define WindowsXP    0x0501
#define Windows2003  0x0502
#define WindowsVista 0x0600

// Use these values to set _WIN32_IE to your minimum support level
#define IE3     0x0300
#define IE301   0x0300
#define IE302   0x0300
#define IE4     0x0400
#define IE401   0x0401
#define IE5     0x0500
#define IE5a    0x0500
#define IE5b    0x0500
#define IE501   0x0501
#define IE55    0x0501
#define IE56    0x0560
#define IE6     0x0600
#define IE601   0x0601
#define IE602   0x0603
#define IE7     0x0700

#define WINVER Windows2000
#define _WIN32_WINNT WindowsXP
#define _WIN32_WINDOWS WindowsME
#define _WIN32_IE IE6

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
#include <atluser.h>	// for CMenu
#include <atlddx.h>		// For DDX support
#include <atlcrack.h>	// For enhanced msg map macros
#include <atlctrlw.h>	// For CCommandBarCtrl
#include <atldlgs.h>	// For common dialogs: CFileDialog, CColorDialog
#include <atlframe.h>	// For CFrameWindowImpl

#ifdef _DEBUG
	#pragma pop_macro("new")
//	#define _MAKE_STR_HELPER(x) #x
//	#define _MAKE_STR(x) _MAKE_STR_HELPER(x)
//	#pragma message ("'new' is defined as: '" _MAKE_STR(new) "'")
#endif _DEBUG
