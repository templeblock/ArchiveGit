/*
// $Workfile: IOPTIONS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IOPTIONS.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:18   Fred
// Initial revision.
// 
//    Rev 1.0   23 Apr 1996 08:26:20   Jay
// Initial revision.
// 
//    Rev 2.2   19 Apr 1996 10:05:06   JAY
// Now suuports previous installation directory.
// 
//    Rev 2.1   08 Feb 1995 13:39:38   JAY
// Reverted. New series.
// 
//    Rev 1.3   22 Jun 1994 11:22:02   FRED
// Fixed bug where dialog was not properly initialized
// if the source and default destination were on the
// same drive.
// 
// 
//    Rev 1.2   22 Jun 1994 08:34:32   FRED
// Does not move focus to "continue" when enter
// is pressed on workspace size edit field.
// 
// 
//    Rev 1.1   21 Jun 1994 15:37:40   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.0   15 Jun 1994 18:59:18   FRED
// Initial revision.
*/ 

#ifndef _IOPTIONS_H_
#define _IOPTIONS_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallOptionsDialog dialog

class CInstallOptionsDialog : public CPmwDialog
{
// Construction
public:
	CInstallOptionsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallOptionsDialog)
	enum { IDD = IDD_INSTALL_OPTIONS };
	//}}AFX_DATA
	
	CString	m_csInstallDirectory;
	CString	m_csPreviousInstallDirectory;
	DWORD		m_dwProgramSize;
	DWORD		m_dwWorkspaceSize;
	
protected:
	CString	m_csAvailableDiskSpaceName;
	BOOL		m_fValidateWorkspaceSize;
	BOOL		m_fCancelPressed;
	BOOL		m_fRedrawInstallDirectory;
	BOOL		m_fRedrawWorkspaceSize;
	BOOL		m_fRedrawDiskRequirements;
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallOptionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetInstallDirectory();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKillfocusWorkspaceSize();
	virtual void OnOK();
	afx_msg LRESULT OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSetfocusWorkspaceSize();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	void FreeAllocatedData(void);
	
private:
	void RedrawInterface(void);
	void SetInstallDirectory(const CString &csDirectory, BOOL fForceDriveChange = FALSE);
	void SetWorkspaceSize(DWORD dwSize);
};

#endif
