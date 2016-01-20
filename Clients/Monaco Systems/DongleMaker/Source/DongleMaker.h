// DongleMaker.h : main header file for the DongleMaker application
//

#if !defined(AFX_DongleMaker_H__CD41E489_D443_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_DongleMaker_H__CD41E489_D443_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerApp:
// See DongleMaker.cpp for the implementation of this class
//

class CDongleMakerApp : public CWinApp
{
public:
	CDongleMakerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDongleMakerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDongleMakerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DongleMaker_H__CD41E489_D443_11D1_9EE4_006008947D80__INCLUDED_)
