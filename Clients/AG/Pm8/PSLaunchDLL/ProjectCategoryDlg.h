#if !defined(AFX_PROJECTCATEGORYDLG_H__DBB037A3_3B9F_11D2_BCFF_006008473DD0__INCLUDED_)
#define AFX_PROJECTCATEGORYDLG_H__DBB037A3_3B9F_11D2_BCFF_006008473DD0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PSBaseDialog.h"

// ProjectCategoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectCategoryDlg dialog

class CProjectCategoryDlg : public CPSBaseDialog
{
// Construction
public:
	CProjectCategoryDlg( UINT nDialogID, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectCategoryDlg)
	enum { IDD = IDD_CATEGORY_ALL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectCategoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	UINT	MapProjectToDialog( UINT nProjType );

	// Generated message map functions
	//{{AFX_MSG(CProjectCategoryDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNext();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	UINT	m_uiNextDlg;
	UINT	m_nProjectType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTCATEGORYDLG_H__DBB037A3_3B9F_11D2_BCFF_006008473DD0__INCLUDED_)
