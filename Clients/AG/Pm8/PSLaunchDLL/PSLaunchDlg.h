// PSLaunchDlg.h : header file
//
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_PSLAUNCHDLG_H__AA4B7266_3932_11D2_BCFE_006008473DD0__INCLUDED_)
#define AFX_PSLAUNCHDLG_H__AA4B7266_3932_11D2_BCFE_006008473DD0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPSLaunchData;

#include <afxtempl.h>
#include "LaunchResource.h"

#include "PSBaseDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDlg dialog

class CPSLaunchDlg : public CPSBaseDialog
{
// Construction
public:
					CPSLaunchDlg(CWnd* pParent = NULL);	// standard constructor
	virtual			~CPSLaunchDlg();

// Dialog Data
	//{{AFX_DATA(CPSLaunchDlg)
	enum { IDD = IDD_PSLAUNCH_DIALOG };
	CStatic	m_cInfoText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSLaunchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	CPSLaunchData*				m_pLaunchData;
	CArray<CDialog*, CDialog*>	m_cDlgStack;
	int							m_nItem;

	UINT						m_uiMouseOverId;

	// Generated message map functions
	//{{AFX_MSG(CPSLaunchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LONG OnPushDialog( UINT, LONG );
	afx_msg LONG OnPopDialog( UINT, LONG );
	afx_msg LONG OnRemoveUnused( UINT, LONG );
	afx_msg LONG OnUmMouseMove( WPARAM, LPARAM );
	virtual void OnOK();
	afx_msg void OnOpen();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSLAUNCHDLG_H__AA4B7266_3932_11D2_BCFE_006008473DD0__INCLUDED_)
