// SerialNumberApp.h : main header file for the SERIALNUMBERAPP application
//

#if !defined(AFX_SERIALNUMBERAPP_H__CA49A615_D314_11D2_8617_00A0241A89B6__INCLUDED_)
#define AFX_SERIALNUMBERAPP_H__CA49A615_D314_11D2_8617_00A0241A89B6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSerialNumberApp:
// See SerialNumberApp.cpp for the implementation of this class
//

class CSerialNumberApp : public CWinApp
{
public:
	CSerialNumberApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialNumberApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSerialNumberApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALNUMBERAPP_H__CA49A615_D314_11D2_8617_00A0241A89B6__INCLUDED_)
