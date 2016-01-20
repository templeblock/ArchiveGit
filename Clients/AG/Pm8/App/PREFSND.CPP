/*
// $Workfile: PREFSND.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFSND.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:02   Fred
// Initial revision.
// 
//    Rev 1.3   22 May 1997 12:52:16   johno
// New controls, functionality for beta
// 
// 
//    Rev 1.2   21 May 1997 18:16:10   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.1   07 Nov 1996 11:28:58   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:34:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:32   FRED
// Initial revision.
// 
//    Rev 1.1   02 Sep 1995 15:04:54   FRED
// More changes to make help work
// 
//    Rev 1.0   16 Jul 1995 15:08:16   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "prefsnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSoundPage property page

CPreferencesSoundPage::CPreferencesSoundPage()
	: CPmwPropertyPage(CPreferencesSoundPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesSoundPage)
   m_SoundEnabled = TRUE;
   m_SoundEnabledNot = FALSE;
	//}}AFX_DATA_INIT
}

CPreferencesSoundPage::~CPreferencesSoundPage()
{
}

void CPreferencesSoundPage::DoDataExchange(CDataExchange* pDX)
{
	m_SoundEnabledNot = !m_SoundEnabled;
   
   CPmwPropertyPage::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CPreferencesSoundPage)
   DDX_Check(pDX, IDC_SOUND_ON, m_SoundEnabled);
   DDX_Check(pDX, IDC_SOUND_OFF, m_SoundEnabledNot);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CPreferencesSoundPage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesSoundPage)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSoundPage message handlers

