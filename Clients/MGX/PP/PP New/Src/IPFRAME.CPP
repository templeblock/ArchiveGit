/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                ipframe.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "afxpriv.h" // WM_SIZECHILD user-define message of resize bar
#include "ppafx.h"
#include "id.h"
#include "ctxhelp.h"
#include "ipframe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

//---------------------------------------------------------------------------
// CRibbonDialog Constants
//---------------------------------------------------------------------------

#define TITLE_HEIGHT	    9
#define BORDER_WIDTH	    1
#define SYS_MENU_WIDTH	    12
#define SYS_COOKIE_OFFSET	3
#define SYS_COOKIE_WIDTH	7
#define SYS_COOKIE_HEIGHT	4
#define COLOR_SYSMENU		RGB(192, 192, 192)
#define COLOR_SYSSHADOW		RGB(128, 128, 128)
#define COLOR_COOKIEFILL	RGB(255, 255, 255)
#define COLOR_BORDER	    RGB(0,   0,   0)

/* ********************************** Macros ******************************** */

IMPLEMENT_DYNCREATE(CInPlaceFrame, COleIPFrameWnd)
BEGIN_MESSAGE_MAP(CInPlaceFrame, COleIPFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()

//UNDER CONSTRUCTION
//	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()

	ON_MESSAGE(WM_SIZECHILD,OnResizeChild)

	// Palette Messages
	ON_COMMAND(IDC_STATUSPALETTE, OnPalette)
	ON_UPDATE_COMMAND_UI(IDC_STATUSPALETTE, OnUpdatePalette)

	// PasteFromFile Messages
	ON_COMMAND(IDM_PASTEFROMFILE, OnPasteFromFile)
 	ON_UPDATE_COMMAND_UI(IDM_PASTEFROMFILE, OnUpdatePasteFromFile)
	ON_MESSAGE(WM_PASTEFROMFILECLOSED, OnPasteFromFileClosed)

END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CRibbonDialog, CDialog)
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
    ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,		// cap lock indicator
	ID_INDICATOR_NUM,		// num lock indicator
	ID_INDICATOR_SCRL,		// scroll lock indicator
};


/* ******************************* Public Data ****************************** */
/* *************************** Private Functions **************************** */

LOCAL BOOL size_image(LPIMAGE lpImage, LFIXED Width , LFIXED Height, 
	int Resolution, BOOL bSmartSize)
{
	int pix, lin, res;
	LPFRAME lpDataFrame;
	LPFRAME lpAlphaFrame;
	LPOBJECT lpBase;
	LPALPHA lpAlpha;
	RECT rAlpha;
	
	if (!lpImage)
		return(FALSE);
	lpBase = ImgGetBase(lpImage);
	lpAlpha = lpBase->lpAlpha;
	res = Resolution;

	ProgressBegin(1, 0);
	pix = FMUL( res, Width );
	lin = FMUL( res, Height );
	lpAlphaFrame = NULL;
	if (lpDataFrame = SizeFrame(lpBase->Pixmap.EditFrame, pix, lin, res,
								bSmartSize))
	{
		if (lpAlpha)
			lpAlphaFrame = SizeFrame(lpAlpha->Pixmap.EditFrame, pix, lin,
									res, bSmartSize);
	}
	
	if ( !lpDataFrame || (lpAlpha && !lpAlphaFrame))
	{
		if (lpDataFrame)
			FrameClose(lpDataFrame);
		if (lpAlphaFrame)
			FrameClose(lpAlphaFrame);
		ProgressEnd();
		return( FALSE );
	}
	
	lpBase->rObject.right = FrameXSize(lpDataFrame)-1;
	lpBase->rObject.bottom = FrameYSize(lpDataFrame)-1;
	if (lpAlpha)
		MaskRectUpdate(lpAlpha, &rAlpha);
	ProgressEnd();
	
	return( TRUE );
}

/* *************************** Private Methods ****************************** */
/* ***************************** Public Methods ***************************** */

