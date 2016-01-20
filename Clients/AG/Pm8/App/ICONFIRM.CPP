/*
// $Workfile: ICONFIRM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ICONFIRM.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:42   FRED
// Initial revision.
// 
//    Rev 2.2   07 Aug 1995 17:13:56   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:36:30   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 19:01:36   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "util.h"
#include "pmwdlg.h"
#include "iconfirm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallConfirmDialog dialog


CInstallConfirmDialog::CInstallConfirmDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallConfirmDialog)
	m_csCompanyName = "";
	m_csUserName = "";
	//}}AFX_DATA_INIT
}

void CInstallConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallConfirmDialog)
	DDX_Text(pDX, IDC_COMPANY_NAME, m_csCompanyName);
	DDX_Text(pDX, IDC_USER_NAME, m_csUserName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallConfirmDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallConfirmDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallConfirmDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csUserName.Empty();
	m_csCompanyName.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CInstallConfirmDialog message handlers

BOOL CInstallConfirmDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
