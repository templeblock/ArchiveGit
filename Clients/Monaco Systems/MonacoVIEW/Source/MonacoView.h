// MonacoView.h : main header file for the MONACOVIEW application
//

#if !defined(AFX_MONACOVIEW_H__9CD0E3C5_E1DA_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_MONACOVIEW_H__9CD0E3C5_E1DA_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMonacoViewApp:
// See MonacoView.cpp for the implementation of this class
//

class CMonacoViewApp : public CWinApp
{
public:
	CMonacoViewApp();

	BOOL mhasDongle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonacoViewApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMonacoViewApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONACOVIEW_H__9CD0E3C5_E1DA_11D0_9B8C_444553540000__INCLUDED_)
