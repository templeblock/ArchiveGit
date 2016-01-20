/*
// $Workfile: PAGEDLGS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PAGEDLGS.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:56   Fred
// Initial revision.
// 
//    Rev 1.2   26 Jul 1996 11:39:22   Jay
// Page stuff
// 
//    Rev 1.1   25 Jul 1996 19:38:44   Jay
//  
// 
//    Rev 1.4   24 Jul 1996 18:25:16   Jay
// PaintCustomize() call
// 
//    Rev 1.3   23 Jul 1996 16:51:56   Fred
// Redraws on WM_ENABLE
// 
//    Rev 1.2   23 Jul 1996 09:05:20   Fred
// Double clicks turned into single clicks
// 
//    Rev 1.1   23 Jul 1996 08:04:06   Fred
// Added size to bitmap flag
// 
//    Rev 1.0   11 Jul 1996 17:46:16   Jay
//  
*/

class CAddPageDialog;
class CDeletePageDialog;
class CGotoPageDialog;

#ifndef __PAGEDLGS_H__
#define __PAGEDLGS_H__

#include "pmwdlg.h"

class CPmwDoc;

/////////////////////////////////////////////////////////////////////////////
// CPageBaseDialog dialog

class CPageBaseDialog : public CPmwDialog
{
public:
	CPageBaseDialog(CPmwDoc* pDocument, UINT uID, CWnd* pParent /*=NULL*/);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPageBaseDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPmwDoc* m_pDocument;

	void InvalidPageNumber(void);
	void LimitEditText(UINT uID, int nLimit);
};

/////////////////////////////////////////////////////////////////////////////
// CAddPageDialog dialog

class CAddPageDialog : public CPageBaseDialog
{
// Construction
public:
	enum
	{
		BeforeCurrentPage,
		AfterCurrentPage,
	};

	enum
	{
		AddBlankPages,
		AddPageCopies
	};
	
	CAddPageDialog(CPmwDoc* pDocument, CWnd* pParent = NULL, int nWhere = AfterCurrentPage, int nPages = 1, int nContents = AddBlankPages);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddPageDialog)
	enum { IDD = IDD_ADD_PAGE };
	int		m_nWhere;
	CString	m_csPages;
	int		m_nContents;
	CString	m_csCopyPage;
	//}}AFX_DATA

	CString m_csCopyPageSave;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddPageDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddBlank();
	afx_msg void OnAddCopy();
	afx_msg void OnChangeAddCopyPage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDeletePageDialog dialog

class CDeletePageDialog : public CPageBaseDialog
{
// Construction
public:
	CDeletePageDialog(CPmwDoc* pDocument, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeletePageDialog)
	enum { IDD = IDD_DELETE_PAGE };
	CString	m_csFrom;
	CString	m_csTo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeletePageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeletePageDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CGotoPageDialog dialog

class CGotoPageDialog : public CPageBaseDialog
{
// Construction
public:
	CGotoPageDialog(CPmwDoc* pDocument, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoPageDialog)
	enum { IDD = IDD_GOTO_PAGE };
	CString	m_csPage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoPageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoPageDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
