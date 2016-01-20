//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ProgStat.h 1     3/03/99 6:11p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ProgStat.h $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 8     10/26/98 6:47p Jayn
// Download manager.
// 
// 7     7/25/98 1:50p Jayn
// Doesn't show anything if 0 bytes.
// 
// 6     7/14/98 12:19p Psasse
// Added estimated time left functionality to progress bar
// 
// 5     7/12/98 5:31p Jayn
// Improved graphic download. Cleanup of code in general.
//////////////////////////////////////////////////////////////////////////////

#ifndef __PROGSTAT_H__
#define __PROGSTAT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDownloadStatusDialog dialog

class CDownloadStatusDialog : public CDialog
{
// Construction
public:
	CDownloadStatusDialog();   // standard constructor

	void Show(void);
	void Hide(void);

	BOOL GetAbortStatus (void)
		{  return m_fAbort;}
	void SetAbortStatus (const BOOL NewStatus)
		{  m_fAbort = NewStatus;}

	// Status management.
	void SetStatusText(LPCSTR pszText = NULL);

	// Show if it has been a while.
	void ShowIfTime(void);

	// Progress management.
	void OnProgress(ULONG ulProgress, ULONG ulProgressMax);
	void SetTimeProgress(LPCSTR szProgress);

	void SetStartTime (const ULONG NewTime)
		{  m_ulStartTime = NewTime;}

	// Dialog Data
	//{{AFX_DATA(CDownloadStatusDialog)
	enum { IDD = IDD_PROGRESS_STATUS };
	CProgressCtrl	m_ctrlProgressBar;
	CStatic	m_staProgress;
	CString	m_strProgress;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadStatusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDownloadStatusDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_fAbort;
	DWORD m_dwCreateTime;

	ULONG			m_ulStartTime;
	double		m_dTimeLeftLast;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
