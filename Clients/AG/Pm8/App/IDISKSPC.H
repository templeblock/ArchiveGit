/*
// $Workfile: IDISKSPC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IDISKSPC.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:12   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:49:24   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:36   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 19:00:12   FRED
// Initial revision.
*/ 

#ifndef _IDISKSPC_H_
#define _IDISKSPC_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallDiskSpaceDialog dialog

class CInstallDiskSpaceDialog : public CPmwDialog
{
// Construction
public:
	CInstallDiskSpaceDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallDiskSpaceDialog)
	enum { IDD = IDD_INSTALL_DISK_SPACE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	virtual int CheckDiskSpace(const CString &csDirectory, DWORD dwNeededSize);
	virtual void FreeAllocatedData(void);
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	virtual void CInstallDiskSpaceDialog::UpdateDiskSpace(void);

	// Generated message map functions
	//{{AFX_MSG(CInstallDiskSpaceDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwNeededSize;
	DWORD m_dwAvailableSize;
	CString m_csDirectory;
	CString m_csDrive;
	BOOL m_fAborted;
};

#endif
