/*
// $Workfile: IREGISTR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IREGISTR.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:20   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:50:36   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:42   JAY
// Reverted. New series.
// 
//    Rev 1.1   21 Jun 1994 15:37:36   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.0   15 Jun 1994 18:59:20   FRED
// Initial revision.
*/ 

#ifndef _IREGISTR_H_
#define _IREGISTR_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallRegistrationDialog dialog

class CInstallRegistrationDialog : public CPmwDialog
{
// Construction
public:
	CInstallRegistrationDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallRegistrationDialog)
	enum { IDD = IDD_INSTALL_REGISTRATION };
	CString	m_csSerialNumber;
	CString	m_csUserName;
	CString	m_csCompanyName;
	//}}AFX_DATA

public:	
	virtual void FreeAllocatedData(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallRegistrationDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
