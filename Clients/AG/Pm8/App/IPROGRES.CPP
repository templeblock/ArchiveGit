/*
// $Workfile: IPROGRES.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IPROGRES.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
//    Rev 1.0   14 Aug 1997 15:21:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:54   Fred
// Initial revision.
// 
//    Rev 1.2   20 Jun 1997 16:47:12   Fred
// Progress bar *is* in permanent table, so switched back to pointer
// 
//    Rev 1.1   20 Jun 1997 11:53:06   Fred
// Fixed bug with pointer to temporary CWnd
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

#include "install.h"
#include "cprogbar.h"
#include "util.h"
#include "installr.h"	// needed for IDS_...
#include "iprogres.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallProgressDialog dialog


CInstallProgressDialog::CInstallProgressDialog(int IDD, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	m_fIsAborted = FALSE;
	m_pProgressBar = NULL;
	m_pAbort = NULL;

	//{{AFX_DATA_INIT(CInstallProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD, pParent);
}

void CInstallProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallProgressDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CInstallProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallProgressDialog::Breathe(void)
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

void CInstallProgressDialog::HideAnimation(void)
{
	CAnimateCtrl* pAnimation = (CAnimateCtrl*)GetDlgItem(IDC_ANIMATION);
	if (pAnimation != NULL)
	{
		pAnimation->Close();
	}
}

void CInstallProgressDialog::ShowAnimation(LPCSTR pszResource)
{
	CAnimateCtrl* pAnimation = (CAnimateCtrl*)GetDlgItem(IDC_ANIMATION);
	if (pAnimation != NULL)
	{
		pAnimation->Close();
		pAnimation->Open(pszResource);
		pAnimation->Play(0, -1, -1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInstallProgressDialog message handlers

void CInstallProgressDialog::OnCancel()
{
	// make sure the user really wants to cancel the installation
	
	if (GetConfiguration()->MessageBox(IDS_ConfirmExitInstallation, 0, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		m_fIsAborted = TRUE;
	}
}

BOOL CInstallProgressDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	Util::CenterWindowInWindow(this);
	GetConfiguration()->ReplaceDialogText(this);

	m_fIsAborted = FALSE;

	m_pProgressBar = (CProgressBar*)GetDlgItem(IDC_PROGRESS_BAR);
	ASSERT(m_pProgressBar != NULL);

	m_pAbort = GetDlgItem(IDCANCEL);
	ASSERT(m_pAbort != NULL);

	HideAction();
	HideItem();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
