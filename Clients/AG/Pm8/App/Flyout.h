#if !defined(AFX_RFLYOUT_H__B1F77442_BA00_11D1_A679_0020AF937692__INCLUDED_)
#define AFX_RFLYOUT_H__B1F77442_BA00_11D1_A679_0020AF937692__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RFlyout.h : header file
//

#include "FrameworkResourceIDs.h"
#include "FlyoutToolbar.h"
/////////////////////////////////////////////////////////////////////////////
// RFlyout dialog

class RFlyout : public CDialog
{
// Construction
public:
	RFlyout(UINT nNumRows, UINT nToolBarID, CWnd* pParent);   // standard constructor

	void SetButtonState(UINT nID, UINT nState );
// Dialog Data
	//{{AFX_DATA(RFlyout)
	enum { IDD = IDD_FLYOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RFlyout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RFlyout)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL m_bDestroyWindow;
	UINT m_nToolBarID;
	RFlyoutToolbar m_toolBar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RFLYOUT_H__B1F77442_BA00_11D1_A679_0020AF937692__INCLUDED_)
