/*
// $Workfile: PREFWRK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFWRK.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     5/28/98 12:32p Hforman
// change resource ID
// 
//    Rev 1.0   14 Aug 1997 15:24:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:56   FRED
// Initial revision.
// 
//    Rev 1.2   02 Sep 1995 15:07:22   FRED
// More changes to make help work
// 
//    Rev 1.1   01 Sep 1995 17:41:38   FRED
// Default workspace size is 5mb
// 
//    Rev 1.0   16 Jul 1995 15:08:20   FRED
// Initial revision.
*/

class CPreferencesWorkspacePage;

#ifndef __PREFWRK_H__
#define __PREFWRK_H__

#include "pmwdlg.h"
#include "cdcache.h"
#include "pmwprop.h"

#define	MINIMUM_WORKSPACE_SIZE	 1000000L
#define	MAXIMUM_WORKSPACE_SIZE	99000000L
#define	DEFAULT_WORKSPACE_SIZE	 5000000L

#define	MAKE_DOUBLE(l) (((double)(((l)+5000L)/10000L))/100.0)

/////////////////////////////////////////////////////////////////////////////
// CPreferencesWorkspacePage dialog

class CPreferencesWorkspacePage : public CPmwPropertyPage
{
// Construction
public:
	CPreferencesWorkspacePage();	// standard constructor
	virtual ~CPreferencesWorkspacePage();

// Dialog Data
	//{{AFX_DATA(CPreferencesWorkspacePage)
	enum { IDD = IDD_PREFS_WORKSPACE };
	CEdit	m_workspace_edit;
	//}}AFX_DATA

	DWORD m_dwWorkspaceSize;
	CDCache *m_pCache;
	
protected:
	CString	m_csWorkspaceDirectory;
	BOOL		m_fRedrawWorkspaceSize;
	BOOL		m_fRedrawDiskRequirements;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPreferencesWorkspacePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetfocusWorkspaceSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnSetActive();     // called when this page gets the focus
	
public:
	void FreeAllocatedData(void);

private:
	DWORD GetRecommendedWorkspaceSize(CString& csDirectory, DWORD dwCachePhysicalSize);
	void RedrawInterface(void);
	void SetWorkspaceSize(DWORD dwSize);
};

/////////////////////////////////////////////////////////////////////////////
#endif
