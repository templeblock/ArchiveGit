// CTPUtil.h : main header file for the CTPUTIL application
//

#if !defined(AFX_CTPUTIL_H__64AE5205_3509_11D3_9331_0080C6F796A1__INCLUDED_)
#define AFX_CTPUTIL_H__64AE5205_3509_11D3_9331_0080C6F796A1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCTPUtilApp:
// See CTPUtil.cpp for the implementation of this class
//

class CCTPUtilApp : public CWinApp
{
public:
	CCTPUtilApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTPUtilApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCTPUtilApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTPUTIL_H__64AE5205_3509_11D3_9331_0080C6F796A1__INCLUDED_)
