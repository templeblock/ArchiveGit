/*
// $Workfile: SCROLLGA.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SCROLLGA.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 2     4/30/98 4:33p Fredf
// Fixed bug with gadgets not being displayed.
// 
//    Rev 1.1   02 Oct 1997 09:38:24   Jay
// Fix for print preview
// 
//    Rev 1.0   14 Aug 1997 15:25:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.11   18 Mar 1997 13:23:18   Jay
// Preview stuff
// 
//    Rev 1.10   16 Jan 1997 13:02:48   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.9   03 Aug 1996 16:43:20   Fred
//  
// 
//    Rev 1.8   02 Aug 1996 15:40:54   Fred
//  
// 
//    Rev 1.7   02 Aug 1996 15:02:22   Fred
// 16-bit changes
// 
//    Rev 1.6   02 Aug 1996 13:11:18   Fred
// More 16-bit changes
// 
//    Rev 1.5   31 Jul 1996 11:58:02   Fred
// 16-bit compatibility
// 
//    Rev 1.4   30 Jul 1996 09:12:18   Jay
// Defaults m_dwStyle in gadget constructors.
// 
//    Rev 1.3   25 Jul 1996 18:21:34   Fred
//  
// 
//    Rev 1.2   24 Jul 1996 13:18:30   Fred
// Gadget improvements
// 
//    Rev 1.1   23 Jul 1996 16:54:00   Fred
// Start of button/view hookup
// 
//    Rev 1.0   23 Jul 1996 08:16:42   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "util.h"
#include "scrollga.h"
#include "toolbar.h"

#ifdef _WIN32
#include <..\src\afximpl.h>
#else
#include <auxdata.h>
#endif

#include <afxpriv.h>

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMlsGadget
// Base class for gadgets.
// Warning: Gadgets are only set up to work next to the status bar.

//WNDPROC CMlsGadget::m_pfnSuperWndProc = NULL;

CMlsGadget::CMlsGadget()
{
	m_hFont = NULL;
	m_fInSizeParent = FALSE;
#ifdef WIN32
	m_dwStyle = 0;
#endif
}

CMlsGadget::~CMlsGadget()
{

	DeleteFont();
}

void CMlsGadget::DeleteFont(void)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

CSize CMlsGadget::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// determine size of font being used.
	TEXTMETRIC tm;
	{
		CClientDC dc(NULL);
		HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
		HGDIOBJ hOldFont = NULL;
		if (hFont != NULL)
			hOldFont = dc.SelectObject(hFont);
		VERIFY(dc.GetTextMetrics(&tm));
		if (hOldFont != NULL)
			dc.SelectObject(hOldFont);
	}

	CSize czSize;
	czSize.cx = tm.tmAveCharWidth*20;
	czSize.cy = tm.tmHeight - tm.tmInternalLeading - 1 + ::GetSystemMetrics(SM_CYBORDER) * 2 + 9;

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame != NULL)
	{
		if (pMainFrame->IsZoomed())
		{
			czSize.cy += 3;
		}

		if (!pMainFrame->StatusBar().IsWindowVisible())
		{
			czSize.cx = 0;
		}
	}

	if (m_fInSizeParent)
	{
		czSize.cx = min(czSize.cx, m_czAvailableSize.cx);
		czSize.cy = min(czSize.cy, m_czAvailableSize.cy);
	}

	if ((czSize.cx > 0) && (!GadgetVisible()))
	{
		czSize.cx = 0;
	}

	return czSize;
}

void CMlsGadget::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	UpdateDialogControls(this, /*pTarget,*/ bDisableIfNoHndler);
}

BEGIN_MESSAGE_MAP(CMlsGadget, CControlBar)
	//{{AFX_MSG_MAP(CMlsGadget)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
END_MESSAGE_MAP()

LRESULT CMlsGadget::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lPARAM)
{                              
	// Nomrmally, CControlBar passes WM_COMMAND to parent. We want to change this for gadgets.
	if (uMessage == WM_COMMAND)
	{
		return CWnd::WindowProc(uMessage, wParam, lPARAM);
	}
	else
	{
		return CControlBar::WindowProc(uMessage, wParam, lPARAM);
	}
}

CPmwView* CMlsGadget::GetCurrentView(void)
{
	CPmwView* pView = NULL;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame != NULL && !pMainFrame->InPreviewMode())
	{
		CFrameWnd* pActiveFrame = pMainFrame->GetActiveFrame();
		if (pActiveFrame != NULL)
		{
			pView = (CPmwView*)(pActiveFrame->GetActiveView());
			if (pView != NULL && !pView->IsKindOf(RUNTIME_CLASS(CPmwView)))
			{
				// Must be a CPmwView.
				pView = NULL;
			}
		}
	}

	return pView;
}

