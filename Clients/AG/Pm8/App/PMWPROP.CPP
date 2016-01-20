/*
// $Workfile: PMWPROP.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWPROP.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 3     4/16/98 7:33p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
// 2     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:24:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:54   Fred
// Initial revision.
// 
//    Rev 1.2   28 Mar 1997 17:10:56   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.1   23 Dec 1996 17:34:34   johno
// No Longer re - position buttons
// 
//    Rev 1.0   14 Mar 1996 13:37:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:22   FRED
// Initial revision.
// 
//    Rev 1.7   23 Jan 1996 12:32:38   JAY
//  
// 
//    Rev 1.6   20 Dec 1995 09:56:22   JAY
// 32-bit fixes
// 
//    Rev 1.5   10 Nov 1995 17:39:24   JAY
// Now won't GP if sound manager is not init'ed.
// 
//    Rev 1.4   22 Sep 1995 15:42:56   JAY
// Made property pages be CLIPSIBLINGS
// 
//    Rev 1.3   13 Sep 1995 11:48:06   JAY
// New 32-bit changes
// 
//    Rev 1.2   03 Sep 1995 17:58:00   FRED
// Warp text dialog works with new tabbed dialog code
// 
//    Rev 1.1   03 Sep 1995 15:55:46   FRED
//  
// 
//    Rev 1.0   03 Sep 1995 12:37:40   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmwdlg.h"
#include "pmwprop.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwPropertyPage property page

CPmwPropertyPage::CPmwPropertyPage(WORD wID)
	: CPropertyPage(wID)
{
	//{{AFX_DATA_INIT(CPmwPropertyPage)
	//}}AFX_DATA_INIT
}

CPmwPropertyPage::~CPmwPropertyPage()
{
}

void CPmwPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPmwPropertyPage)
	//}}AFX_DATA_MAP
}

BOOL CPmwPropertyPage::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	ModifyStyle(0, WS_CLIPSIBLINGS);
	return CPropertyPage::OnNcCreate(lpCreateStruct);
}

BEGIN_MESSAGE_MAP(CPmwPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPmwPropertyPage)
	//}}AFX_MSG_MAP
	ON_WM_NCCREATE()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmwPropertyPage message handlers

LRESULT CPmwPropertyPage::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, m_nIDHelp, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CPmwPropertySheet

IMPLEMENT_DYNAMIC(CPmwPropertySheet, CPropertySheet)

CPmwPropertySheet::CPmwPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPmwPropertySheet::~CPmwPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CPmwPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPmwPropertySheet)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmwPropertySheet message handlers

int CPmwPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

 	return 0;
}

void CPmwPropertySheet::OnDestroy()
{
	standard_dialog_shutdown(this, GetSoundId());

	CPropertySheet::OnDestroy();
}

LRESULT CPmwPropertySheet::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, GetSoundId(), lParam);
}

void CPmwPropertySheet::OnHelp()
{
	OnCommandHelp(0, 0);
}

void CPmwPropertySheet::OnOK()
{
	m_nModalResult = IDOK;			/* Why doesn't it do this? */
	Default();
}

void CPmwPropertySheet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertySheet::OnActivate(nState, pWndOther, bMinimized);

	if ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE))
	{
		for (int i = 0; i < GetPageCount(); i++)
		{
			CPropertyPage* pPage = GetPage(i);
			if (pPage->GetSafeHwnd() == NULL)
			{
				SetActivePage(i);
			}
		}
	}
}

WORD CPmwPropertySheet::GetSoundId(void)
{
	CPmwPropertyPage* pPage = (CPmwPropertyPage*)GetActivePage();
	if (pPage != NULL)
	{
		return pPage->GetSoundId();
	}

	return 0;
}
