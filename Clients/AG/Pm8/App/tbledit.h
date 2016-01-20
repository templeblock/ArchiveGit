/*
// $Workfile: tbledit.h $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/tbledit.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 5     7/12/98 5:36p Jayn
// Various fixes.
// 
// 4     6/02/98 3:00p Jayn
// 
// 3     5/31/98 2:35p Jayn
// Delete row/columns
// 
// 2     5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 1     5/29/98 6:38p Jayn
*/

#ifndef __TBLEDIT_H__
#define __TBLEDIT_H__

#include "pmwdlg.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTableInsertDialog dialog

class CTableInsertDialog : public CPmwDialog
{
// Construction
public:
	CTableInsertDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	int m_nRows;
	int m_nColumns;
	int m_nMaxInsert;

	//{{AFX_DATA(CTableInsertDialog)
	enum { IDD = IDD_TABLE_INSERT };
	int		m_nWhere;
	int		m_nWhat;
	int		m_nInsertAmount;
	//}}AFX_DATA

	// "Where" values
	enum
	{
		INSERT_Before,
		INSERT_After,
	};

	// "What" values
	enum
	{
		INSERT_Rows,
		INSERT_Columns,
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableInsertDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableInsertDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRows();
	afx_msg void OnColumns();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void SetMaxInsert(void);
};

/////////////////////////////////////////////////////////////////////////////
// CTableDeleteDialog dialog

class CTableDeleteDialog : public CPmwDialog
{
// Construction
public:
	CTableDeleteDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTableDeleteDialog)
	enum { IDD = IDD_TABLE_DELETE };
	int		m_nWhat;
	//}}AFX_DATA

	// "What" values
	enum
	{
		DELETE_Rows,
		DELETE_Columns,
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableDeleteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableDeleteDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
