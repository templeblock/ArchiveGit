/*
// $Header: /PM8/App/WORKDLG.H 1     3/03/99 6:14p Gbeddow $
//
// workdlg.h : header file
//
// $Log: /PM8/App/WORKDLG.H $
// 
// 1     3/03/99 6:14p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:14   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:50   JAY
// Reverted. New series.
// 
//    Rev 1.0   22 Jun 1994 13:47:24   JAY
// Initial revision.
*/

#ifndef __WORKDLG_H__
#define __WORKDLG_H__

#include "pmwdlg.h"
#include "cdcache.h"

#define	MINIMUM_WORKSPACE_SIZE	 1000000L
#define	MAXIMUM_WORKSPACE_SIZE	99000000L
#define	DEFAULT_WORKSPACE_SIZE	 9000000L

#define	MAKE_DOUBLE(l) (((double)(((l)+5000L)/10000L))/100.0)

/////////////////////////////////////////////////////////////////////////////
// CWorkspacePreferencesDialog dialog

class CWorkspacePreferencesDialog : public CPmwDialog
{
// Construction
public:
	CWorkspacePreferencesDialog(CDCache *cache, CWnd* pParent = NULL);	// standard constructor
	void FreeAllocatedData(void);

// Dialog Data
	//{{AFX_DATA(CWorkspacePreferencesDialog)
	enum { IDD = IDD_WORKSPACE_PREFERENCES };
	CEdit	m_workspace_edit;
	//}}AFX_DATA

	DWORD m_dwWorkspaceSize;
	CDCache *m_cache;

protected:
	CString	m_csWorkspaceDirectory;
	CString	m_csAvailableDiskSpaceName;
	BOOL		m_fWorkspaceSizeChanged;
	BOOL		m_fRedrawInstallDirectory;
	BOOL		m_fRedrawWorkspaceSize;
	BOOL		m_fRedrawDiskRequirements;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWorkspacePreferencesDialog)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKillfocusWorkspaceSize();
	virtual void OnOK();
	afx_msg void OnChangeWorkspaceSize();
	afx_msg LRESULT OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSetfocusWorkspaceSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RedrawInterface(void);
	void SetWorkspaceSize(DWORD dwSize);
};

#endif						/* #ifndef __WORKDLG_H__ */
