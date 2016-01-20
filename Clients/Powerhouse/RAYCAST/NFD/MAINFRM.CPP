/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <afxpriv.h>
#include "fd.h"

#include "mainfrm.h"
#include "fdstatus.h"
#include "fddoc.h"
#include "fdview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void PaintIcon(void);

/////////////////////////////////////////////////////////////////////////////
// Equates and Variables

CFrameWnd	*pFrameWnd;
BOOL		fIsIconic=FALSE;
BOOL		fActivateApp=TRUE;
int			StatusBarView=0, ToolBarView=0;
RECT		frmRect;
extern CPalette		*CPalPtr;
extern CFdView		*OurView;
extern CFdStatus	*fds;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTEISCHANGING()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ACTIVATEAPP()
	ON_MESSAGE(WM_QUERYDRAGICON, OnQueryDragIcon)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

int		x=GetSystemMetrics(SM_CXSCREEN) / 2 - FRAME_WIDTH / 2;
int		y=GetSystemMetrics(SM_CYSCREEN) / 2 - FRAME_HEIGHT / 2;
int		w=FRAME_WIDTH + GetSystemMetrics(SM_CXFRAME)  * 2;
int		h=FRAME_HEIGHT + GetSystemMetrics(SM_CYFRAME) * 2 +
				GetSystemMetrics(SM_CYCAPTION) +
				GetSystemMetrics(SM_CYMENU);

	MoveWindow(	AfxGetApp()->GetProfileInt("Preferences", "Window_X", x),
				AfxGetApp()->GetProfileInt("Preferences", "Window_Y", y),
				AfxGetApp()->GetProfileInt("Preferences", "Window_W", w),
				AfxGetApp()->GetProfileInt("Preferences", "Window_H", h));

	pFrameWnd = this;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	//	240 x 150
	lpMMI->ptMinTrackSize.x = 320 + GetSystemMetrics(SM_CXFRAME) * 2;
	lpMMI->ptMinTrackSize.y = 200 + GetSystemMetrics(SM_CYFRAME) * 2 +
								GetSystemMetrics(SM_CYCAPTION) +
								GetSystemMetrics(SM_CYMENU);
	lpMMI->ptMaxTrackSize.x = FULL_CX;
	lpMMI->ptMaxTrackSize.y = FULL_CY;
	lpMMI->ptMaxPosition.x = GetSystemMetrics(SM_CXBORDER) - 1;
	lpMMI->ptMaxPosition.y = GetSystemMetrics(SM_CYBORDER) - 1;
	lpMMI->ptMaxSize.x = FULL_CX;
	lpMMI->ptMaxSize.y = FULL_CY;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

////////////////////////////////////////////////////////////////////////////
//	GetIconWndClass was modified from the standard class library to allow
//	us to paint our own icon. We paint our whole screen as an icon
//	(when iconized) so people can see our status when inactive.

LPCSTR CMainFrame::GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource)
{
	ASSERT_VALID_IDR(nIDResource);
	HINSTANCE hInst = AfxGetResourceHandle();
	HICON hIcon = NULL;
	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(CREATESTRUCT));
	cs.style = dwDefaultStyle;
	PreCreateWindow(cs);
		// will fill lpszClassName with default WNDCLASS name
		// ignore instance handle from PreCreateWindow.

	WNDCLASS wndcls;
	if (cs.lpszClass != NULL &&
		GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndcls) &&
		wndcls.hIcon != hIcon)
	{
		// register a very similar WNDCLASS
		return AfxRegisterWndClass(wndcls.style,
			wndcls.hCursor, wndcls.hbrBackground, hIcon);
	}
	return NULL;        // just use the default
}

////////////////////////////////////////////////////////////////////////////
//	LoadFrame had to be brought local (like GetIconWndClass) otherwise
//	the default class GetIconWndClass was called instead of our own!?

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
	CWnd* pParentWnd, CCreateContext* pContext)
{
	// only do this once
	ASSERT_VALID_IDR(nIDResource);
	ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

	m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

	CString strFullString, strTitle;
	if (strFullString.LoadString(nIDResource))
		AfxExtractSubString(strTitle, strFullString, 0);    // first sub-string

	//	will not append open document name to title
	dwDefaultStyle &= ~(DWORD)FWS_ADDTOTITLE;
	if (!Create(GetIconWndClass(dwDefaultStyle, nIDResource),
	  strTitle, dwDefaultStyle, rectDefault,
	  pParentWnd, MAKEINTRESOURCE(nIDResource), 0L, pContext))
		return FALSE;   // will self destruct on failure normally

	LoadAccelTable(MAKEINTRESOURCE(nIDResource));

	if (pContext == NULL)   // send initial update
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE);

	return TRUE;
}

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
	CFrameWnd::OnPaletteChanged(pFocusWnd);
	
	// TODO: Add your message handler code here
	TRACE("The palette was changed!\n");
	if (pFocusWnd)
	if (pFocusWnd->GetParentFrame() == this) return;
	OnQueryNewPalette();
	//	if ((pFocusWnd != this) && (pFocusWnd != OurView)) OnQueryNewPalette();
}

BOOL CMainFrame::OnQueryNewPalette()
{
	// TODO: Add your message handler code here and/or call default

	if (OurView == NULL) return FALSE;
	if (CPalPtr == NULL) return FALSE;
CClientDC	dc(OurView);
	dc.SelectPalette(CPalPtr, FALSE);
int	nMapped = dc.RealizePalette();
	TRACE("Number of entries in new palette = %d\n",nMapped);
	if (nMapped)
	{
		OurView->InvalidateRect(NULL,TRUE);
		return TRUE;
	}
	return FALSE;

//	return CFrameWnd::OnQueryNewPalette();
}

void CMainFrame::OnPaletteIsChanging(CWnd* pRealizeWnd)
{
	CFrameWnd::OnPaletteIsChanging(pRealizeWnd);
	
	// TODO: Add your message handler code here
}

void CMainFrame::OnSysColorChange()
{
	CFrameWnd::OnSysColorChange();
	
	// TODO: Add your message handler code here
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	// TODO: Add your message handler code here
	fActivateApp = bActive;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

long CMainFrame::OnQueryDragIcon(WPARAM wParam, LPARAM lParam)
{
	return (long)(HCURSOR) NULL;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
RECT	rect;
	GetClientRect(&frmRect);
	fIsIconic = IsIconic();
	if (fIsIconic)
	{
		SetWindowText("WinAdv");
	}
	else
	{
		GetWindowRect(&rect);
		AfxGetApp()->WriteProfileInt("Preferences", "Window_W", rect.right - rect.left);
		AfxGetApp()->WriteProfileInt("Preferences", "Window_H", rect.bottom - rect.top);
	}
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
	
	// TODO: Add your message handler code here
RECT	rect;
	fIsIconic = IsIconic();
	if (!fIsIconic)
	{
		GetWindowRect(&rect);
		AfxGetApp()->WriteProfileInt("Preferences", "Window_X", rect.left);
		AfxGetApp()->WriteProfileInt("Preferences", "Window_Y", rect.top);
	}
}

void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (fIsIconic) PaintIcon();
	// Do not call CFrameWnd::OnPaint() for painting messages
}
