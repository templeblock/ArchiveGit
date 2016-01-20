/*
// $Workfile: DBCONVRT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DBCONVRT.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:28   Fred
// Initial revision.
// 
//    Rev 1.1   18 Jul 1996 14:52:56   FRED
//  
// 
//    Rev 1.0   17 Jul 1996 11:21:48   Fred
// Initial revision.
*/

class CDbconvertDlg;
class CDbconvertApp;

#ifndef __DBCONVRT_H__
#define __DBCONVRT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "dbcvtres.h"		// main symbols
#include "progress.h"
#include "cctl3d.h"

/////////////////////////////////////////////////////////////////////////////
// CDbconvertDlg dialog

class CDbconvertDlg : public CDialog
{
// Construction
public:
	CDbconvertDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDbconvertDlg();

// Dialog Data
	//{{AFX_DATA(CDbconvertDlg)
	enum { IDD = IDD_CONVERT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbconvertDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDbconvertDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	void SetName(LPCSTR pszName);
	void SetRange(DWORD dwMin, DWORD dwMax)
		{ m_ProgressBar.SetRange((double)dwMin, (double)dwMax); }
	void SetPosition(DWORD dwPos)
		{ m_ProgressBar.SetPosition((double)dwPos); }
		
	BOOL CheckForAbort();
	
protected:
	CMlsProgressBar m_ProgressBar;
	BOOL m_fAbort;
};

/////////////////////////////////////////////////////////////////////////////
// CDbconvertApp:

class CDbconvertApp : public CWinApp
{
public:
	CDbconvertApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbconvertApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDbconvertApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	void Init3DControls(void);
	CControl3D m_ctl3d;
	CDbconvertDlg* m_pDialog;
	BOOL CheckForAbort(void)
		{ return m_pDialog->CheckForAbort(); }
	void SetName(LPCSTR pszName)
		{ m_pDialog->SetName(pszName); }
	void SetRange(DWORD dwMin, DWORD dwMax)
		{ m_pDialog->SetRange(dwMin, dwMax); }
	void SetPosition(DWORD dwPos)
		{ m_pDialog->SetPosition(dwPos); }
	BOOL OpenBDE(void);
	void CloseBDE(void);
	
protected:
	ERRORCODE OpenAddressBookDatabase(
		LPCSTR pszFullName,
		StorageFile*& pDatabaseFile,
		CFlatFileDatabase*& pDatabase,
		BOOL fCreate = FALSE,
		LPCSTR pszDescription = NULL,
		CFlatFileFieldDescription* pFieldDescriptions = NULL,
		CFlatFileIndexDescription* pIndexDescriptions = NULL);
	ERRORCODE OpenAddressBookSenderDatabase(
			LPCSTR pszFullName,
			StorageFile*& pDatabaseFile,
			CFlatFileDatabase*& pDatabase,
			BOOL fCreate = FALSE);
	void CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase);
	LPSTR LocateParameter(LPSTR lpszParamName, LPSTR lpszValue);
	BOOL FindParameter(LPSTR lpszParamName);
	LPSTR GetParameter(LPSTR lpszParamName, LPSTR lpszValue);
	void ConvertOldAddressBooks(void);
	void ConvertAddressBook(LPCSTR pszOldName, LPCSTR pszNewName);
	void ConvertSenderBook(LPCSTR pszOldName, LPCSTR pszNewName);
	void ConvertField(CBdeCursor& Cursor, LPCSTR pszOldFieldName, CFlatFileDatabaseCursor* pDatabaseCursor, LPCSTR pszNewFieldName);
	BOOL GetDirectoryNames(void);
	void ShowDialog(void);
	
	CString m_csOldDirectory;
	CString m_csNewDirectory;
};

#endif
