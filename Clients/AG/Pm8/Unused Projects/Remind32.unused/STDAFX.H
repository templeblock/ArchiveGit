// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef STRICT
#ifndef NOSTRICT
#define STRICT      // default is to use STRICT interfaces
#endif
#endif

#ifdef WIN32
#define EXPORT
#else
#define EXPORT __export
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>

#ifdef WIN32
#define INHERIT(me, parent) typedef parent PARENTCLASS;
#define INHERITED PARENTCLASS
#else
#define INHERIT(me, parent) typedef me THISCLASS; typedef parent PARENTCLASS;
#define INHERITED THISCLASS::PARENTCLASS
#endif

#include "system.h"
