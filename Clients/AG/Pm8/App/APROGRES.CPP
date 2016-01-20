/*
// $Workfile: APROGRES.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/APROGRES.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:06   Fred
// Initial revision.
// 
//    Rev 1.3   19 Jun 1997 18:08:14   johno
// 
//    Rev 1.2   19 Jun 1997 11:20:50   johno
// 
//    Rev 1.1   18 Jun 1997 16:41:56   johno
// 
//    Rev 1.0   17 Jun 1997 14:06:14   johno
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:32:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:58   FRED
// Initial revision.
// 
//    Rev 2.4   24 Jan 1996 10:03:18   JAY
// New string management in Configuration
// 
//    Rev 2.3   24 Jan 1996 08:43:44   JAY
//  
// 
//    Rev 2.2   07 Aug 1995 17:14:56   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:37:38   JAY
// Reverted. New series.
// 
//    Rev 1.0   16 Jun 1994 18:16:00   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "cprogbar.h"
#include "util.h"
#include "aprogres.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddonProgressDialog dialog


CAddonProgressDialog::CAddonProgressDialog(int IDD, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddonProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	BOOL b = Create(IDD, pParent); 
   ASSERT (b != FALSE);
}

void CAddonProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddonProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddonProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CAddonProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddonProgressDialog::Breathe(void)
{
	MSG Message;

	while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		if ((m_hWnd == 0) || !IsDialogMessage(&Message))
		{
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddonProgressDialog message handlers

void CAddonProgressDialog::OnCancel()
{
	// make sure the user really wants to cancel the installation
	
	if (GetConfiguration()->MessageBox(IDS_ADDON_CONFIRM_ABORT, 0, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		m_fIsAborted = TRUE;
	}
}

BOOL CAddonProgressDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	Util::CenterWindowInWindow(this);
	GetConfiguration()->ReplaceDialogText(this);

	m_fIsAborted = FALSE;
	//m_pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR);
   //ASSERT (m_pProgressBar != NULL);
	HideAction();
	HideItem();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
