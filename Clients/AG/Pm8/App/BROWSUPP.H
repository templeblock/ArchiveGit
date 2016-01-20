/*
// $Workfile: BROWSUPP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BROWSUPP.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     4/30/98 10:23p Fredf
// Support for "Special Collection" button used for American Greetings.
// 
// 3     3/12/98 4:39p Hforman
// added CEditUserTitle
// 
// 2     1/22/98 7:15p Hforman
// add CMsgBoxYesAll dialog
// 
//    Rev 1.0   14 Aug 1997 15:18:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:48   Fred
// Initial revision.
// 
//    Rev 1.1   06 Jun 1997 11:51:26   Jay
// Licensing changes
// 
//    Rev 1.0   30 May 1997 19:16:10   Jay
// Initial revision.
*/

#ifndef __BROWSUPP_H__
#define __BROWSUPP_H__

#include "pmwdlg.h"
#include "progress.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowserPropertiesDialog dialog

#define MAX_FRIENDLY_NAME_LEN			55		// maximum characters in title of item

class CBrowserPropertiesDialog : public CPmwDialog
{
// Construction
public:
	CBrowserPropertiesDialog(UINT uID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBrowserPropertiesDialog)
	enum { IDD = IDD_BROWSER_USER_PROPERTIES };
	CEdit	m_ctrlFriendlyName;
	CString	m_csKeywords;
	CString	m_csFriendlyName;
	CString	m_csFileName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowserPropertiesDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBrowserProgressDialog dialog

class CBrowserProgressDialog : public CDialog
{
	INHERIT(CBrowserProgressDialog, CDialog)

// Construction
public:
	CBrowserProgressDialog(UINT uID, int nItems, CWnd* pParent = NULL, UINT uAbortMessage = 0);
	virtual ~CBrowserProgressDialog();

// Dialog Data
	//{{AFX_DATA(CBrowserProgressDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserProgressDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowserProgressDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetPosition(int nPosition, LPCSTR pszFileName);
	BOOL CheckForAbort(void);

protected:
	CMlsProgressBar m_ProgressBar;
	BOOL m_fAbort;
	CString m_csFilenameFormat;
   int m_nCount;
	UINT m_uAbortMessage;
};

/////////////////////////////////////////////////////////////////////////////
// CUnlicensedDialog dialog

class CUnlicensedDialog : public CPmwDialog
{
// Construction
public:
	CUnlicensedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnlicensedDialog)
	enum { IDD = IDD_UNLICENSED_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnlicensedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUnlicensedDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnOrderForm();
	afx_msg void OnViewSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMsgBoxYesAll dialog

// value sent back when "Yes All" button pressed
#define IDYESALL	20

class CMsgBoxYesAll : public CPmwDialog
{
// Construction
public:
	CMsgBoxYesAll(LPCTSTR str, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMsgBoxYesAll)
	enum { IDD = IDD_MSGBOX_YESALL };
	CStatic	m_icon;
	CString	m_csMessage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBoxYesAll)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgBoxYesAll)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnYesall();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditUserTitle window

class CEditUserTitle : public CEdit
{
// Construction
public:
	CEditUserTitle();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditUserTitle)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditUserTitle();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditUserTitle)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif //__BROWSUPP_H__

