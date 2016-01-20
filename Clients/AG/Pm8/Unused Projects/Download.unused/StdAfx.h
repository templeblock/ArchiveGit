// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__42B2C469_61F5_11D2_9862_00A0246D4780__INCLUDED_)
#define AFX_STDAFX_H__42B2C469_61F5_11D2_9862_00A0246D4780__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef STRICT
#ifndef NOSTRICT
#define STRICT      // default is to use STRICT interfaces
#endif
#endif

#define EXPORT

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>			// MFC socket extensions
#include <afxtempl.h>      // MFC template classes
#include <afxinet.h>

#define INHERIT(me, parent) typedef parent PARENTCLASS;
#define INHERITED PARENTCLASS

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__42B2C469_61F5_11D2_9862_00A0246D4780__INCLUDED_)
