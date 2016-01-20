/*
// $Workfile: CALBDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CALBDLG.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:18   Fred
// Initial revision.
// 
//    Rev 1.13   29 Aug 1996 07:49:42   Jay
// Now maximizes albums by default.
// 
//    Rev 1.12   24 Aug 1996 14:14:16   Fred
// Properly focuses album control
// 
//    Rev 1.11   17 Aug 1996 09:27:58   Fred
// Added null HWND check
// 
//    Rev 1.10   16 Aug 1996 10:56:04   Fred
// Fixed sound buttons sticking down on album
// 
//    Rev 1.9   05 Aug 1996 08:34:56   Fred
//  
// 
//    Rev 1.8   03 Aug 1996 16:43:30   Fred
//  
// 
//    Rev 1.7   29 Jul 1996 10:31:26   Jay
// 
//    Rev 1.6   29 Jul 1996 10:23:22   Jay
// 
//    Rev 1.5   28 Jul 1996 07:46:38   Fred
// More album control stuff
// 
//    Rev 1.4   26 Jul 1996 17:43:28   Fred
//  
// 
//    Rev 1.3   26 Jul 1996 15:24:04   Fred
// More stuff for new album controls
// 
//    Rev 1.2   26 Jul 1996 13:28:00   Fred
// New album controls
// 
//    Rev 1.1   11 Apr 1996 13:06:42   FRED
// Copied from PMW2
// 
//    Rev 2.34   27 Mar 1996 14:58:54   JAY
// Fixed open album/upgrade/gp problem
// 
//    Rev 2.33   26 Dec 1995 10:29:12   FRED
// Project structure changes for mac port
// 
//    Rev 2.32   20 Dec 1995 09:56:32   JAY
// 32-bit fixes
// 
//    Rev 2.31   11 Oct 1995 17:47:40   JAY
// Handles enabling correctly in WIN32
// 
//    Rev 2.30   06 Oct 1995 15:23:16   FRED
// Win32 SendMessage() changes
// 
//    Rev 2.29   13 Sep 1995 11:48:14   JAY
// New 32-bit changes
// 
//    Rev 2.28   04 Sep 1995 17:09:08   FRED
// Arrow and other keys work again
// 
//    Rev 2.27   02 Sep 1995 09:06:08   FRED
// Help fixes.
// 
//    Rev 2.26   29 Aug 1995 09:36:54   FRED
//  
// 
//    Rev 2.25   24 Aug 1995 07:42:20   JAY
// Added help hit test.
// 
//    Rev 2.24   22 Aug 1995 17:34:04   FRED
//  
// 
//    Rev 2.23   22 Aug 1995 11:34:08   FRED
// Some album code cleanup
// 
//    Rev 2.22   16 Aug 1995 11:03:46   JAY
// updates
// 
//    Rev 2.21   11 Aug 1995 14:40:18   FRED
// Album key search
// 
//    Rev 2.20   11 Aug 1995 07:15:58   JAY
// Misc. experiments
// 
//    Rev 2.19   10 Aug 1995 12:50:54   FRED
// Album menu items
// 
//    Rev 2.18   10 Aug 1995 07:04:56   JAY
// Removed Minimize button from album dialogs.
// 
//    Rev 2.17   09 Aug 1995 13:12:20   JAY
// Misc changes.
// 
//    Rev 2.16   08 Jul 1995 16:26:48   FRED
// Album refinement.
// 
// 
//    Rev 2.15   08 Jul 1995 13:13:44   FRED
// Still more album focus/navigation stuff!
// 
//    Rev 2.14   06 Jul 1995 09:36:50   JAY
// Window position remembering code.
// 
//    Rev 2.13   05 Jul 1995 15:19:16   FRED
// Some album focus stuff.
// 
//    Rev 2.12   03 Jul 1995 18:48:44   FRED
// Open project button
// 
//    Rev 2.11   03 Jul 1995 17:14:28   JAY
// Initial project window.
// 
//    Rev 2.10   26 Jun 1995 16:59:00   JAY
//  
// 
//    Rev 2.9   16 Jun 1995 17:36:22   FRED
// Start of Make your own
// 
//    Rev 2.8   14 Jun 1995 14:12:58   FRED
// Album messages
// 
//    Rev 2.7   09 Jun 1995 15:37:00   FRED
// Project Album Dialog stuff
// 
//    Rev 2.6   09 Jun 1995 10:36:20   FRED
// Uses new code adapted from MFC\SRC\PROPDLG for
// simulating a modelss dialog box in MFC.
// 
// 
//    Rev 2.5   09 Jun 1995 07:43:02   FRED
// Sound Manager changes
// 
//    Rev 2.4   07 Jun 1995 17:43:42   JAY
// Detachs the menu it uses in the window.
// Doesn't call "standard_dialog_shutdown" in destroy. Deactivate handles it.
// Adds some test projects.
// 
//    Rev 2.3   07 Jun 1995 16:45:46   FRED
// Album dialog and Project album dialog.
// 
// 
//    Rev 2.2   05 Jun 1995 16:51:58   JAY
// Now a CPmwDialog.
// 
//    Rev 2.1   05 Jun 1995 11:32:54   JAY
//  
*/

