////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CnfrmRm.h 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CnfrmRm.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     5/13/98 6:56p Hforman
// add CConfirmCategoryRemove
// 
// 2     5/06/98 1:39p Hforman
// 
// 1     5/06/98 1:39p Hforman
// Changed file name, merged two dialogs, etc.
// 
////////////////////////////////////////////////////////////////////////////

#ifndef _CNFRMRM_H_
#define _CNFRMRM_H_

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CConfirmFileRemove dialog

class CConfirmFileRemove : public CPmwDialog
{
// Construction
public:
	CConfirmFileRemove(BOOL fProjects, const CString& csFriendlyName, const CString& csFileName, BOOL fDeleteChecked, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CConfirmFileRemove)
	enum { IDD = IDD_CONFIRM_FILE_DELETE };
	CStatic	m_staticConfirmText;
	CButton	m_chkDeleteFile;
	CString	m_strFileName;
	BOOL	m_fDeleteFile;
	//}}AFX_DATA

	BOOL DeleteChecked(void)	{ return m_fDeleteChecked; }

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CConfirmFileRemove)
	virtual BOOL OnInitDialog();
	afx_msg void OnNo();
	afx_msg void OnYes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL		m_fProjects;			// deleting projects or art?
	CString	m_csFriendlyName;
	CString	m_csFileName;
	BOOL		m_fDeleteChecked;
};

#endif // _CNFRMRM_H_
/////////////////////////////////////////////////////////////////////////////
// CConfirmCategoryRemove dialog

class CConfirmCategoryRemove : public CDialog
{
// Construction
public:
	CConfirmCategoryRemove(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmCategoryRemove)
	enum { IDD = IDD_CONFIRM_DELETE_CATEGORY };
	BOOL	m_fDeleteChecked;
	//}}AFX_DATA

	BOOL DeleteChecked()		{ return m_fDeleteChecked; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmCategoryRemove)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmCategoryRemove)
	afx_msg void OnYes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
