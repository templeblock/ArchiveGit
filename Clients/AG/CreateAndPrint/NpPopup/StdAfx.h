#pragma once
#define STRICT

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0400
#endif

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <atlwin.h>
#include <vector>

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#define _ATL_USE_CSTRING_FLOAT
#include <atlmisc.h>	// CSize, CPoint, CRect, CString