#include "stdafx.h"
#ifdef WIN32
#include <..\src\afximpl.h>			// for AfxGetSafeOwner()
#define _AfxGetSafeOwner AfxGetSafeOwner
#else
#include <auxdata.h>			// for _AfxGetSafeOwner()
#endif
#include <afxpriv.h>

#include "pmw.h"
#include "mainfrm.h"

#include "pmwdlg.h"
#include "calbdlg.h"
#include "cbook.h"
#include "cdlgitem.h"
#include "soundmgr.h"
#include "docitem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// implementation helpers

static BOOL PASCAL WalkPreTranslateTree(HWND hWndStop, MSG* pMsg)
{
	ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
	ASSERT(pMsg != NULL);

	// walk from the target window up to the hWndStop window checking
	//  if any window wants to translate this message

	for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
	{
		CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
		if (pWnd != NULL)
		{
			// target window is a C++ window
			if (pWnd->PreTranslateMessage(pMsg))
				return TRUE; // trapped by target window (eg: accelerators)
		}

		// got to hWndStop window without interest
		if (hWnd == hWndStop)
			break;
	}
	return FALSE;       // no special processing
}

/////////////////////////////////////////////////////////////////////////////
// CAlbumDialog dialog

CAlbumDialog::CAlbumDialog(LPCSTR pszCaption, CWnd* pParent, UINT nMenu, UINT wSoundID)
{
	//{{AFX_DATA_INIT(CAlbumDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pAlbum = NULL;
	
	m_nMenu = nMenu;
	
	m_wSoundID = wSoundID;
	
	m_pParentWnd = pParent;
	m_hFocusWnd = NULL;
	m_fParentDisabled = FALSE;
	m_fModeless = FALSE;
	m_csCaption = pszCaption;
	
	m_nIDHelp = wSoundID;
}

CAlbumDialog::~CAlbumDialog()
{
	delete m_pAlbum;
	    
	POSITION Pos = m_Register.GetStartPosition();
	CString Key;
	CObject* Value;
	
	while (Pos != NULL)
	{
		m_Register.GetNextAssoc(Pos, Key, Value);
		
		if (Value != NULL)
		{
			delete Value;
		}
	}
}

BEGIN_MESSAGE_MAP(CAlbumDialog, CWnd)
	//{{AFX_MSG_MAP(CAlbumDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDC_EXIT_ALBUM, OnExitAlbum)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_PAGE_BACK, OnPageBack)
	ON_BN_CLICKED(IDC_PAGE_NEXT, OnPageNext)
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(CAlbumControl::MsgSelect, OnAlbumSelect)
	ON_REGISTERED_MESSAGE(CAlbumControl::MsgDeselect, OnAlbumDeselect)
	ON_REGISTERED_MESSAGE(CAlbumControl::MsgChoose, OnAlbumChoose)
	ON_REGISTERED_MESSAGE(CAlbumControl::MsgInspect, OnAlbumInspect)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

