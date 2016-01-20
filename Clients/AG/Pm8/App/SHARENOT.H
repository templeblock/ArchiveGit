/*
// $Workfile: SHARENOT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHARENOT.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:18   FRED
// Initial revision.
// 
//    Rev 1.0   15 Nov 1995 14:41:42   FRED
// Initial revision.
*/ 

class CShareNotLoadedDialog;
class CAutoexecBatModified;
class CCannotModifyAutoexecBatDialog;

#ifndef __SHARENOT_H__
#define __SHARENOT_H__

/////////////////////////////////////////////////////////////////////////////
// CShareNotLoadedDialog dialog

class CShareNotLoadedDialog : public CDialog
{
// Construction
public:
	CShareNotLoadedDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CShareNotLoadedDialog)
	enum { IDD = IDD_SHARE_NOT_LOADED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
public:
	BOOL m_fAutoexecBatModified;
	CString m_csBackupName;
	CString m_csReason;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CShareNotLoadedDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAutoexecBatModified dialog

class CAutoexecBatModifiedDialog : public CDialog
{
// Construction
public:
	CAutoexecBatModifiedDialog(CString& csBackupName, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoexecBatModifiedDialog)
	enum { IDD = IDD_AUTOEXEC_BAT_MODIFIED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAutoexecBatModifiedDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	CString m_csBackupName;
};

/////////////////////////////////////////////////////////////////////////////
// CCannotModifyAutoexecBat dialog

class CCannotModifyAutoexecBatDialog : public CDialog
{
// Construction
public:
	CCannotModifyAutoexecBatDialog(CString& csReason, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCannotModifyAutoexecBatDialog)
	enum { IDD = IDD_CANNOT_MODIFY_AUTOEXEC_BAT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CCannotModifyAutoexecBatDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	CString m_csReason;
};

#endif