CInPlaceFrame::CInPlaceFrame()
{
	m_hCurrRibbon = NULL;	
	m_idCurrRibbon = -1;
	m_pToolBox = NULL;
	m_bUseOwnStatusBar = FALSE;
	m_bNeedCacheUpdate = FALSE;
	m_bNeedSetInitToolbarPos = FALSE;
	m_hPasteFromFileDlg = NULL;
	m_hPaletteDlg = NULL;
}

CInPlaceFrame::~CInPlaceFrame()
{
	if (m_hCurrRibbon != NULL) // destroy old ribbon

		::DestroyWindow(m_hCurrRibbon);		
	if (m_pToolBox && PictPubApp.GetCustomBoxes())
		PictPubApp.GetCustomBoxes()->Remove (m_pToolBox);

	if (m_wndRibbonBar.GetSafeHwnd())
		m_wndRibbonBar.DestroyWindow();

	if (m_wndFloatRibbonBar.GetSafeHwnd())
		m_wndFloatRibbonBar.DestroyWindow();

	if (m_hPaletteDlg)
	{
		AstralDlgDestroy(m_hPaletteDlg);
		m_hPaletteDlg = NULL;
	}

	DestroyPasteFromFileDlg ();
}

BOOL CInPlaceFrame::CanGetBorderSpace(
    LPCRECT lpRectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, BOOL bFrame)
{
    ASSERT_VALID(this);
    ASSERT(lpRectBorder == NULL ||
        AfxIsValidAddress(lpRectBorder, sizeof(RECT), FALSE));
    ASSERT(lpUIWindow != NULL);

    // use IOleInPlaceUIWindow::GetBorder if no border given
    CRect rectBorder;
    if (lpRectBorder != NULL)
        rectBorder.CopyRect(lpRectBorder);
    else
        lpUIWindow->GetBorder(&rectBorder);

    // get CWnd* for the OLE window
    HWND hwnd;
    VERIFY(lpUIWindow->GetWindow(&hwnd) == NOERROR);
    CWnd* pWnd = CWnd::FromHandle(hwnd);
    ASSERT(pWnd != NULL);
  
    // see how much space we need by calling reposition bars
    CRect rectNeeded = rectBorder;
    pWnd->RepositionBars(0, 0xFFFF, 0, CWnd::reposQuery, &rectNeeded, &rectBorder);
    CRect rectRequest;

    // request the border space from the container
    rectRequest.SetRect(
        rectNeeded.left - rectBorder.left,
        rectNeeded.top - rectBorder.top,
        rectBorder.right - rectNeeded.right,
        rectBorder.bottom - rectNeeded.bottom);
    CRect rectTemp;
    rectTemp = rectRequest;

    // if no border space, just call SetBorderSpace
    if (rectRequest.IsRectNull())
        goto RequestFailed;

    if (lpUIWindow->RequestBorderSpace(&rectTemp) != NOERROR)
        goto RequestFailed;

    return TRUE;

RequestFailed:

	return FALSE;
}

void CInPlaceFrame::DestroyPasteFromFileDlg ()
{
	if (m_hPasteFromFileDlg)
	{
		AstralDlgDestroy(m_hPasteFromFileDlg);
		m_hPasteFromFileDlg = NULL;	   
	}
}

