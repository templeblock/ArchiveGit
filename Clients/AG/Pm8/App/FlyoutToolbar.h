#if !defined(AFX_FLYOUTTOOLBAR_H__A54CE077_BA00_11D1_A679_0020AF937692__INCLUDED_)
#define AFX_FLYOUTTOOLBAR_H__A54CE077_BA00_11D1_A679_0020AF937692__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FlyoutToolbar.h : header file
//
////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE: This toolbar is designed to work as a flyout.  For it to 
// function properly you must set the capture to the toolbar window after you 
// you create it.  You must also handle the command message UM_DESTROYTOOLBAR.
// This handler should destroy flyout's window.


/////////////////////////////////////////////////////////////////////////////
// RFlyoutToolbar window

class RFlyoutToolbar : public CToolBar
{
// Construction
public:
	RFlyoutToolbar(int nRows);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CSize GetSize();
	virtual ~RFlyoutToolbar();

	// Generated message map functions
protected:
	CRect m_rcSelRect;
	int m_nRows;

	//{{AFX_MSG(RFlyoutToolbar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYOUTTOOLBAR_H__A54CE077_BA00_11D1_A679_0020AF937692__INCLUDED_)
