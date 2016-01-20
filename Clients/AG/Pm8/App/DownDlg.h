//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/DownDlg.h 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
// 
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNDLG_H__553F9671_6838_11D2_9863_00A0246D4780__INCLUDED_)
#define AFX_DOWNDLG_H__553F9671_6838_11D2_9863_00A0246D4780__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DownDlg.h : header file
//

#include "downmgr.h"
#include "progstat.h"

/////////////////////////////////////////////////////////////////////////////
// CCollectionListBox

class CCollectionListBox : public CCheckListBox
{
public:
	CCollectionListBox();
	virtual ~CCollectionListBox();
	virtual void DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);

	void SetDownloadedAlign(int nAlign)
		{ m_nDownloadedAlign = nAlign; }
protected:
	int m_nDownloadedAlign;
};

/////////////////////////////////////////////////////////////////////////////
// CDownloadDialog dialog

class CDownloadDialog : public CDialog
{
// Construction
public:
	CDownloadDialog(CDownloadCollectionArray* pCollections, CString* pcsDownloadDirectory, LPCSTR pszChosenCollection = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDownloadDialog)
	enum { IDD = IDD_COLLECTION_DOWNLOAD };
	CString	m_csDownloadDirectory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDownloadDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkchangeCollectionList();
	afx_msg void OnDblclkCollectionList();
	afx_msg void OnBrowse();
	afx_msg void OnDeleteFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void BuildCollectionList(void);
	void UpdateDownloadButton(void);
	BOOL SomethingChecked(void);
	BOOL DoDelete(CDownloadCollection& Collection, LPCSTR pszDirectory);

protected:
	CCollectionListBox m_CollectionList;
	CDownloadCollectionArray* m_pCollections;
	LPCSTR m_pszChosenCollection;
	CString* m_pcsDownloadDirectory;
	static UINT m_uContentChangedMessage;

};

/////////////////////////////////////////////////////////////////////////////
// CCollectionProgressDialog dialog

class CCollectionProgressDialog : public CDownloadStatusDialog
{
// Construction
public:
	CCollectionProgressDialog();   // standard constructor

	void SetCollectionsLeft(int nCollectionsLeft);
	void SetCurrentCollection(LPCSTR pszCollection);
	void SetFilesLeft(int nFilesLeft);

// Dialog Data
	//{{AFX_DATA(CCollectionProgressDialog)
	enum { IDD = IDD_COLLECTION_DOWNLOAD_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollectionProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCollectionProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CString m_csCollectionsLeftFormat;
	CString m_csCurrentCollectionFormat;
	CString m_csFilesLeftFormat;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNDLG_H__553F9671_6838_11D2_9863_00A0246D4780__INCLUDED_)