UINT CMlsGadget::GadgetId(void)
{
	return GetDlgCtrlID();
}

BOOL CMlsGadget::GadgetVisible()
{
	BOOL fResult = FALSE;

	CPmwView* pView = GetCurrentView();
	if (pView != NULL)
	{
		fResult = pView->GadgetVisible(GadgetId());
	}

	return fResult;
}

BOOL CMlsGadget::GadgetEnabled(UINT uPart)
{
	BOOL fResult = FALSE;

	CPmwView* pView = GetCurrentView();
	if (pView != NULL)
	{
		fResult = pView->GadgetEnabled(GadgetId(), uPart);
	}

	return fResult;
}

void CMlsGadget::GadgetClicked(UINT uPart)
{
	CPmwView* pView = GetCurrentView();
	if (pView != NULL)
	{
		pView->GadgetClicked(GadgetId(), uPart, this);
	}
}

void CMlsGadget::GadgetText(CString& csText)
{
	csText.Empty();

	CPmwView* pView = GetCurrentView();
	if (pView != NULL)
	{
		pView->GadgetText(GadgetId(), csText);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsGadget message handlers

LRESULT CMlsGadget::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;

	m_fInSizeParent = TRUE;

	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

	CRect crRect;
	crRect.CopyRect(&lpLayout->rect);

	// Get the size of the gadget.
	m_czAvailableSize = crRect.Size();  // maximum size available
	CSize size = CalcFixedLayout(FALSE, TRUE);

	crRect.top = crRect.bottom-size.cy;
	crRect.right = crRect.left+size.cx;

	SetRectangle(crRect);

	if (lpLayout->hDWP != NULL)
	{
#ifdef WIN32
		AfxRepositionWindow(lpLayout, m_hWnd, crRect);
#else
		_AfxRepositionWindow(lpLayout, m_hWnd, crRect);
#endif
	}

	m_fInSizeParent = FALSE;

	return lResult;
}

LRESULT CMlsGadget::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	return INHERITED::OnHelpHitTest(wParam, lParam);
}

BOOL CMlsGadget::OnEraseBkgnd(CDC* pDC) 
{
	CRect crClient;
	GetClientRect(crClient);
	Util::FillRectangleWithColor(*pDC, crClient, GetSysColor(COLOR_BTNFACE));
	return TRUE;
}

LRESULT CMlsGadget::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	HFONT hNewFont = NULL;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			hNewFont = CreateFontIndirect(&LogFont);
		}
	}
		
	DeleteFont();
	m_hFont = hNewFont;
	
	if (LOWORD(lParam) != 0)
	{
		Invalidate();
	}
	
	return 0;
}

LRESULT CMlsGadget::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(UINT)m_hFont;
}

/////////////////////////////////////////////////////////////////////////////
// CMlsSeparatorGadget

CMlsSeparatorGadget::CMlsSeparatorGadget()
{
#ifdef WIN32
	m_dwStyle = CBRS_ALIGN_BOTTOM;
#endif
}

CMlsSeparatorGadget::~CMlsSeparatorGadget()
{
}

void CMlsSeparatorGadget::DoPaint(CDC* pDC)
{
	if (GadgetVisible())
	{
		CRect crClient;
		GetClientRect(crClient);
		if (!crClient.IsRectEmpty())
		{
			CRect crRect(0, 0, 1, crClient.Height());
			Util::FillRectangleWithColor(*pDC, crRect, GetSysColor(COLOR_BTNSHADOW));
			crRect.OffsetRect(1, 0);
			Util::FillRectangleWithColor(*pDC, crRect, GetSysColor(COLOR_BTNHILIGHT));
		}
	}
}

CSize CMlsSeparatorGadget::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize czSize = INHERITED::CalcFixedLayout(bStretch, bHorz);

	if (czSize.cx > 0)
	{
		czSize.cx = 6;
	}

	return czSize;
}

BOOL CMlsSeparatorGadget::GadgetVisible(void)
{ 
	BOOL fResult = FALSE;

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	if (pMainWnd != NULL)
	{
		CToolBar* pEdit2Bar = pMainWnd->GetToolBar(TOOLBAR_EDIT2);
		fResult = (pEdit2Bar->m_hWnd != NULL) && (pEdit2Bar->IsWindowVisible());
	}

	return fResult;
}

BEGIN_MESSAGE_MAP(CMlsSeparatorGadget, CMlsGadget)
	//{{AFX_MSG_MAP(CMlsSeparatorGadget)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsSeparatorGadget message handlers

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetButton

CMlsScrollGadgetButton::CMlsScrollGadgetButton()
{
}

CMlsScrollGadgetButton::~CMlsScrollGadgetButton()
{
}

