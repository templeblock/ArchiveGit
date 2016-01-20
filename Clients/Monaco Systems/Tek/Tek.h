// Tek.h : main header file for the TEK application
//

#if !defined(AFX_TEK_H__26149F86_19E4_11D3_91E8_00A02459C447__INCLUDED_)
#define AFX_TEK_H__26149F86_19E4_11D3_91E8_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTekApp:
// See Tek.cpp for the implementation of this class
//

class CTekApp : public CWinApp
{
public:
	CTekApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTekApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTekApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEK_H__26149F86_19E4_11D3_91E8_00A02459C447__INCLUDED_)
