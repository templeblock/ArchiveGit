// Download.h : main header file for the DOWNLOAD application
//

#if !defined(AFX_DOWNLOAD_H__42B2C465_61F5_11D2_9862_00A0246D4780__INCLUDED_)
#define AFX_DOWNLOAD_H__42B2C465_61F5_11D2_9862_00A0246D4780__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDownloadApp:
// See Download.cpp for the implementation of this class
//

class CDownloadApp : public CWinApp
{
public:
	CDownloadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDownloadApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOAD_H__42B2C465_61F5_11D2_9862_00A0246D4780__INCLUDED_)