BOOL CInPlaceFrame::InstallRibbon(UINT idRibbon, DLGPROC lpRibbonProc, BOOL bShow)
{
	ASSERT(m_wndRibbonBar.GetSafeHwnd() || m_wndFloatRibbonBar.GetSafeHwnd());
	ASSERT(lpRibbonProc);

	if (!lpRibbonProc ||
		(!m_wndRibbonBar.GetSafeHwnd() && !m_wndFloatRibbonBar.GetSafeHwnd()))
		return FALSE;

	if (m_idCurrRibbon != idRibbon)
	{
		if (m_hCurrRibbon) // destroy old ribbon
		{
			::DestroyWindow(m_hCurrRibbon);		
			m_hCurrRibbon = NULL;
		}

		HWND hParentWnd = (m_wndRibbonBar.GetSafeHwnd()) ? 
			m_wndRibbonBar.GetSafeHwnd() : m_wndFloatRibbonBar.GetSafeHwnd();

		m_hCurrRibbon = CreateDialogParam( PictPubApp.GetResourceHandle(), 
			MAKEINTRESOURCE(idRibbon), hParentWnd, lpRibbonProc, 0);

		m_idCurrRibbon = idRibbon;
	}

	if (!m_hCurrRibbon)
	{
		ASSERT(m_hCurrRibbon);
		return FALSE;
	}

	// adjust the size of the toolbar
	CWnd  *pRibbon = CWnd::FromHandle(m_hCurrRibbon);
	CRect rect;
	if (m_wndRibbonBar.GetSafeHwnd())
		m_wndRibbonBar.GetWindowRect(&rect);
	else
	{
		m_wndFloatRibbonBar.GetWindowRect(&rect);
		m_wndFloatRibbonBar.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
	// -1 on Height resize allows for beveling on ribbon
	pRibbon->MoveWindow(0,0,rect.Width(),rect.Height()-1,FALSE);
	pRibbon->ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	if (idRibbon == IDD_CUSTOMVIEW)
	{
		if (::IsDlgButtonChecked(m_hCurrRibbon,IDC_ZOOMWINDOW))
		{
			FORWARD_WM_COMMAND(m_hCurrRibbon,IDC_ZOOMWINDOW, 
				::GetDlgItem(m_hCurrRibbon,IDC_ZOOMWINDOW) , BN_CLICKED, 
				::SendMessage);
			::CheckDlgButton(m_hCurrRibbon,IDC_ZOOMWINDOW,FALSE);
		}
		::EnableWindow(::GetDlgItem(m_hCurrRibbon,IDC_ZOOMWINDOW),FALSE);
	}

	Tool.hRibbon = m_hCurrRibbon; // 08/15/94 Jake and Joe fix for global ribbon state

	return TRUE;
}

BOOL CInPlaceFrame::InstallToolBox(
BOOL bShow)
{
	BOOL bResult = TRUE;

	if (m_pToolBox)
	{
		if (m_bNeedSetInitToolbarPos)
		{
			CRect rRibbon(0,0,10,10);
			// Set initial position 10 pels below the ribbon 
			if (m_wndRibbonBar.GetSafeHwnd())
				m_wndRibbonBar.GetWindowRect(&rRibbon);
			else
			if (m_wndFloatRibbonBar.GetSafeHwnd())
				m_wndFloatRibbonBar.GetWindowRect(&rRibbon);
	
			m_pToolBox->SetWindowPos(NULL, 0, rRibbon.Height() + 10, 0, 0, 
				SWP_SHOWWINDOW | SWP_NOSIZE | SWP_DRAWFRAME);
	
			m_bNeedSetInitToolbarPos = FALSE;
		}
		m_pToolBox->ShowWindow(bShow ? SW_SHOW : SW_HIDE); 
	}
	else
		bResult = FALSE;

	return bResult;
}

afx_msg int CInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndResizeBar.Create(this))
	{
		TRACE("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	return 0;
}

afx_msg void CInPlaceFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (pFocusWnd)
	{
		if (IsOurWindow(pFocusWnd->GetSafeHwnd()))
		{
			FORWARD_WM_PALETTECHANGED(GetSafeHwnd(), 
				pFocusWnd->GetSafeHwnd(),SendMessageToChildren);
		}
	}

/****************************************
UNDER CONSTRUCTION .............
	CServerDoc *pDoc = (CServerDoc*)GetActiveDocument();
	ASSERT(pDoc);
	if (pDoc)
	{
		CServerView *pView = pDoc->GetActiveView();
		ASSERT(pView);
		if (pView)
		{
			if (pFocusWnd != pView)
			{
				if (lpBltScreen->hPal)
				{
					CDC *pDC = pView->GetDC();
					pDC->SelectPalette(CPalette::FromHandle(lpBltScreen->hPal), TRUE);
					pView->ReleaseDC(pDC);
				}
			}
			else
			{
				FORWARD_WM_PALETTECHANGED(GetSafeHwnd(), 
					pFocusWnd->GetSafeHwnd(),SendMessageToChildren);
			}
		}
	}
****************************************/
}

