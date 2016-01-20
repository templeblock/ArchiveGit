/*
// $Workfile: BASEDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/BASEDLG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:36   Fred
// Initial revision.
// 
//    Rev 1.2   10 Jun 1997 15:43:20   Fred
// Cleanup
// 
//    Rev 1.1   20 May 1997 16:07:40   johno
// Changes for PrintMaster integration
// 
//    Rev 1.0   20 May 1997 10:41:26   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/BASEDLG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.1   08 Sep 1996 09:51:56   Fred
// Disabled sound for PMW 3.0
// 
//    Rev 1.0   08 Sep 1996 09:40:20   Fred
// Initial revision.
// 
//    Rev 1.2   02 Feb 1996 13:44:02   FRED
// Added palette support to bitmap control
// 
//    Rev 1.1   02 Feb 1996 13:04:20   JAY
//  
// 
//    Rev 1.0   02 Feb 1996 08:58:28   JAY
// Initial revision.
// 
//    Rev 1.0   05 Dec 1995 13:03:08   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "license.h"
#include "basedlg.h"

#ifdef _WIN32
//#include <..\src\afximpl.h>
#else
#include <auxdata.h>
#endif

#include <afxpriv.h>			// for WM_HELPHITTEST

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseDialog dialog

CBaseDialog::CBaseDialog(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
{
#pragma message("embed dialog into printmaster here.")
	m_nID = nID;

	//{{AFX_DATA_INIT(CBaseDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
	//{{AFX_MSG_MAP(CBaseDialog)
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseDialog message handlers

BOOL CBaseDialog::OnInitDialog() 
{
//	GetApp()->EventNotify(UPGRADE_EVENT_DialogStartup, GetSafeHwnd(), m_nID);

	return CDialog::OnInitDialog();
}

void CBaseDialog::OnDestroy() 
{
//	GetApp()->EventNotify(UPGRADE_EVENT_DialogShutdown, GetSafeHwnd(), m_nID);

	CDialog::OnDestroy();
}

void CBaseDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	switch (nState)
	{
		case WA_INACTIVE:
		{
//			GetApp()->EventNotify(UPGRADE_EVENT_DialogDeactivate, GetSafeHwnd(), m_nID);
			break;
		}
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
		{
//			GetApp()->EventNotify(UPGRADE_EVENT_DialogActivate, GetSafeHwnd(), m_nID);
			break;
		}
		default:
		{
			break;
		}
	}
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

BOOL CBaseDialog::OnQueryNewPalette()
{
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	return TRUE;
}

void CBaseDialog::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (pFocusWnd != this)
	{
		OnQueryNewPalette();
	}
}

LRESULT CBaseDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	HWND hDlgWnd = GetSafeHwnd();
	HWND hWnd;

	if ((hWnd = ::ChildWindowFromPoint(hDlgWnd, CPoint(lParam))) == NULL)
	{
		return 0;
	}
	else if (hWnd != hDlgWnd)
	{
		UINT nID = ::GetDlgCtrlID(hWnd);
		if (nID != 0 && nID != (UINT)-1)
			return nID;
	}

/* Default action. */

	return HID_BASE_RESOURCE+m_nID;
}
