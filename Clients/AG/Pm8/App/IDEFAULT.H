/*
// $Workfile: IDEFAULT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IDEFAULT.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:10   Fred
// Initial revision.
// 
//    Rev 1.0   23 Apr 1996 08:26:24   Jay
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:34   JAY
// Reverted. New series.
// 
//    Rev 1.1   21 Jun 1994 15:37:34   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.0   15 Jun 1994 19:00:10   FRED
// Initial revision.
*/ 

#ifndef _IDEFAULT_H_
#define _IDEFAULT_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallDefaultDialog dialog

class CInstallDefaultDialog : public CPmwDialog
{
// Construction
public:
	CInstallDefaultDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallDefaultDialog)
	enum { IDD = IDD_INSTALL_DEFAULT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	virtual void FreeAllocatedData(void);
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallDefaultDialog)
	afx_msg void OnCustomInstallation();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	CString m_csDirectory;
	DWORD m_dwProgramSize;
	DWORD m_dwWorkspaceSize;
};

#endif