int CAlbumDialog::DoModal()
{
	m_fModeless = FALSE;
	int nResult = IDABORT;

	// cannot call DoModal on a dialog already constructed as modeless
	ASSERT(m_hWnd == NULL);
	
	if (m_hWnd == NULL)
	{
		// allow OLE servers to disable themselves
		CWinApp* pApp = AfxGetApp();
		pApp->EnableModeless(FALSE);
	
		// find parent HWND
#ifdef WIN32
		CWnd* pParentWnd = CWnd::GetSafeOwner(m_pParentWnd);
#else
		CWnd* pParentWnd = CWnd::FromHandle(_AfxGetSafeOwner(m_pParentWnd));
#endif
		// create the dialog, then enter modal loop
		if (Create(pParentWnd, WS_SYSMENU|WS_POPUP|WS_CAPTION|WS_MAXIMIZEBOX|WS_THICKFRAME))
		{
			// disable parent (should not disable this window)
			m_fParentDisabled = FALSE;
			if (pParentWnd != NULL && pParentWnd->IsWindowEnabled())
			{
				pParentWnd->EnableWindow(FALSE);
				m_fParentDisabled = TRUE;
#ifdef WIN32
				EnableWindow(TRUE);
#endif
			}
			ASSERT(IsWindowEnabled());  // should not be disabled to start!
			SetActiveWindow();
	
			// for tracking the idle time state
			BOOL fShown = (GetStyle() & WS_VISIBLE) != 0;
			m_nID = -1;
	
			// acquire and dispatch messages until a WM_QUIT message is received.
			MSG msg;
			while (m_nID == -1 && m_hWnd != NULL)
			{
				// phase1: check to see if we can do idle work
				if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
				{
					// send WM_ENTERIDLE since queue is empty
					if (pParentWnd != NULL &&
							!(pParentWnd->GetStyle() & DS_NOIDLEMSG))
					{
#ifdef WIN32
						pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);
#else
						pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, MAKELONG(m_hWnd, 0));
#endif
					}
	
					if (!fShown)
					{
						// show and activate the window
						fShown = TRUE;
						ShowWindow(SW_SHOWNORMAL);
					}
				}
	
				// phase2: pump messages while available
				do
				{
					// pump message -- if WM_QUIT assume cancel and repost
					if (!PumpMessage())
					{
						::PostQuitMessage((int)msg.wParam);
						m_nID = IDCANCEL;
						break;
					}
	
				} while (m_nID == -1 && m_hWnd != NULL &&
					::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
			}
	
			nResult = m_nID;
			if (m_hWnd != NULL)
				EndDialog(nResult);
		}
	
		// allow OLE servers to enable themselves
		pApp->EnableModeless(TRUE);
	}

	return nResult;
}

BOOL CAlbumDialog::PumpMessage()
{
	ASSERT_VALID(this);

	MSG Msg;
	if (!::GetMessage(&Msg, NULL, NULL, NULL))
		return FALSE;

	//  let's see if the message should be handled at all
	if (CallMsgFilter(&Msg, MSGF_DIALOGBOX))
		return TRUE;
		
	// process this message
	if (!WalkPreTranslateTree(m_hWnd, &Msg))
	{
		::TranslateMessage(&Msg);
		::DispatchMessage(&Msg);
	}
	return TRUE;
}

BOOL CAlbumDialog::Create(CWnd* pParent, DWORD dwStyle, DWORD dwExStyle)
{
	BOOL fResult = FALSE;
	HMENU hMenu = NULL;

	TRY
	{
		// Load menu.
		
		if (dwStyle & WS_CHILD)
		{
		/* Menu is child id. */
			hMenu = (HMENU)m_nMenu;
		}
		else if (m_nMenu != 0)
		{
			hMenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(m_nMenu));

			if (hMenu == NULL)
			{
				AfxThrowResourceException();
			}
		}

		// Create dialog window.

		CRect crDialog;
		AfxGetMainWnd()->GetWindowRect(crDialog);
		pParent->ScreenToClient(crDialog);
		if (!CreateEx(
					dwExStyle,
					AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE+1)),
					m_csCaption,
					dwStyle,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					525,
					400,
					pParent->GetSafeHwnd(),
					hMenu,
					NULL))
		{
			AfxThrowResourceException();
		}

		// **** Menu is now owned by the window. ****
		
		hMenu = NULL;

// Maximized by default
		GET_PMWAPP()->LoadWindowPosition(this, WindowPositionSection(), SW_SHOWNORMAL, FALSE, FALSE);
// Not Maximized by default
//		GET_PMWAPP()->LoadWindowPosition(this, WindowPositionSection());

		fResult = TRUE;
	}
	END_TRY
	
	if (!fResult)
	{
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
			hMenu = NULL;
		}
		
		DestroyWindow();
	}
	
	return fResult;
}

void CAlbumDialog::EndDialog(int nEndID)
{
	ASSERT_VALID(this);

	m_nID = nEndID;
	DestroyWindow();
}

