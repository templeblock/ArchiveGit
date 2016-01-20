#if !defined(AFX_CONNCFG_H__FAD387A3_9401_11D1_9296_444553540000__INCLUDED_)
#define AFX_CONNCFG_H__FAD387A3_9401_11D1_9296_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/***************************************************************************
*  FILE:        PREFONL.CPP                                                *
*  SUMMARY:     Property Page for Online configuration.                    *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  1/27/98  DGP      Created                                               *

   See prefonl.cpp for revision history

***************************************************************************/

class CConnectionSettings;
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesOnlinePage dialog

class CPreferencesOnlinePage : public CDialog
{
	INHERIT(CPreferencesOnlinePage, CDialog)
// Construction
public:
	CPreferencesOnlinePage();   // standard constructor
	~CPreferencesOnlinePage();  // standard destructor

// Dialog Data
	//{{AFX_DATA(CPreferencesOnlinePage)
	enum { IDD = IDD_PREFS_CONNECT };
	CButton	m_btnDirect;
	CButton	m_btnModem;
	CComboBox	m_comboDialups;
	CButton	m_btnAdvanced;
	int		m_nCurDialup;
	//}}AFX_DATA

// Operations
   void        DeInit();
   void        InitSettings(CConnectionSettings * pConnSettings);
// Overrides
//   virtual int DoModal();
#ifdef IS_PROPERTY_PAGE
   virtual BOOL OnSetActive();
   virtual BOOL OnKillActive();
#else
   virtual int  DoModal(CConnectionSettings * pConnSettings);
#endif

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesOnlinePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesOnlinePage)
	afx_msg void OnConnectionModemProperties();
	afx_msg void OnConnectionModem();
	afx_msg void OnConnectionDirect();
	virtual void OnOK();
	afx_msg void OnSelchangeDialupConnections();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
   BOOL                 m_bOwnSettings;
   BOOL                 m_bVisitedThisPage;
   BOOL                 m_bSetActiveOnce;
   CConnectionSettings  *m_pConnSettings;
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionSettingsAdvanced dialog

class CConnectionSettingsAdvanced : public CDialog
{
// Construction
public:
	CConnectionSettingsAdvanced(CWnd* pParent = NULL);   // standard constructor

   void SetAppTitle(LPCSTR szAppTitle);
   LPCSTR GetAppTitle() const;
   LPCSTR GetAppFileName() const;
   DWORD        GetTimeOut() const
      { return m_dwTimeOut; }
   DWORD        GetFirstCheck() const
      { return m_dwFirstCheck; }
   void SetAppFileName(LPCSTR szFileName);
   void        SetTimeOut(DWORD dwTimeOut) 
     { m_dwTimeOut = dwTimeOut; }
   void        SetFirstCheck(DWORD dwFirstCheck)
     { m_dwFirstCheck = dwFirstCheck; }

// Dialog Data
	//{{AFX_DATA(CConnectionSettingsAdvanced)
	enum { IDD = IDD_CONNECTION_ADVANCED_SETTINGS };
	CButton	m_btnRestoreAutodial;
	CButton	m_btnAppPath;
	DWORD	m_dwFirstCheck;
	DWORD	m_dwTimeOut;
	CString	m_csAppTitle;
	CString	m_csAppPathLabel;
	BOOL	m_bUsesDialup;
	BOOL	m_bRestoreAutodial;
	//}}AFX_DATA


// Overrides
   virtual int DoModal(CConnectionSettings *pConnSettings);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionSettingsAdvanced)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   virtual int DoModal() { return IDCANCEL; } // Empty DoModal() not allowed with this class

	// Generated message map functions
	//{{AFX_MSG(CConnectionSettingsAdvanced)
	afx_msg void OnConnectionProgram();
	virtual BOOL OnInitDialog();
	afx_msg void OnConnectionUsesDialup();
   afx_msg void OnUpdateAutodial(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Data Members
   CString              m_csFileName;
   CConnectionSettings  *m_pConnSettings;
};

/////////////////////////////////////////////////////////////////////////////
// COnlineProgramConfirmStart dialog

class COnlineProgramConfirmStart : public CDialog
{
// Construction
public:
	COnlineProgramConfirmStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnlineProgramConfirmStart)
	enum { IDD = IDD_ONLINE_CONFIRM_START };
	BOOL	m_bDontShowMe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineProgramConfirmStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineProgramConfirmStart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNCFG_H__FAD387A3_9401_11D1_9296_444553540000__INCLUDED_)

