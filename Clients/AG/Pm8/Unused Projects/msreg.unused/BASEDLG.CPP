/*
// $Workfile: BASEDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BASEDLG.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   28 Apr 1997 15:46:38   Fred
// Changed call to RedrawWindow() to handle palette with non-static colors
// 
//    Rev 1.1   02 Feb 1996 09:51:52   FRED
// Palette Support
// 
//    Rev 1.0   05 Dec 1995 13:03:08   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "msregx.h"
#include "basedlg.h"

#ifdef _WIN32
#include <afximpl.h>
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
	GetApp()->EventNotify(EVENT_DialogStartup, GetSafeHwnd(), m_nID);

	return CDialog::OnInitDialog();
}

void CBaseDialog::OnDestroy() 
{
	GetApp()->EventNotify(EVENT_DialogShutdown, GetSafeHwnd(), m_nID);

	CDialog::OnDestroy();
}

void CBaseDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	switch (nState)
	{
		case WA_INACTIVE:
		{
			GetApp()->EventNotify(EVENT_DialogDeactivate, GetSafeHwnd(), m_nID);
			break;
		}
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
		{
			GetApp()->EventNotify(EVENT_DialogActivate, GetSafeHwnd(), m_nID);
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
	RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN);
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
		UINT nID = _AfxGetDlgCtrlID(hWnd);
		if (nID != 0 && nID != (UINT)-1)
			return nID;
	}

/* Default action. */

	return HID_BASE_RESOURCE+m_nID;
}
