// MonacoNew.h : main header file for the MONACONEW application
//

#if !defined(AFX_MONACONEW_H__882436CB_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_)
#define AFX_MONACONEW_H__882436CB_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMonacoNewApp:
// See MonacoNew.cpp for the implementation of this class
//

class CMonacoNewApp : public CWinApp
{
public:
	CMonacoNewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonacoNewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMonacoNewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONACONEW_H__882436CB_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_)
