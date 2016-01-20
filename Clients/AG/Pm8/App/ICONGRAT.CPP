/*
// $Workfile: ICONGRAT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ICONGRAT.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:44   FRED
// Initial revision.
// 
//    Rev 2.2   07 Aug 1995 17:13:58   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:36:32   JAY
// Reverted. New series.
// 
//    Rev 1.0   16 Jun 1994 18:16:28   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "pmwdlg.h"
#include "icongrat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallCongratulationsDialog dialog


CInstallCongratulationsDialog::CInstallCongratulationsDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallCongratulationsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallCongratulationsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CInstallCongratulationsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallCongratulationsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallCongratulationsDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallCongratulationsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInstallCongratulationsDialog message handlers
