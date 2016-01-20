#pragma once

#define STRICT
#define ISOLATION_AWARE_ENABLED 1 // For Visual Themes

#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define _WIN32_WINDOWS 0x0410
#define _WIN32_IE 0x0501

#define _ATL_APARTMENT_THREADED

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#ifdef _DEBUG
	#define DEBUG_NEW new
#endif _DEBUG

#include "resource.h"
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlctl.h>

//extern CComModule _Module;
#define _Module (*_pModule)

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#define _WTL_NO_CSTRING
#include <atlmisc.h>	// CSize, CPoint, CRect, CString
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#include <atlsafe.h>	// CComSafeArray
#include <atlcomtime.h>	// COleDateTime

#undef _Module

#include <gdiplus.h>
using namespace Gdiplus;
#include "initgdiplus.h"
