// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//

/* Eliminate the warning about "unused local function removed"*/
#pragma warning(disable:4505)

#define VC_EXTRALEAN

#define OEMRESOURCE

#ifndef STRICT
#ifndef NOSTRICT
#define STRICT      // default is to use STRICT interfaces
#endif
#endif

#define EXPORT
#define _WINCON_

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions (including VB)
#include <afxole.h>			// MFC OLE support
#include <afxodlgs.h>		// MFC OLE dialog classes
#include <afxcmn.h>
#include <afxtempl.h>      // MFC template classes
#include "pmwinet.h"

#ifdef WIN32
#define INHERIT(me, parent) typedef parent PARENTCLASS;
#define INHERITED PARENTCLASS
#else
#define INHERIT(me, parent) typedef me THISCLASS; typedef parent PARENTCLASS;
#define INHERITED THISCLASS::PARENTCLASS
#endif

typedef short int SHORT;
typedef unsigned short int USHORT;

#include "system.h"

// This macro is the same as IMPLEMENT_OLECREATE, except it passes TRUE
//  for the bMultiInstance parameter to the COleObjectFactory constructor.
//  We want a separate instance of this application to be launched for
//  each OLE automation proxy object requested by automation controllers.
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2

#ifdef PRECOMPILE_FRAMEWORK
#include "FrameworkIncludes.h"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
