// PluginTest.h : main header file for the PLUGINTEST application
//

#if !defined(AFX_PLUGINTEST_H__5DDFD749_7F8E_11D2_91E7_00A02459C447__INCLUDED_)
#define AFX_PLUGINTEST_H__5DDFD749_7F8E_11D2_91E7_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPluginTestApp:
// See PluginTest.cpp for the implementation of this class
//

class CPluginTestApp : public CWinApp
{
public:
	CPluginTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPluginTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINTEST_H__5DDFD749_7F8E_11D2_91E7_00A02459C447__INCLUDED_)