void CAlbumDialog::OnExitAlbum()
{
	OnClose();
}

BOOL CAlbumDialog::DestroyWindow()
{
	GET_PMWAPP()->SaveWindowPosition(this, WindowPositionSection());

	// re-enable parent if it was disabled
	CWnd* pParentWnd = m_pParentWnd != NULL ? m_pParentWnd : GetParent();
	if (m_fParentDisabled && pParentWnd != NULL)
		pParentWnd->EnableWindow();

	// transfer the focus to ourselves to give the active control
	//  a chance at WM_KILLFOCUS
	if (::GetActiveWindow() == m_hWnd && ::IsChild(m_hWnd, ::GetFocus()))
	{
		m_hFocusWnd = NULL;
		SetFocus();
	}
	// hide this window and move activation to the parent
	SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
		SWP_NOSIZE | SWP_NOZORDER);

	pParentWnd = GetParent();
	if (pParentWnd != NULL)
		pParentWnd->SetActiveWindow();

	// finally, destroy this window
	BOOL fResult = CWnd::DestroyWindow();

	return fResult;
}


void CAlbumDialog::Resize(void)
{
	CRect crClient;
	GetClientRect(&crClient);
	
	if (m_pAlbum != NULL)
	{
		if (!crClient.IsRectEmpty())
		{
			m_pAlbum->SetWindowPos(
				NULL, 
				crClient.left,
				crClient.top,
				crClient.right,
				crClient.bottom,
				SWP_NOZORDER|SWP_SHOWWINDOW);
		}
	}
}

/*
// Return the INI file section to use for saving the window's position.
// If this returns NULL, then the position is not saved or loaded.
*/

LPCSTR CAlbumDialog::WindowPositionSection(void)
{
	return NULL;
}

CAlbumControl* CAlbumDialog::MakeAlbum(void)
{
	CAlbumControl* pAlbum = NULL;
	
	TRY
	{
		pAlbum = new CAlbumControl;
		ASSERT(pAlbum != NULL);
		
		if (pAlbum != NULL)
		{
			CRect crClient;
			GetClientRect(&crClient);
						
			if (!pAlbum->Create(NULL, "Album Control", WS_CHILD|WS_VISIBLE, crClient, this, 0, NULL))
			{
				AfxThrowResourceException();
			}
		}
	}
	END_TRY
	
	return pAlbum;
}

void CAlbumDialog::InitDialog(void)
{
}

void CAlbumDialog::OnDestroy()
{
	CWnd::OnDestroy();
}

BOOL CAlbumDialog::PreTranslateMessage(MSG* pMsg)
{
	// The following was copied from CDialog::PreTranslateMessage().
	
	// for modeless processing (or modal)
	ASSERT(m_hWnd != NULL);

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	return FALSE;
}

void CAlbumDialog::CommandComponent(CBookComponent& Component)
{
	ASSERT(m_pAlbum != NULL);
	
	if (m_pAlbum != NULL)
	{
		m_pAlbum->CommandComponent(Component);
	}
}
			
/////////////////////////////////////////////////////////////////////////////
// CAlbumDialog message handlers

void CAlbumDialog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Resize();
}

int CAlbumDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create album control.
		
	ASSERT(m_pAlbum == NULL);
		
	BeginWaitCursor();

	m_pAlbum = MakeAlbum();
		
	// Initialize everything (kind of like InitDialog()).

	InitDialog();

	// Size everything.
		
	Resize();
	
	EndWaitCursor();

	// Add the sound buttons.
			
	return 0;   // success
}

LRESULT CAlbumDialog::OnCommandHelp(WPARAM, LPARAM lParam)
{
	if (lParam == 0 && m_nIDHelp != 0)
		lParam = HID_BASE_RESOURCE + m_nIDHelp;
	if (lParam != 0)
	{
		AfxGetApp()->WinHelp(lParam);
		return TRUE;
	}
	return FALSE;
}

LRESULT CAlbumDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, m_wSoundID, lParam);
}

void CAlbumDialog::OnPageBack()
{
	TRACE("Page Back\n");
	CBookComponent Component;
	Component.PageUpType();
	CommandComponent(Component);
}

void CAlbumDialog::OnPageNext()
{
	TRACE("Page Next\n");
	CBookComponent Component;
	Component.PageDownType();
	CommandComponent(Component);
}

void CAlbumDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	DoActivate(nState, pWndOther, bMinimized);
	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CAlbumDialog::DoActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (nState == WA_INACTIVE)
		m_hFocusWnd = ::GetFocus();

	if ((m_pAlbum != NULL) && (m_pAlbum->m_hWnd != NULL))
	{
		m_pAlbum->DoActivate(nState, pWndOther, bMinimized);
	}
}

void CAlbumDialog::OnOK()
{
	ASSERT_VALID(this);

	if (!m_fModeless)
		EndDialog(IDOK);
}

void CAlbumDialog::OnCancel()
{
	ASSERT_VALID(this);

	if (!m_fModeless)
		EndDialog(IDCANCEL);
}

void CAlbumDialog::OnClose()
{
	ASSERT_VALID(this);
	if (!m_fModeless)
		OnCancel();
	else
		CWnd::OnClose();
}

void CAlbumDialog::OnSetFocus(CWnd* pOldWnd)
{
	if (m_hFocusWnd != NULL)
	{
		ASSERT(m_hFocusWnd != NULL);
		::SetFocus(m_hFocusWnd);
	}
}

LRESULT CAlbumDialog::OnAlbumSelect(WPARAM wParam, LPARAM lParam)
{
	return AlbumSelect(wParam, (CBookComponent*)lParam);
}

LRESULT CAlbumDialog::OnAlbumDeselect(WPARAM wParam, LPARAM lParam)
{
	return AlbumDeselect(wParam, (CBookComponent*)lParam);
}

LRESULT CAlbumDialog::OnAlbumChoose(WPARAM wParam, LPARAM lParam)
{
	return AlbumChoose(wParam, (CBookComponent*)lParam);
}

LRESULT CAlbumDialog::OnAlbumInspect(WPARAM wParam, LPARAM lParam)
{
	return AlbumInspect(wParam, (CBookComponent*)lParam);
}

LRESULT CAlbumDialog::AlbumSelect(int wParam, CBookComponent* pComponent)
{
	if (pComponent != NULL)
	{
		if (pComponent->Type() == CBookComponent::CATEGORY)
		{
			CBookCategory* pCategory = pComponent->Category();
			if (pCategory != NULL)
			{
				if (pCategory->CategoryDataType() == DATATYPE_SimpleCategory)
				{
					if (m_pAlbum != NULL)
					{
						m_pAlbum->SetFocus();
					}
				}
			}
		}
	}
	return 0;
}

LRESULT CAlbumDialog::AlbumDeselect(int wParam, CBookComponent* pComponent)
{
	TRACE("AlbumDeselect\n");
	return 0;
}

LRESULT CAlbumDialog::AlbumChoose(int wParam, CBookComponent* pComponent)
{
	TRACE("AlbumChoose\n");
	return 0;
}

LRESULT CAlbumDialog::AlbumInspect(int wParam, CBookComponent* pComponent)
{
	TRACE("AlbumInspect\n");
	return 0;
}

CObject* CAlbumDialog::Register(const CString& csKey, CObject* pValue /*=NULL*/)
{
	CObject* pResult = NULL;
	
	TRY
	{
		if (!m_Register.Lookup(csKey, pResult))
		{
			if (pValue != NULL)
			{
				m_Register.SetAt(csKey, pValue);
				m_Register.Lookup(csKey, pResult);
			}
		}
	}
	END_TRY
	
	return pResult;
}

void CAlbumDialog::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
#ifdef WIN32
	AfxCancelModes(m_hWnd);
#else
	_AfxCancelModes(m_hWnd);
#endif

	if (bSysMenu)
		return;     /* don't support system menu */

	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
//	if (_afxTrackingMenu == pMenu->m_hMenu)
//		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
//	else
	{
		CMenu* pOurMenu = GetMenu();
		ASSERT(pOurMenu != NULL);
		if (pOurMenu != NULL)
		{
			int nIndexMax = pOurMenu->GetMenuItemCount();
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (pOurMenu->GetSubMenu(nIndex) == pMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = pOurMenu;
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       /* first item of popup can't be routed to */
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			/*
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			*/
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, /*m_bAutoMenuEnable &&*/ state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
//	CWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CAlbumDialog::FreeingAlbum(void)
{
	m_pAlbum->FreeingAlbum();
	EndDialog(IDCANCEL);
}

void CAlbumDialog::FixFocus(void)
{
	if (m_pAlbum != NULL)
	{
		m_pAlbum->FixFocus();
	}
}
