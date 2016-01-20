/*
// $Workfile: WCONGRAT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WCONGRAT.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:48   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:34   JAY
// Reverted. New series.
// 
//    Rev 1.0   22 Jun 1994 18:54:54   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "wsinstr.h"
#include "util.h"
#include "wcongrat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkstationCongratulationsDialog dialog


CWorkstationCongratulationsDialog::CWorkstationCongratulationsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkstationCongratulationsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkstationCongratulationsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CWorkstationCongratulationsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkstationCongratulationsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWorkstationCongratulationsDialog, CDialog)
	//{{AFX_MSG_MAP(CWorkstationCongratulationsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWorkstationCongratulationsDialog message handlers

BOOL CWorkstationCongratulationsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	Util::CenterWindowInWindow(this);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}
