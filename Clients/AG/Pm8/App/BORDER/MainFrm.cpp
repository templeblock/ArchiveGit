// MainFrm.cpp : implementation file
//

#include "stdafx.h"
#include "border.h"
#include "MainFrm.h"
#include "Bordview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

CMainFrame::CMainFrame()
{
} 

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	return (CFrameWnd::LoadFrame(IDR_MAINFRAME, dwDefaultStyle, CWnd::FromHandle(theBorderApp.GetParent()), pContext));
}


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_SIZING()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	HBITMAP hToolBmp = theBorderApp.GetCallbacks()->GetToolbarBitmap();
	if(hToolBmp)
		m_wndToolBar.SetBitmap(hToolBmp);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

void CMainFrame::OnClose()
{
	// Clean up this instance
	theBorderApp.ExitApp();

}

// Override RunModalLoop to correctly process Idle messages.
int CMainFrame::RunModalLoop(DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd)); // window must be created
	ASSERT(!(m_nFlags & WF_MODALLOOP)); // window must not already be in modal state

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	BOOL bShowIdle = (dwFlags & MLF_SHOWONIDLE) && !(GetStyle() & WS_VISIBLE);
	HWND hWndParent = theBorderApp.GetParent();
	m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL);
	MSG* pMsg = &AfxGetThread()->m_msgCur;

	// acquire and dispatch messages until the modal state is done
	for (;;)
	{
		ASSERT(ContinueModal());

#ifdef _MAC
		// PeekMessage is particularly expensive because it calls the Event Manager,
		// so don't call it if we don't need to
		if (bShowIdle && !GetInputState())
		{
			ShowWindow(SW_SHOWNORMAL);
			UpdateWindow();
			bShowIdle = FALSE;
		}
#endif

		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			ASSERT(ContinueModal());

			// show the dialog when the message queue goes idle
			if (bShowIdle)
			{
				ShowWindow(SW_SHOWNORMAL);
				UpdateWindow();
				bShowIdle = FALSE;
			}

			// call OnIdle while in bIdle state
			if (!(dwFlags & MLF_NOIDLEMSG) && hWndParent != NULL && lIdleCount == 0)
			{
				// send WM_ENTERIDLE to the parent
				::SendMessage(hWndParent, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);
			}
			if (!theBorderApp.OnIdle(lIdleCount++))
			{
				// stop idle processing next time
				bIdle = FALSE;
			}
		}

		// phase2: pump messages while available
		do
		{
			ASSERT(ContinueModal());

			// pump message, but quit on WM_QUIT
			if (!AfxGetThread()->PumpMessage())
			{
				AfxPostQuitMessage(0);
				return -1;
			}

			// show the window when certain special messages rec'd
			if (bShowIdle &&
				(pMsg->message == 0x118 || pMsg->message == WM_SYSKEYDOWN))
			{
				ShowWindow(SW_SHOWNORMAL);
				UpdateWindow();
				bShowIdle = FALSE;
			}

			if (!ContinueModal())
				goto ExitModal;

			// reset "no idle" state after pumping "normal" message
			if (AfxGetThread()->IsIdleMessage(pMsg))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE));
	}

ExitModal:
	m_nFlags &= ~(WF_MODALLOOP|WF_CONTINUEMODAL);
	return m_nModalResult;
}

void CMainFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam) 
{	
	// Enable maximum drawing
	CBordview* pView = (CBordview*)GetActiveView();
	pView->DrawMinimum(FALSE);
	pView->Invalidate(FALSE);
}


void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CFrameWnd::OnPaletteChanged(pFocusWnd);
	
	// always realize the palette for the active view
	CView* pView = GetActiveView();
	ASSERT(pView != NULL);
	TRACE("OnPaletteChanged...\n");

	// notify all child windows that the palette has changed
	SendMessageToDescendants(WM_DOREALIZE, (WPARAM)pView->m_hWnd);
	
}

BOOL CMainFrame::OnQueryNewPalette() 
{
	// always realize the palette for the active view
	CView* pView = GetActiveView();
	ASSERT(pView != NULL);
	TRACE("OnQueryNewPalette...\n");

	// just notify the target view
	pView->SendMessage(WM_DOREALIZE, (WPARAM)pView->m_hWnd);
	
	return CFrameWnd::OnQueryNewPalette();
}


void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnSizing(fwSide, pRect);
	
	// Enable minimum drawing
	CBordview* pView = (CBordview*)GetActiveView();
	pView->DrawMinimum(TRUE);
	pView->Invalidate(FALSE);
	
}

int CMainFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
   extern BOOL PopupIsActive(void);
   if (nHitTest == HTCLIENT && message == WM_LBUTTONDOWN)
   {
      if (PopupIsActive())
      {
         return MA_ACTIVATEANDEAT;
      }
   }

	return CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	
	// Show the helpful hint on top
	if (nState == WA_ACTIVE)
	{
		theBorderApp.GetCallbacks()->DisplayHelpfulHint();
	}
}
