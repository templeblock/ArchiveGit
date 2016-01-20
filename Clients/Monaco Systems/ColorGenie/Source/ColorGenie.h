// ColorGenie.h : main header file for the COLORGENIE application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "CmmConvert.h"

/////////////////////////////////////////////////////////////////////////////
// CApp:
// See ColorGenie.cpp for the implementation of this class
//

class CApp : public CWinApp
{
protected:
	BOOL LoadCmm();

public:
	CApp();
	BOOL mhasDongle;

	BOOL mhasCmm;
	CCmmConvert	mcmmConvert;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CApp)
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenGamut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