/****************************************
UNDER CONSTRUCTION .............
afx_msg BOOL CInPlaceFrame::OnQueryNewPalette()
{
	CServerDoc *pDoc = (CServerDoc*)GetActiveDocument();
	ASSERT(pDoc);
	if (pDoc)
	{
		CServerView *pView = pDoc->GetActiveView();
		ASSERT(pView);
		if (pView && lpBltScreen->hPal)
		{
			CDC *pDC = pView->GetDC();
			pDC->SelectPalette(CPalette::FromHandle(lpBltScreen->hPal), FALSE);
			pView->ReleaseDC(pDC);
			return 1; // handled
		}
	}
	return 0; // not handled
}
****************************************/

afx_msg LRESULT CInPlaceFrame::OnResizeChild(WPARAM wParam, LPARAM lParam)
{
	CRect rSize((LPRECT)lParam);
	CServerDoc *pDoc = (CServerDoc*)GetActiveDocument();

	// change our extents
	pDoc->m_sizeDoc.cx = MulDiv(pDoc->m_ZoomDenom.cx, rSize.Width(),
		pDoc->m_ZoomNum.cx);
	pDoc->m_sizeDoc.cy = MulDiv(pDoc->m_ZoomDenom.cy, rSize.Height(),
		pDoc->m_ZoomNum.cy);

	// Notify the container that we have changed
	pDoc->NotifyChanged();
	return COleIPFrameWnd::OnResizeChild(wParam,lParam);
}

afx_msg BOOL CInPlaceFrame::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg)
{

	// turn off hints if they are on
    if( Hints.fBubbleHintsOn && HelpContext.BubbleHintsOn( HL_BUBBLE_QUERY ))     
	{
	    HelpContext.OnHints( pWnd->GetSafeHwnd(), 0 );
	    HintLine( 0 );
	}

	return COleIPFrameWnd::OnSetCursor(pWnd, nHitTest, msg);
}

