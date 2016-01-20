/*
// $Workfile: UPGRADLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADLG.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 3     10/13/98 11:57a Mwilson
// added craft upgrade
// 
// 2     4/21/98 9:56a Dennis
// Upgrade changes for American Greetings.
// 
//    Rev 1.0   14 Aug 1997 15:26:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.3   08 Sep 1996 09:40:14   Jay
// CPmwDialogs with no sound.
// 
//    Rev 1.2   15 May 1996 08:46:46   Jay
// From PMW2
// 
//    Rev 1.4   13 May 1996 13:15:54   FRED
// Trial Version
// 
//    Rev 1.3   26 Apr 1996 13:09:34   JAY
// More demo code
// 
//    Rev 1.2   24 Apr 1996 16:44:48   JAY
// New demo support
// 
//    Rev 1.1   21 Feb 1996 11:18:18   FRED
// Upgrade dialogs which explain feature deltas.
// 
//    Rev 1.0   12 Feb 1996 09:45:28   FRED
//  
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwcfg.h"
#include "upgradlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNeedToUpgradeDialog dialog


CNeedToUpgradeDialog::CNeedToUpgradeDialog(LPCSTR pszMessage, LPCSTR pszTitle, CWnd* pParent /*=NULL*/)
	: CPmwDialog(DialogId(pszMessage), pParent)
{
	// We have no sound for this. Turn off the sound buttons.
	// Take this out if/when a sound is recorded.
	set_sound_id((UINT)-1);

	TRY
		if (pszMessage != NULL)
		{
			m_csMessage = pszMessage;
		}
	END_TRY

	TRY
		if (pszTitle != NULL)
		{
			m_csTitle = pszTitle;
		}
	END_TRY

	//{{AFX_DATA_INIT(CNeedToUpgradeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNeedToUpgradeDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNeedToUpgradeDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNeedToUpgradeDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CNeedToUpgradeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Determine the dialog template ID to use.

UINT CNeedToUpgradeDialog::DialogId(LPCSTR pszMessage)
{
	switch (GetConfiguration()->ProductLevel())
	{
		case CPrintMasterConfiguration::Demo:
		{
			return (pszMessage == NULL) ?
						IDD_UPGRADE_FROM_DEMO :
						IDD_NEED_TO_UPGRADE_FROM_DEMO;
		}
		case CPrintMasterConfiguration::Classic:
		{
			return (pszMessage == NULL) ?
						IDD_UPGRADE_FROM_CLASSIC :
						IDD_NEED_TO_UPGRADE_FROM_CLASSIC;
		}
		case CPrintMasterConfiguration::BonusPack:
		{
			return (pszMessage == NULL) ?
						IDD_UPGRADE_FROM_BONUS_PACK :
						IDD_NEED_TO_UPGRADE_FROM_BONUS_PACK;
		}
		case CPrintMasterConfiguration::PrintStandard:
		{
			return (pszMessage == NULL) ?
						IDD_UPGRADE_FROM_PRINT_STANDARD:
						IDD_UPGRADE_FROM_PRINT_STANDARD;
		}
		case CPrintMasterConfiguration::CraftStandard:
		{
			return (pszMessage == NULL) ?
						IDD_UPGRADE_FROM_CRAFT_STANDARD:
						IDD_UPGRADE_FROM_CRAFT_STANDARD;
		}
		default:
		{
			ASSERT(FALSE);
			return IDD_UPGRADE_FROM_CLASSIC;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNeedToUpgradeDialog message handlers

BOOL CNeedToUpgradeDialog::OnInitDialog()
{
	if (!m_csMessage.IsEmpty())
	{
		SetDlgItemText(IDC_MESSAGE, m_csMessage);
	}
	
	if (!m_csTitle.IsEmpty())
	{
		SetWindowText(m_csTitle);
	}
	
	CPmwDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
