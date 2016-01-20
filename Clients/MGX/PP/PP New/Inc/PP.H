// pp.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//
#ifndef WIN32
#include "winerr16.h"
#endif
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxole.h>         // MFC OLE classes
#include <afxcmn.h>         // MFC common controls
//#include <afxdlgs.h> 	// !!!!DELETE WHEN UNCOMMENTING AFXOLE.h Line
#include <shellapi.h>
#include <math.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

// the below files are commonly used header files that infrequently change
// define FIX_HEADERS if you want to try to fix the way we include
// everything in the world so maybe we could have real dependencies
#ifdef FIX_HEADERS
#include "mgxframe.h"
#include "types.h"
#include "imgutil.h"
#include "app.h"
#else
#include "routines.h"
#endif
