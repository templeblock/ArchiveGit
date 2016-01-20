// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ColorGenie.h"
#include "MainFrame.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

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
	// CG: This line was added by the Palette Support component
		m_pPalette = NULL;


	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Center the window
	CenterWindow();

	// CG: The following line was added by the Splash Screen component.
		CSplashWnd::ShowSplashScreen(this, 5000L);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.cx = 704;
	cs.cy = 576;

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::OnQueryNewPalette()
{
	// CG: This function was added by the Palette Support component


		if (m_pPalette == NULL)
			return FALSE;
		
		// BLOCK
		{
			CClientDC dc(this);
			CPalette* pOldPalette = dc.SelectPalette(m_pPalette,
				GetCurrentMessage()->message == WM_PALETTECHANGED);
			UINT nChanged = dc.RealizePalette();
			dc.SelectPalette(pOldPalette, TRUE);


			if (nChanged == 0)
				return FALSE;
		}
		
		Invalidate();
		
		return TRUE;
}

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
	// CG: This function was added by the Palette Support component


		if (pFocusWnd == this || IsChild(pFocusWnd))
			return;
		
		OnQueryNewPalette();
}

CPalette* CMainFrame::SetPalette(CPalette* pPalette)
{
	// CG: This function was added by the Palette Support component


		// Call this function when the palette changes.  It will
		// realize the palette in the foreground to cause the screen
		// to repaint correctly.  All calls to CDC::SelectPalette in
		// painting code should select palettes in the background.


		CPalette* pOldPalette = m_pPalette;
		m_pPalette = pPalette;
		OnQueryNewPalette();
		return pOldPalette;
}
