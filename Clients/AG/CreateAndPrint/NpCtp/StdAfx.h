#pragma once

#define STRICT
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <atlwin.h>
#include <mshtmhst.h>

// ATL/WTL classes in the shared folder
#include <atlapp.h>
#include <atlctrls.h>	// CStatic, CButton, CListViewCtrl
#include <atlctrlx.h>	// CWaitCursor
#define _ATL_USE_CSTRING_FLOAT
#include <atlmisc.h>	// CSize, CPoint, CRect, CString

#ifdef _DEBUG
	#ifndef DEBUG_NEW
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#include <crtdbg.h>

		// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of that type
		#define DEBUG_NEW \
					new(_NORMAL_BLOCK, __FILE__, __LINE__)
		#define malloc(nSize) \
					_malloc_dbg(nSize, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define calloc(nCount, nSize) \
					_calloc_dbg(nCount, nSize, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define realloc(pBlock , nSize) \
					_realloc_dbg(pBlock, nSize, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define expand(pBlock , nSize ) \
					_expand_dbg(pBlock, nSize, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define free(pBlock) \
					_free_dbg(pBlock, _NORMAL_BLOCK)
		#define msize(pBlock) \
					_msize_dbg(pBlock, _NORMAL_BLOCK)
		// Use the following line to break at a specific block allocation: _crtBreakAlloc = 199;
	#endif DEBUG_NEW
#endif _DEBUG

#include "GdiClasses.h"
#include "Utility.h"