BEGIN_MESSAGE_MAP(CMlsScrollGadgetButton, CButtonWnd)
	//{{AFX_MSG_MAP(CMlsScrollGadgetButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetButton message handlers

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetPanel
// Variation of CMlsPanel which sends ID command on mouse click.

CMlsScrollGadgetPanel::CMlsScrollGadgetPanel()
{
}

CMlsScrollGadgetPanel::~CMlsScrollGadgetPanel()
{
}

BEGIN_MESSAGE_MAP(CMlsScrollGadgetPanel, CMlsPanel)
	//{{AFX_MSG_MAP(CMlsScrollGadgetPanel)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadgetPanel message handlers

void CMlsScrollGadgetPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Let our owner know that the button was pressed.
	CWnd* pOwner = GetOwner();
	if (pOwner != NULL)
	{
		Util::SendWmCommand(pOwner, GetDlgCtrlID(), NULL, 0);
	}
}

void CMlsScrollGadgetPanel::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Turn double clicks into single clicks.
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void CMlsScrollGadgetPanel::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Let our owner know that the button was pressed.
	CWnd* pOwner = GetOwner();
	if (pOwner != NULL)
	{
		Util::SendWmCommand(pOwner, GetDlgCtrlID(), NULL, 0);
	}
}

void CMlsScrollGadgetPanel::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// Turn double clicks into single clicks.
	SendMessage(WM_RBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadget
// Base class for scrolling gadgets.

CMlsScrollGadget::CMlsScrollGadget()
{
#ifdef WIN32
	m_dwStyle = CBRS_ALIGN_BOTTOM;
#endif
}

CMlsScrollGadget::~CMlsScrollGadget()
{
}

void CMlsScrollGadget::DoPaint(CDC* pDC)
{
	// Do nothing!
}

CSize CMlsScrollGadget::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize czSize = INHERITED::CalcFixedLayout(bStretch, bHorz);

	if ((m_fInSizeParent) && (czSize.cx > 0))
	{
		CRect crBack;
		CRect crText;
		CRect crNext;
		if (!GetButtonRectangles(czSize, crBack, crText, crNext))
		{
			czSize.cx = 0;
		}
	}

	return czSize;
}

void CMlsScrollGadget::SetButtons(UINT uBackButton, UINT uNextButton)
{
	CBitmap* pBitmap = NULL;

	CRect crRect;
	crRect.SetRectEmpty();

	// Set up the back bitmap.
	TRY
	{
		if (m_BackButton.m_hWnd == NULL)
		{
			m_BackButton.Create(NULL, "", WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, crRect, this, ID_GADGET_BACK, NULL);
		}
		pBitmap = new CBitmap;
		if (pBitmap->LoadBitmap(uBackButton))
		{
			m_BackButton.SetBitmap(pBitmap, TRUE, FALSE);
			pBitmap = NULL;
		}
	}
	CATCH_ALL(e)
	{
		delete pBitmap;
	}
	END_CATCH_ALL
			
	// Set up the next bitmap.
	TRY
	{
		if (m_NextButton.m_hWnd == NULL)
		{
			m_NextButton.Create(NULL, "", WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, crRect, this, ID_GADGET_NEXT, NULL);
		}
		pBitmap = new CBitmap;
		if (pBitmap->LoadBitmap(uNextButton))
		{
			m_NextButton.SetBitmap(pBitmap, TRUE, FALSE);
			pBitmap = NULL;
		}
	}
	CATCH_ALL(e)
	{
		delete pBitmap;
	}
	END_CATCH_ALL

	// Set up the middle panel.
	TRY
	{
		if (m_TextPanel.m_hWnd == NULL)
		{
			m_TextPanel.Create(NULL, "|SN", WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, crRect, this, ID_GADGET_TEXT);
		}
	}
	END_TRY
}

BOOL CMlsScrollGadget::GetButtonRectangles(CSize czSize, CRect& crBack, CRect& crText, CRect& crNext)
{
	int nXBorder = 6;
	int nYBorder = 2;
	int nY = czSize.cy-nYBorder;

	if (AfxGetMainWnd()->IsZoomed())
	{
		nY -= 3;
	}

	int nX = nY;

	crBack.SetRect(0, nYBorder, nX, nY+nYBorder);
	crText.SetRect(nX-1, nYBorder, czSize.cx-(nX+nXBorder)+2, nY+nYBorder);
	crNext.SetRect(czSize.cx-(nX+nXBorder), nYBorder, czSize.cx-nXBorder, nY+nYBorder);

	return crText.Width() > 3;
}

