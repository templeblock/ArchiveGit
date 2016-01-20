/*
// $Workfile: PMWDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWDLG.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 6     3/01/99 1:40p Mwilson
// changed base class
// 
// 5     7/28/98 9:39a Jayn
// INSTALL ifdefs
// 
// 4     7/26/98 7:11p Hforman
// moved OnQueryNewPalette() and OnPaletteChanged() here from
// CPmwDialogColor
// 
// 3     2/27/98 2:48p Jayn
// ifdefs for install program
// 
// 2     12/16/97 5:39p Hforman
// Helpful Hint removal
// 
//    Rev 1.0   14 Aug 1997 15:24:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:50   Fred
// Initial revision.
// 
//    Rev 1.4   11 Apr 1997 11:23:36   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.3   28 Mar 1997 17:10:54   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.2   06 Sep 1996 18:14:36   Jay
// Sound changes
// 
//    Rev 1.1   21 Jun 1996 17:50:46   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:37:26   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 10:31:18   Jay
// Changes from 2.1
// 
//    Rev 2.14   23 Feb 1996 08:55:38   JAY
// Fixed activation problem in 16-bit version.
// 
//    Rev 2.13   24 Jan 1996 10:03:20   JAY
// New string management in Configuration
// 
//    Rev 2.12   23 Jan 1996 12:31:30   JAY
// Configuration in INSTALL. Strings, etc.
// 
//    Rev 2.11   26 Dec 1995 10:30:18   FRED
// Project structure changes for mac port
// 
//    Rev 2.10   06 Oct 1995 15:23:46   FRED
// Win32 SendMessage() changes
// 
//    Rev 2.9   31 Aug 1995 07:39:02   JAY
// Photo CD dialog support
// 
//    Rev 2.8   30 Aug 1995 15:16:58   FRED
//  
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdlg.h"
#include "util.h"
#include "pmwcfg.h"

#ifdef _WIN32
#include <..\src\afximpl.h>
#else
#include <auxdata.h>
#endif

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwDialog dialog

CPmwDialog::CPmwDialog(UINT idd, CWnd* pParent /*=NULL*/)
	: CHybridBaseDlg(idd, pParent)
{
	m_sound_id = idd;
	//{{AFX_DATA_INIT(CPmwDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hFont = NULL;
	m_nNormalWidth = m_nNormalHeight = 0;
}

CPmwDialog::~CPmwDialog()
{
	if (m_hFont != NULL)
	{
		::DeleteObject(m_hFont);
	}
	
	POSITION Position = m_ChildPosition.GetStartPosition();

	while (Position != NULL)
	{
		void* pKey;
		void* pValue;
		
		m_ChildPosition.GetNextAssoc(Position, pKey, pValue);
		
		if (pValue != NULL)
		{
			delete pValue;
			pValue = NULL;
		}
	}
}

#if 0
void CPmwDialog::DoDataExchange(CDataExchange* pDX)
{
	CHybridBaseDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPmwDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
#endif

BEGIN_MESSAGE_MAP(CPmwDialog, CHybridBaseDlg)
	//{{AFX_MSG_MAP(CPmwDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
   ON_WM_MOUSEACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmwDialog message handlers

BOOL CPmwDialog::OnInitDialog()
{
	standard_dialog_startup(this, m_sound_id);

	BOOL fResult = CHybridBaseDlg::OnInitDialog();

	// Record the full size of the dialog.
	
	CRect crClient;
	GetClientRect(crClient);

	m_nNormalWidth = crClient.right;
	m_nNormalHeight = crClient.bottom;

	// Record the size and positions of the child windows.

	EnumChildWindows(GetSafeHwnd(), RecordChildWindow, (LPARAM)this);

/* Perform the default now. */

	return fResult;
}

void CPmwDialog::OnDestroy()
{
	standard_dialog_shutdown(this, m_sound_id);

	CHybridBaseDlg::OnDestroy();
}

LRESULT CPmwDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, m_sound_id, lParam);
}

void CPmwDialog::EnableDlgItem(int nId, BOOL fEnable)
{
	CWnd* pItem;

	if ((pItem = GetDlgItem(nId)) != NULL)
	{
		pItem->EnableWindow(fEnable);
	}
}

void CPmwDialog::SizeDialog(int nWidth, int nHeight)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	if (((long)m_nNormalWidth*(long)nHeight) < ((long)m_nNormalHeight*(long)nWidth))
	{
		nWidth = MulDiv(nHeight, m_nNormalWidth, m_nNormalHeight);
	}

	else
	{
		nHeight = MulDiv(nWidth, m_nNormalHeight, m_nNormalWidth);
	}

	m_nTargetWidth = nWidth;
	m_nTargetHeight = nHeight;

	LOGFONT lfFont = m_lfFont;		
	lfFont.lfHeight = MulDiv(lfFont.lfHeight, m_nTargetHeight, m_nNormalHeight);
	m_hFont = ::CreateFontIndirect(&lfFont);
	
	if (m_hFont != NULL)
	{
		EnumChildWindows(GetSafeHwnd(), SizeChildWindow, (LPARAM)this);
	}

