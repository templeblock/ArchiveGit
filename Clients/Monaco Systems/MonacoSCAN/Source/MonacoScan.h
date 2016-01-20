// MonacoScan.h : main header file for the MONACOSCAN application
//

#if !defined(AFX_MONACOSCAN_H__F09703CE_364F_11D1_BAE5_200604C10000__INCLUDED_)
#define AFX_MONACOSCAN_H__F09703CE_364F_11D1_BAE5_200604C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CApp:
// See MonacoScan.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp();

	BOOL mhasDongle;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONACOSCAN_H__F09703CE_364F_11D1_BAE5_200604C10000__INCLUDED_)
