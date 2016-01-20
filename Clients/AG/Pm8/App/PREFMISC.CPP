/*
// $Workfile: PREFMISC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFMISC.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 4     11/20/98 1:33p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 3     5/28/98 12:23p Hforman
// remove Welcome stuff; add voice choice for Helpful Hints
// 
// 2     12/16/97 5:37p Hforman
// change CPreferencesMiscPage to CPreferencesHelpPage, add Helpful Hints
// stuff
// 
//    Rev 1.0   14 Aug 1997 15:24:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:02   Fred
// Initial revision.
// 
//    Rev 1.3   22 May 1997 12:51:46   johno
// New controls, functionality for beta
// 
// 
//    Rev 1.2   21 May 1997 18:17:04   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.1   21 Apr 1997 19:15:52   johno
// cleanup
// 
//    Rev 1.0   21 Apr 1997 16:51:32   johno
// Initial revision.
// 
*/

#include "stdafx.h"

#include "pmw.h"
#include "prefmisc.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesHelpPage property page

CPreferencesHelpPage::CPreferencesHelpPage()
   : CPmwPropertyPage(CPreferencesHelpPage::IDD)
{
   //{{AFX_DATA_INIT(CPreferencesHelpPage)
	m_fShowDesignTips = FALSE;
	m_fShowHelpfulHints = FALSE;
	m_fPlayVoice = FALSE;
	m_fAutoInstructions = TRUE;
	//}}AFX_DATA_INIT
}

CPreferencesHelpPage::~CPreferencesHelpPage()
{
}

void CPreferencesHelpPage::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertyPage::DoDataExchange(pDX);
   
   //{{AFX_DATA_MAP(CPreferencesHelpPage)
	DDX_Check(pDX, IDC_HINTS_SHOW_DESIGNTIPS, m_fShowDesignTips);
	DDX_Check(pDX, IDC_HINTS_SHOW_HINTS, m_fShowHelpfulHints);
	DDX_Check(pDX, IDC_HINTS_PLAY_VOICE, m_fPlayVoice);
	DDX_Check(pDX, IDC_HELP_AUTO_INSTRUCTIONS, m_fAutoInstructions);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesHelpPage, CPmwPropertyPage)
   //{{AFX_MSG_MAP(CPreferencesHelpPage)
	ON_BN_CLICKED(IDC_HINTS_RESET, OnHintsReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPreferencesHelpPage::OnInitDialog()
{
	if(!GetConfiguration()->SupportsInstructionPage())
	{
		CWnd* pWnd = GetDlgItem(IDC_HELP_AUTO_INSTRUCTIONS);
		if(pWnd)
			pWnd->ShowWindow(SW_HIDE);		
	}
	
   return CPmwPropertyPage::OnInitDialog();
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesHelpPage message handlers


void CPreferencesHelpPage::OnHintsReset() 
{
	GET_PMWAPP()->ResetHelpfulHints();
}