//	SetWindowPos(NULL, 0, 0, m_nTargetWidth, m_nTargetHeight, SWP_NOZORDER);
}

BOOL CALLBACK CPmwDialog::RecordChildWindow(HWND hWindow, LPARAM lData)
{
	CPmwDialog* pDialog = (CPmwDialog*)lData;
	
	CRect* pcrWindow = new CRect;
	::GetWindowRect(hWindow, pcrWindow);
	
	CRect crDialog;
	pDialog->GetClientRect(crDialog);
	pDialog->ClientToScreen(crDialog);
	
	pcrWindow->left   -= crDialog.left;
	pcrWindow->top    -= crDialog.top;
	pcrWindow->right  -= crDialog.left;
	pcrWindow->bottom -= crDialog.top;
	
	pDialog->m_ChildPosition.SetAt((LPVOID)hWindow, pcrWindow);
	
	return TRUE;
}

BOOL CALLBACK CPmwDialog::SizeChildWindow(HWND hWindow, LPARAM lData)
{
	CPmwDialog* pDialog = (CPmwDialog*)lData;
	
	void* pValue;
	
	if (pDialog->m_ChildPosition.Lookup((void*)hWindow, pValue))
	{
		CRect crChild = *((CRect*)pValue);
		
		crChild.left   = MulDiv(crChild.left,   pDialog->m_nTargetWidth,  pDialog->m_nNormalWidth);
		crChild.top    = MulDiv(crChild.top,    pDialog->m_nTargetHeight, pDialog->m_nNormalHeight);
		crChild.right  = MulDiv(crChild.right,  pDialog->m_nTargetWidth,  pDialog->m_nNormalWidth);
		crChild.bottom = MulDiv(crChild.bottom, pDialog->m_nTargetHeight, pDialog->m_nNormalHeight);
		
		::SetWindowPos(hWindow, NULL, crChild.left, crChild.top, crChild.Width(), crChild.Height(), SWP_NOZORDER);
		::SendMessage(hWindow, WM_SETFONT, (WPARAM)(pDialog->m_hFont), MAKELPARAM(TRUE, 0));
		
		char szClassName[64];
		
		::GetClassName(hWindow, szClassName, sizeof(szClassName));
		
		if (lstrcmpi(szClassName, "LISTBOX") == 0)
		{
			MEASUREITEMSTRUCT mis;
				
			mis.CtlType = ODT_LISTBOX;
			mis.CtlID = ::GetDlgCtrlID(hWindow);
			mis.itemWidth = 0;
			mis.itemHeight = 0;
				
			DWORD dwStyle = ::GetWindowLong(hWindow, GWL_STYLE);
			
			if ((dwStyle & LBS_OWNERDRAWFIXED) != 0)
			{
				mis.itemID = 0;
				mis.itemData = 0;
				
				if (pDialog->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
				{
#ifdef WIN32
					::SendMessage(hWindow, LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
					::SendMessage(hWindow, LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
				}
				
				// This seems to force the list box to figure out if it needs to show the scrollbar.
								
				::SendMessage(hWindow, LB_SETHORIZONTALEXTENT, (WPARAM)0, (LPARAM)0);
			}
			
			if ((dwStyle & LBS_OWNERDRAWVARIABLE) != 0)
			{
				UINT nCount = (UINT)::SendMessage(hWindow, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				
				for (mis.itemID = 0; mis.itemID < nCount; mis.itemID++)
				{
					mis.itemData = ::SendMessage(hWindow, LB_GETITEMDATA, (WPARAM)(mis.itemID), (LPARAM)0);
				
					if (pDialog->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
					{
#ifdef WIN32
						::SendMessage(hWindow, LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
						::SendMessage(hWindow, LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
					}
				}
			}
		}
	}

	return TRUE;
}

void CPmwDialog::GetDoubleValue(CDataExchange* pDX,
											UINT uID,
											double& dValue,
											BOOL& fValue,
											double dMin,
											double dMax)
{
	CString csValue;
	DDX_Text(pDX, uID, csValue);

	if (csValue.IsEmpty())
	{
		fValue = FALSE;
	}
	else
	{
		DDX_Text(pDX, uID, dValue);
		DDV_MinMaxDouble(pDX, dValue, dMin, dMax);
		fValue = TRUE;
	}
}

/*
// Set a double value (intelligently).
*/

void CPmwDialog::SetDoubleValue(UINT uID, double dValue, int nPrecision, BOOL fValue /*=TRUE*/)
{
	CString csValue;
	if (fValue)
	{
		Util::FormatDouble(dValue, nPrecision, csValue);
	}
	else
	{
		csValue.Empty();
	}
	SetDlgItemText(uID, csValue);
}

/*
// Get an integer value.
*/

void CPmwDialog::GetIntValue(CDataExchange* pDX,
									  UINT uID,
									  int& nValue,
									  BOOL& fValue,
									  int nMin,
									  int nMax)
{
	CString csValue;
	DDX_Text(pDX, uID, csValue);

	if (csValue.IsEmpty())
	{
		fValue = FALSE;
	}
	else
	{
		DDX_Text(pDX, uID, nValue);
		DDV_MinMaxInt(pDX, nValue, nMin, nMax);
		fValue = TRUE;
	}
}

/*
// Set an integer value (intelligently).
*/

void CPmwDialog::SetIntValue(UINT uID, int nValue, BOOL fValue /*=TRUE*/)
{
	CString csValue;
	if (fValue)
	{
		csValue.Format("%d", nValue);
	}
	else
	{
		csValue.Empty();
	}
	SetDlgItemText(uID, csValue);
}

void CPmwDialog::OnSetFont(CFont* pFont)
{
	CHybridBaseDlg::OnSetFont(pFont);
	pFont->GetObject(sizeof(m_lfFont), &m_lfFont);
}

void CPmwDialog::OnSize(UINT nType, int cx, int cy)
{
	CHybridBaseDlg::OnSize(nType, cx, cy);

	if (m_nNormalWidth != 0 && m_nNormalHeight != 0)
	{
		CRect crClient;
		GetClientRect(crClient);

		SizeDialog(crClient.right, crClient.bottom);
	}
	
	standard_dialog_resize(this, m_sound_id);
}

/*
// The standard startup for a dialog.
// This is only used by dialogs not derived from CPmwDialog (which means that
// they are derived from something else - otherwise they should be!).
*/

void standard_dialog_startup(CWnd *pDlg, UINT sound_id, CHybridBaseDlgManagerState* pState /*=NULL*/)
{
/* Make sure we're centered. */

#ifdef WIN32
	pDlg->CenterWindow();
#else
	// CenterWindow() has an activation bug in 16-bit.
	// So use ours.
	Util::CenterWindowInWindow(pDlg);
#endif

/* Replace necessary text in the dialog strings. */

	GetConfiguration()->ReplaceDialogText(pDlg);
}

/*
// The standard shutdown for a dialog.
// This is only used by dialogs not derived from CPmwDialog (which means that
// they are derived from something else - otherwise they should be!).
*/

void standard_dialog_shutdown(CWnd *pDlg, UINT sound_id, CHybridBaseDlgManagerState* pState /*=NULL*/)
{
}

void standard_dialog_activate(UINT sound_id, UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
}

/*
// The standard resize for a dialog.
// This is only used by dialogs not derived from CPmwDialog (which means that
// they are derived from something else - otherwise they should be!).
*/

void standard_dialog_resize(CWnd *pDlg, UINT sound_id, CHybridBaseDlgManagerState* pState /*=NULL*/)
{
}

LRESULT standard_dialog_hit_test(CWnd *pDlg, UINT sound_id, LPARAM lParam)
{
	HWND hDlgWnd = pDlg->GetSafeHwnd();
	HWND hWnd;

	if ((hWnd = Util::ChildWindowFromPointEx(hDlgWnd, CPoint(lParam))) == NULL)
	{
		return 0;
	}
	else if (hWnd != hDlgWnd)
	{
		UINT nID = _AfxGetDlgCtrlID(hWnd);
		od("child id is %d\r\n", nID);
		if (nID != 0 && nID != (UINT)-1)
			return nID;
	}

/* Default action. */

	od("Dialog\r\n");
	return HID_BASE_RESOURCE+sound_id;
}

void CPmwDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	standard_dialog_activate(m_sound_id, nState, pWndOther, bMinimized);
	CHybridBaseDlg::OnActivate(nState, pWndOther, bMinimized);
}

int CPmwDialog::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
#if 0
	TRACE("Mouse Activate: %u; %x\n", nHitTest, message);
#endif

#ifndef INSTALL
	GET_PMWAPP()->RemoveHelpfulHint();
#endif
	return CHybridBaseDlg::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CPmwDialog::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
#ifndef INSTALL
	GET_PMWAPP()->RemoveHelpfulHint();
#endif
	CHybridBaseDlg::OnNcLButtonDown(nHitTest, point);
}

#ifndef INSTALL
void CPmwDialog::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CHybridBaseDlg::OnPaletteChanged(pFocusWnd);
	
	if (pFocusWnd == this)
		return ;

	OnQueryNewPalette();
}

BOOL CPmwDialog::OnQueryNewPalette() 
{
	int i = 0;
	CDC *pDC;

	if ((pOurPal != NULL) && (pOurPal->m_hObject != NULL))
	{
		if ((pDC = GetDC()) != NULL)
		{
			CPalette *pOldPal = pDC->SelectPalette (pOurPal, FALSE);
			// Find out how many colors changed
			if (pOldPal != NULL)
			{
				i = pDC->RealizePalette();

				pDC->SelectPalette (pOldPal, FALSE);
			}

			ReleaseDC (pDC);
		}
   
		if (i > 0)
		{
			RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
			AfxGetMainWnd()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
		}
	}

	return i;
}
#endif