BOOL CInPlaceFrame::OnCreateControlBars(
// OnCreateControlBars is called by the framework to create control
// bars on the container application's windows.
CWnd* pWndFrame, 
CWnd* pWndDoc)
{
	// ------------------------------------------------
	// create new ribbonbar
	// ------------------------------------------------
	if (!m_wndRibbonBar.Create(pWndFrame,MAKEINTRESOURCE(IDD_INPLACE_BLANKTOOLBOX),
		CBRS_TOP,IDD_INPLACE_BLANKTOOLBOX))
	{
		TRACE("Failed to install new ribbonbar\n");
		return FALSE;
	}

	if (!CanGetBorderSpace(NULL, m_lpFrame, TRUE))
	{
		// Kill the fixed ribbon we couldn't get some space
		m_wndRibbonBar.DestroyWindow();

		// create a Floating Ribbon
		if (!m_wndFloatRibbonBar.Create(IDD_INPLACE_BLANKTOOLBOXFLOAT, 
			pWndFrame)) 
		{
			TRACE("Failed to install new ribbonbar\n");
			return FALSE;
		}

		// Force the window to top and show. Don't know why this works but don't
		// touch!
		m_wndFloatRibbonBar.SetWindowPos(NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
		m_wndFloatRibbonBar.SetOwner(this);
	}
	else
		m_wndRibbonBar.SetOwner(this);

	// ------------------------------------------------
	// create the Toolbox
	// ------------------------------------------------
/* Nezar commented for now
	CCustToolList *pCustToolList = PictPubApp.GetCustomBoxes();
	if (!pCustToolList || 
		!(m_pToolBox = pCustToolList->CreateInsitueBox(pWndFrame)))
	{
		TRACE("Failed to install new toolbox\n");
		return FALSE;
	}
	else
	{
		m_bNeedSetInitToolbarPos = TRUE;
		m_pToolBox->SetOwner(this); 
	}

*/

//-----------------------------------------------------------------------------
// This is good code for creating our own Status bar.  If we can determine
// if the Client has NO status bar then we will want to create our own, else
// "by convention" we are to use its' bar.
//-----------------------------------------------------------------------------
//	// create new statusbar
//	if (m_lpFrame)
//		m_bUseOwnStatusBar = 
//			(NOERROR != m_lpFrame->SetStatusText("Picture Publisher Ready"));
//
//	if (m_bUseOwnStatusBar)
//	{
//		if (!m_wndStatusBar.Create(pWndFrame) || 
//			!m_wndStatusBar.SetIndicators(indicators,
//				sizeof(indicators)/sizeof(UINT)))
//		{
//			TRACE("Failed to install new statusbar\n");
//			return FALSE;
//		}
//		m_wndStatusBar.SetOwner(this);
//	}
	
	CServerDoc *pDoc = (CServerDoc*)GetActiveDocument();
	ASSERT(pDoc);
	if (pDoc)
	{
		CServerView *pView = pDoc->GetActiveView();
		ASSERT(pView);
		if (pView)
		{
			CRect rPosRect;
	
			pDoc->InPlaceInitialUpdate(pWndFrame);
			pDoc->GetItemPosition((LPRECT)&rPosRect);
			CRect rDispRect((LPRECT)&pView->m_lpDisplay->DispRect);
			if ((rPosRect.Width() != rDispRect.Width()) ||
				(rPosRect.Height() != rDispRect.Height()))
				SetUpdateCacheFlag(TRUE);
			else
				SetUpdateCacheFlag(FALSE);
		}
	}

	return TRUE;
}

void CInPlaceFrame::SetHintString(LPCSTR lpHintStr)
{
	if (m_bUseOwnStatusBar)
		m_wndStatusBar.SetPaneText(0, lpHintStr, TRUE);
	else if (m_lpFrame)
		m_lpFrame->SetStatusText(lpHintStr);
}

BOOL CInPlaceFrame::ShowHidePalette(int iShowPalette)
{
	HMENU hMenu = GetIPMenu();
	BOOL bStatus = FALSE;

	if (iShowPalette != -1)
	{
		if (hMenu && m_hPaletteDlg)
		{
			int iShow = (iShowPalette!=FALSE) ? SW_NORMAL : SW_HIDE;
			BOOL bVisible = (BOOL)iShowPalette;
			::ShowWindow( m_hPaletteDlg, iShow );
			if (hMenu)
				ShowHideMenu( IDC_STATUSPALETTE, !bVisible, hMenu);
		}
	}
	
	if (m_hPaletteDlg && ::IsWindowVisible( m_hPaletteDlg ))
		bStatus = TRUE;

	return bStatus;
}

afx_msg void CInPlaceFrame::OnPalette()
{
	BOOL bCreate = FALSE;

	if (m_hPaletteDlg == NULL)
	{
		if (m_hPaletteDlg = AstralDlg(DLG_MODELESS_MULTIPLE, 
			PictPubApp.GetResourceHandle(), this->GetSafeHwnd(), 
			IDD_PALETTE, DlgPaletteProc))
		{
			bCreate = TRUE;
			ShowHidePalette(TRUE);
			return;
		}
	}

	if (m_hPaletteDlg && !bCreate)
	{
    	if ( ::IsWindowVisible( m_hPaletteDlg ) )
			ShowHidePalette(FALSE);
		else
			ShowHidePalette(TRUE);
	}
}

afx_msg void CInPlaceFrame::OnUpdatePalette(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( TRUE );
}

afx_msg void CInPlaceFrame::OnPasteFromFile()
{
	if (m_hPasteFromFileDlg == NULL)
    	m_hPasteFromFileDlg = AstralDlgParam( DLG_MODELESS_MULTIPLE, 
			PictPubApp.GetResourceHandle(), 
			PictPubApp.Get_hWndAstral(),
			ExtBrowser.PasteViewThumbnails ? 
				IDD_PASTEFROMFILE_THUMB:IDD_PASTEFROMFILE,
			DlgPasteFromFileProc,
			(LPARAM)GetSafeHwnd());
}

LONG CInPlaceFrame::OnPasteFromFileClosed(UINT wParam, LONG lParam)
{
	HWND hWhichOne = (HWND)wParam;
	if (m_hPasteFromFileDlg && m_hPasteFromFileDlg == hWhichOne)
		m_hPasteFromFileDlg = NULL;		
	return 1L;
}

afx_msg void CInPlaceFrame::OnUpdatePasteFromFile(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame diagnostics

#ifdef _DEBUG
void CInPlaceFrame::AssertValid() const
{
	COleIPFrameWnd::AssertValid();
}

void CInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleIPFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame commands

void CInPlaceFrame::OnUpdateContextHelp(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!!m_bHelpMode);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CRibbonDialog Methods
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CRibbonDialog::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
	if (bCalcValidRects)
	{
		lpncsp->rgrc[0].top    += TITLE_HEIGHT;
		lpncsp->rgrc[0].left   += BORDER_WIDTH;
		lpncsp->rgrc[0].right  -= BORDER_WIDTH;
		lpncsp->rgrc[0].bottom -= BORDER_WIDTH;
	}
	else
    	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
}

UINT CRibbonDialog::OnNcHitTest(CPoint pt)
{
	// Always return CAPTION will prevent the system cookie from ever closing
	// dialog
	return(HTCAPTION);
}

void CRibbonDialog::OnNcLButtonDown(UINT nHitTest, CPoint pt)
{
	if (nHitTest == HTSYSMENU)
	{
		return; // do nothing
	}
	else 
		CDialog::OnNcLButtonDown(nHitTest, pt);
}

BOOL CRibbonDialog::OnNcActivate(BOOL f)
{
   OnNcPaint();
   return(TRUE);
}

void CRibbonDialog::OnNcPaint()
{
	CRect       r, rTitle, rMenu, rCookie;

	GetWindowRect(r);
	r.OffsetRect(-r.left, -r.top);
	CalcTitleRect(TRUE, rTitle);
	CalcSysMenuRect(TRUE, rMenu);
	rCookie.SetRect(
		rMenu.left + SYS_COOKIE_OFFSET + 1,
		rMenu.top  + SYS_COOKIE_OFFSET + 1,
		rMenu.left + SYS_COOKIE_OFFSET + SYS_COOKIE_WIDTH,
		rMenu.top  + SYS_COOKIE_OFFSET + SYS_COOKIE_HEIGHT);

	// Draw the title bar, sys menu cookie, and border	
	CDC *pDC = GetWindowDC();
	if (!pDC)
    	return;
	
	CBrush TitleBrush(GetSysColor(COLOR_ACTIVECAPTION));
	CBrush FrameBrush(COLOR_BORDER);
	CBrush MenuBrush(COLOR_SYSMENU);
	CBrush CkBrush(COLOR_COOKIEFILL);
	CBrush CkBrushShadow(COLOR_SYSSHADOW);
	
	pDC->FillRect(rTitle, &TitleBrush);
	pDC->FillRect(rMenu,  &MenuBrush);
	
	pDC->FrameRect(rTitle,  &FrameBrush);
	pDC->FrameRect(r,       &FrameBrush);
	pDC->FrameRect(rMenu,   &FrameBrush);
	pDC->FrameRect(rCookie, &CkBrushShadow);
	rCookie.OffsetRect(-1, -1);
	pDC->FillRect(rCookie,  &CkBrush);
	pDC->FrameRect(rCookie, &FrameBrush);

	CFont fnt;
	int FntHeight = 12;
	if (fnt.CreateFont(FntHeight,0,0,0,FW_NORMAL,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE|TMPF_TRUETYPE,NULL))
	{
		CFont   *pfont = pDC->SelectObject(&fnt);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
		pDC->DrawText((LPCSTR)"Picture Publisher",-1,&rTitle,DT_CENTER|DT_VCENTER);
		pDC->SelectObject(pfont);
		fnt.DeleteObject();
	}
	
	ReleaseDC(pDC);
}

void CRibbonDialog::CalcSysMenuRect(BOOL bClient, CRect &rMenu)
{
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rMenu = r;
	rMenu.bottom = rMenu.top  + TITLE_HEIGHT;
	rMenu.right  = rMenu.left + SYS_MENU_WIDTH;
}

void CRibbonDialog::CalcTitleRect(BOOL bClient, CRect &rTitle)
{
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rTitle = r;
	rTitle.bottom = rTitle.top + TITLE_HEIGHT;
}

//******************************* EXPORTED Functions *****************************/



