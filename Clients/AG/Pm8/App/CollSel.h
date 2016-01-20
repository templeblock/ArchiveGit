//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CollSel.h 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
// 
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELCOLLDLG_H__8D9DC0A1_EFB5_11D1_99A7_00A0C99E4AF4__INCLUDED_)
#define AFX_SELCOLLDLG_H__8D9DC0A1_EFB5_11D1_99A7_00A0C99E4AF4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelCollDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCollectionSelectionDialog dialog

class CCollectionSelectionDialog : public CDialog
{
// Construction
public:
	CCollectionSelectionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCollectionSelectionDialog)
	enum { IDD = IDD_COLLECTION_SELECTION };
	CListCtrl	m_clcCollections;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollectionSelectionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   void UpdateCollectionList(void);
protected:

	// Generated message map functions
	//{{AFX_MSG(CCollectionSelectionDialog)
	afx_msg void OnInstallCollection();
	virtual BOOL OnInitDialog();
	afx_msg void OnRemoveAllBuildCollections();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:

	ICollectionManager* m_pCollectionManager;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELCOLLDLG_H__8D9DC0A1_EFB5_11D1_99A7_00A0C99E4AF4__INCLUDED_)
