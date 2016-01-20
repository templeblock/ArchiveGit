#if !defined(AFX_CONNMGR_H__94FB2C4C_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
#define AFX_CONNMGR_H__94FB2C4C_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ConnMgr.h : main header file for CONNMGR.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CConnMgrApp : See ConnMgr.cpp for implementation.

class CConnMgrApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
   void SetMainWnd(HWND hwndMainWnd);
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNMGR_H__94FB2C4C_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED)
