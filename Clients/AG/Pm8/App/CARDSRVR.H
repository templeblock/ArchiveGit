/*
// $Workfile: CARDSRVR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
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
// $Log: /PM8/App/CARDSRVR.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     2/08/99 8:48p Psasse
// New improved scheduling of Online Greetings
// 
//    Rev 1.0   14 Aug 1997 15:18:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:54   Fred
// Initial revision.
// 
//    Rev 1.4   10 Jul 1997 12:31:24   Jay
// Card server, etc.
// 
//    Rev 1.3   01 Jul 1997 15:21:08   Jay
// New 16-bit DLL
// 
//    Rev 1.2   23 May 1997 16:15:00   Jay
// Disclaimer dialog
// 
//    Rev 1.1   05 May 1997 08:53:42   Jay
// Added DLL. Better error handling.
// 
//    Rev 1.0   18 Apr 1997 17:09:46   Jay
// Initial revision.
*/

class CCardServerDialog;
class CCardServerDLL;
class CCardServerDisclaimer;
class CCardServerProgressDialog;

#ifndef __CARDSRVR_H__
#define __CARDSRVR_H__

#include "pmwdlg.h"
#include "progress.h"
#include "cardftp\cardftp.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCardServerDialog dialog

class CCardServerDialog : public CPmwDialog
{
// Construction
public:
	CCardServerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardServerDialog)
	enum { IDD = IDD_CARDSERVER };
	CMonthCalCtrl	m_ctrlMonthCal;
	CString	m_csFrom;
	CString	m_csTo;
	CString	m_csTitle;
	CTime	m_ctTime;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardServerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:

	SYSTEMTIME* GetTime(void)
	{return &m_stTime;}

// Implementation
protected:

	void UpdateOK(void);

	static CString m_csLastFrom;
	static CString m_csLastTo;
	static CString m_csLastTitle;
	
	SYSTEMTIME m_stTime;

	// Generated message map functions
	//{{AFX_MSG(CCardServerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFrom();
	afx_msg void OnChangeTo();
	afx_msg void OnSelchangeMonthcalendar1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeTitle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCardServerDLL

class CCardServerDLL
{
public:
	CCardServerDLL();
	~CCardServerDLL();

	BOOL Startup(LPCSTR pszDLLName);
	void Shutdown(void);

	int SendFiles(LPCSTR pTheDir, CCardServerProgressDialog& ProgressDialog);

protected:
	CARDFTP_SENDFILES m_pSendFiles;
	HINSTANCE m_hDLL;
};

/////////////////////////////////////////////////////////////////////////////
// CCardServerDisclaimer dialog

class CCardServerDisclaimer : public CPmwDialog
{
// Construction
public:
	CCardServerDisclaimer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardServerDisclaimer)
	enum { IDD = IDD_CARD_SERVER_LEGAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardServerDisclaimer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardServerDisclaimer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCardServerProgressDialog dialog

class CCardServerProgressDialog : public CDialog
{
// Construction
public:
	CCardServerProgressDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCardServerProgressDialog();              // standard destructor

	void SetStatus(UINT uStatus, double dPosition = 0.0);

// Dialog Data
	//{{AFX_DATA(CCardServerProgressDialog)
	enum { IDD = IDD_CARD_SERVER_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardServerProgressDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardServerProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CMlsProgressBar m_ProgressBar;
};

#endif

