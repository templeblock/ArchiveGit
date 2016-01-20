/*
// $Workfile: IDEFAULT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IDEFAULT.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     6/24/98 8:26a Jayn
// 
//    Rev 1.0   14 Aug 1997 15:21:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:44   Fred
// Initial revision.
// 
//    Rev 1.1   23 Apr 1996 08:25:26   Jay
//  
// 
//    Rev 2.4   19 Apr 1996 10:05:12   JAY
// Now suuports previous installation directory.
// 
//    Rev 2.3   24 Jan 1996 08:43:42   JAY
//  
// 
//    Rev 2.2   07 Aug 1995 17:14:54   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:36:32   JAY
// Reverted. New series.
// 
//    Rev 1.1   21 Jun 1994 15:37:24   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.0   15 Jun 1994 19:01:38   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "util.h"
#include "pmwdlg.h"
#include "idefault.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallDefaultDialog dialog


CInstallDefaultDialog::CInstallDefaultDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallDefaultDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallDefaultDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CInstallDefaultDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallDefaultDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallDefaultDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallDefaultDialog)
	ON_BN_CLICKED(IDC_CUSTOM_INSTALLATION, OnCustomInstallation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallDefaultDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csDirectory.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CInstallDefaultDialog message handlers

void CInstallDefaultDialog::OnCustomInstallation()
{
	EndDialog(IDC_CUSTOM_INSTALLATION);
}

BOOL CInstallDefaultDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	   
	if (m_csDirectory.IsEmpty())
	{
		LoadConfigurationString(IDS_DefaultInstallDirectory, m_csDirectory);
	}

	CInstallApp* pApp = GetInstallApp();
	m_dwProgramSize = pApp->GetProgramSize(m_csDirectory);
	m_dwWorkspaceSize = pApp->GetRecommendedWorkspaceSize(m_csDirectory);
	
	if ((m_csDirectory.IsEmpty())
	 || (m_dwProgramSize == DISK_SPACE_NOT_AVAILABLE)
	 || (m_dwWorkspaceSize == DISK_SPACE_NOT_AVAILABLE))
	{
		EndDialog(IDOK);
	}
	
	SetDlgItemText(IDC_INSTALL_DIRECTORY, m_csDirectory);
	
	Util::CenterWindowInWindow(this);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallDefaultDialog::OnCancel()
{
	/*
	// Confirm that the user really does want to cancel.
	*/
	
	if (GetConfiguration()->MessageBox(IDS_ConfirmExitInstallation, 0, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		CPmwDialog::OnCancel();
	}
}
