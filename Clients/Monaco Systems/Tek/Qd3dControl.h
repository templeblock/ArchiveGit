#pragma once

// Qd3dControl.h : header file
//
#include "Qd3dGamutBase.h"

/////////////////////////////////////////////////////////////////////////////
// CQd3dControl window

class CQd3dControl : public CWnd
{
// Construction
public:
	CQd3dControl(RECT rect, CWnd* pWnd, CQd3dGamutBase* pQd3dGamut, int nID);

// Attributes
public:
	CQd3dGamutBase* m_pQd3dGamut;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQd3dControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQd3dControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQd3dControl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