void CMlsScrollGadget::PositionButtons(void)
{
	CRect crBack;
	CRect crText;
	CRect crNext;
	if (GetButtonRectangles(m_crRect.Size(), crBack, crText, crNext))
	{
		if (m_BackButton.m_hWnd != NULL)
		{
			m_BackButton.MoveWindow(crBack);
			m_BackButton.SetFont(GetFont());
			m_BackButton.ShowWindow(SW_SHOW);
		}
		if (m_NextButton.m_hWnd != NULL)
		{
			m_NextButton.MoveWindow(crNext);
			m_NextButton.SetFont(GetFont());
			m_NextButton.ShowWindow(SW_SHOW);
		}
		if (m_TextPanel.m_hWnd != NULL)
		{
			m_TextPanel.MoveWindow(crText);
			m_TextPanel.SetFont(GetFont());
			m_TextPanel.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (m_BackButton.m_hWnd != NULL)	m_BackButton.ShowWindow(SW_HIDE);
		if (m_NextButton.m_hWnd != NULL)	m_NextButton.ShowWindow(SW_HIDE);
		if (m_TextPanel.m_hWnd != NULL)	m_TextPanel.ShowWindow(SW_HIDE);
	}
}

BEGIN_MESSAGE_MAP(CMlsScrollGadget, CMlsGadget)
	//{{AFX_MSG_MAP(CMlsScrollGadget)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_GADGET_BACK, OnUpdateBack)
	ON_UPDATE_COMMAND_UI(ID_GADGET_TEXT, OnUpdateText)
	ON_UPDATE_COMMAND_UI(ID_GADGET_NEXT, OnUpdateNext)
	ON_COMMAND(ID_GADGET_BACK, OnBack)
	ON_COMMAND(ID_GADGET_TEXT, OnText)
	ON_COMMAND(ID_GADGET_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsScrollGadget message handlers

void CMlsScrollGadget::OnSize(UINT nType, int cx, int cy) 
{
	PositionButtons();
}

void CMlsScrollGadget::OnUpdateBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GadgetEnabled(ID_GADGET_BACK));
}

void CMlsScrollGadget::OnUpdateText(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GadgetEnabled(ID_GADGET_TEXT));

	CString csText;
	TRY
	{
		GadgetText(csText);
		pCmdUI->SetText(csText);
	}
	END_TRY
}

void CMlsScrollGadget::OnUpdateNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GadgetEnabled(ID_GADGET_NEXT));
}

void CMlsScrollGadget::OnBack()
{
	GadgetClicked(ID_GADGET_BACK);
}

void CMlsScrollGadget::OnText()
{
	GadgetClicked(ID_GADGET_TEXT);
}

void CMlsScrollGadget::OnNext()
{
	GadgetClicked(ID_GADGET_NEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CMlsPageGadget
// Gadget for turning pages.

CMlsPageGadget::CMlsPageGadget()
{
}

CMlsPageGadget::~CMlsPageGadget()
{
}

BEGIN_MESSAGE_MAP(CMlsPageGadget, CMlsScrollGadget)
	//{{AFX_MSG_MAP(CMlsPageGadget)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsPageGadget message handlers

LRESULT CMlsPageGadget::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	if (lpLayout != NULL)
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame != NULL)
		{
			CMlsSeparatorGadget& SeparatorGadget = pMainFrame->SeparatorGadget();
			CRect crSeparatorGadget = SeparatorGadget.GetRectangle();
			lpLayout->rect.left = crSeparatorGadget.right;
			lpLayout->rect.bottom = crSeparatorGadget.bottom;
		}
	}

	LRESULT lResult = INHERITED::OnSizeParent(wParam, lParam);

	return lResult;
}

int CMlsPageGadget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	SetButtons(IDB_BACKPAGE, IDB_NEXTPAGE);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMlsZoomGadget
// Gadget for turning pages.

CMlsZoomGadget::CMlsZoomGadget()
{
}

CMlsZoomGadget::~CMlsZoomGadget()
{
}

BEGIN_MESSAGE_MAP(CMlsZoomGadget, CMlsScrollGadget)
	//{{AFX_MSG_MAP(CMlsZoomGadget)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMlsZoomGadget message handlers

LRESULT CMlsZoomGadget::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	if (lpLayout != NULL)
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame != NULL)
		{
			CMlsScrollGadget& PageGadget = pMainFrame->PageGadget();
			CRect crPageGadget = PageGadget.GetRectangle();
			lpLayout->rect.left = crPageGadget.right;
			lpLayout->rect.bottom = crPageGadget.bottom;
		}
	}

	LRESULT lResult = INHERITED::OnSizeParent(wParam, lParam);

	return lResult;
}

int CMlsZoomGadget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	SetButtons(IDB_BACKZOOM, IDB_NEXTZOOM);

	return 0;
}
