#if !defined(AFX_STDAFX_H__94FB2C4A_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
#define AFX_STDAFX_H__94FB2C4A_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _WINCON_     // Disable including "wincon.h" from standard include directory

#include <afxctl.h>         // MFC support for ActiveX Controls

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#include <afxdb.h>			// MFC database classes
#include <afxdao.h>			// MFC DAO database classes

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#ifdef WIN32
#define INHERIT(me, parent) typedef parent PARENTCLASS;
#define INHERITED PARENTCLASS
#else
#define INHERIT(me, parent) typedef me THISCLASS; typedef parent PARENTCLASS;
#define INHERITED THISCLASS::PARENTCLASS
#endif

#endif // !defined(AFX_STDAFX_H__94FB2C4A_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
