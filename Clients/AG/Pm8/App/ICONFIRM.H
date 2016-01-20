/*
// $Workfile: ICONFIRM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ICONFIRM.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:08   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:48:02   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:34   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 19:00:12   FRED
// Initial revision.
*/ 

#ifndef _ICONFIRM_H_
#define _ICONFIRM_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallConfirmDialog dialog

class CInstallConfirmDialog : public CPmwDialog
{
// Construction
public:
	CInstallConfirmDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallConfirmDialog)
	enum { IDD = IDD_INSTALL_CONFIRM };
	CString	m_csCompanyName;
	CString	m_csUserName;
	//}}AFX_DATA
	
public:
	virtual void FreeAllocatedData(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallConfirmDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
