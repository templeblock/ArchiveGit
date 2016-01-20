////////////////////////////////////////////////////////////////
// Class that paints the caption are of a window
//	Code from example in MSJ
//
//	Modified by LWu	Feb, 1999

#include "StdAfx.h"
#include "PaintCap.h"
#include "Resource.h"
#include "imgctrl.h"
#include "dcdrawingsurface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_WHITE RGB(255,255,255)
#define COLOR_BLACK RGB(0,0,0)
#define NCOLORSHADES 64		// this many shades in gradient
#define kCtrlInset	2

//////////////////
// CCaptionRect Constructor computes caption rectangle in window coords.
//
CCaptionRect::CCaptionRect()
{
}

CCaptionRect::CCaptionRect(const CWnd& wnd, const int nBdrInc, const int nTitleInc)
{
	CalcCaptionRect(wnd, nBdrInc, nTitleInc);
}

CCaptionRect::CalcCaptionRect(const CWnd& wnd, const int nBdrInc, const int nTitleInc)
{
	left = right = top = bottom = 0;	// make sure values are 0 first

	BOOL fCustThickness = FALSE;

	// Get size of frame around window
	DWORD dwStyle = wnd.GetStyle();
	CSize szFrame = (dwStyle & WS_THICKFRAME) ?
		CSize(GetSystemMetrics(SM_CXSIZEFRAME),
			   GetSystemMetrics(SM_CYSIZEFRAME)) :
		CSize(GetSystemMetrics(SM_CXFIXEDFRAME),
				GetSystemMetrics(SM_CYFIXEDFRAME));

	if ( dwStyle & CAC_THICKFRAME)
	{
		szFrame.cx += nBdrInc / 2;
		szFrame.cy += nBdrInc / 2;

		fCustThickness = TRUE;
	}

	int cxIcon = GetSystemMetrics(SM_CXSIZE); // width of caption icon/button

	/*int frameX = szFrame.cx;
	int frameY = szFrame.cy;*/

	// Compute rectangle
	wnd.GetWindowRect(this);		// window rect in screen coords
	*this -= CPoint(left, top);	// shift origin to (0,0)
	left  += szFrame.cx;				// frame
	right -= szFrame.cx;				// frame
	top   += szFrame.cy;				// top = end of frame
	bottom = top 
				+ GetSystemMetrics(SM_CYCAPTION)  // height of caption
				- ((fCustThickness)? 0: GetSystemMetrics(SM_CYBORDER)) // minus gray shadow border
				+ nTitleInc;

	m_czFrameInset = szFrame;
}

////////////////
// CaptionPainter

IMPLEMENT_DYNAMIC(CCaptionPainter, CWnd);

CCaptionPainter::CCaptionPainter():
	m_pParentDlg			(NULL),
	m_pTitleBkImgCtrl		(NULL),
	m_pCloseBxImgCtrl		(NULL),
	m_pMinBxImgCtrl		(NULL),
	m_pMaxBxImgCtrl		(NULL),
	m_pRestoreBxImgCtrl	(NULL),
	m_pHelpBxImgCtrl		(NULL),
	m_nCurNcBtn				(0),
	m_fNcMouseTracking	(FALSE),
	m_fInButton				(FALSE)
{
	m_uiCustFlags =  kUseCustFont;
	m_nCapFillType = kDefFillCapType;
	
	m_crcCaption.SetRectEmpty();
	m_rcIcon.SetRectEmpty();
	m_rcMinBtn.SetRectEmpty();
	m_rcMaxBtn.SetRectEmpty();
	m_rcCloseBtn.SetRectEmpty();

	Invalidate();
}

CCaptionPainter::~CCaptionPainter()
{
}

//////////////////
//
//
BOOL CCaptionPainter::Install(CDialog* pDlg, 
										int		fontSize, 
										CString	fontName,
										int		nBdrInc,
										int		nTitleInc)
{
	ASSERT_KINDOF(CDialog, pDlg);
	m_pParentDlg = pDlg;

	CreateDefFont();
	SetCustFont(fontSize, fontName);

	m_nBdrInc = nBdrInc;
	m_nTitleInc = nTitleInc;

	return TRUE;
}

void CCaptionPainter::Redraw()
{
	m_fontDefCaption.DeleteObject();	// only delete this one coz it is system related
	Invalidate();
}
//////////////////
// make the default font based on system info
//
void CCaptionPainter::CreateDefFont()
{
	if (m_fontDefCaption.m_hObject)
		m_fontDefCaption.DeleteObject();

	// Get current system caption font, just to get its size
	//
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0));
	m_fontDefCaption.CreateFontIndirect(&ncm.lfCaptionFont);

	int fontHeight = ncm.lfCaptionFont.lfHeight;
	int nbrdWidth = ncm.iBorderWidth;
	int nbtnWidth = ncm.iCaptionWidth;
	int nbtnHeight = ncm.iCaptionHeight;
	int nbtnSmWidth = ncm.iSmCaptionWidth;
	int nbtnSmHeight = ncm.iSmCaptionHeight;
	
}

void CCaptionPainter::SetCustFont(int fontSize, CString fontName)
{
	if (m_fontCustCaption.m_hObject)
		m_fontCustCaption.DeleteObject();

	LOGFONT lf;                        // Used to create the CFont.
	memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
	lf.lfHeight = fontSize;                  
	strcpy(lf.lfFaceName, fontName);    
	m_fontCustCaption.CreateFontIndirect(&lf);    // Create the font.

 }

//////////////////
// Message handler handles caption-related messages
//
BOOL CCaptionPainter::RelayEvent(MSG* pMsg) 
{
	switch (pMsg->message)
	{
		case WM_MOUSEMOVE:
			return OnMouseMoveMsg(pMsg->pt);

		case WM_LBUTTONUP:
			return OnLButtonUp(pMsg->lParam, pMsg->pt);

		case WM_NCPAINT:
			return OnNcPaint(HRGN(pMsg->wParam));
		
		case WM_NCACTIVATE:
			return OnNcActivate(pMsg->wParam);
		
		case WM_SETTEXT:
			return OnSetText((LPCTSTR)pMsg->lParam);
		
		case WM_SYSCOLORCHANGE:
		case WM_SETTINGCHANGE:
			Invalidate();
			//m_pParentDlg->SendMessage(m_nPaintMsg, 0, 0L);
			return FALSE;
	}
	return FALSE;
}



/////////////////
// Handle WM_NCPAINT for main window
//
BOOL CCaptionPainter::OnNcPaint(HRGN hRgn)
{
	ASSERT_VALID(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	m_crcCaption.CalcCaptionRect(wnd, m_nBdrInc, m_nTitleInc);		// caption rectangle in window coords
	CRect rcWin;					// window rect
	wnd.GetWindowRect(&rcWin);	// .. get window rect
	CRect rc = m_crcCaption;
	rc += rcWin.TopLeft();		// convert caption rect to screen coords

	// Don't bother painting if the caption doesn't lie within the region.
	//
	if ((WORD)hRgn > 1 && !::RectInRegion(hRgn, &rc)) 
	{
		Default();					// just do default thing
		return TRUE;						// and quit
	}

	// Exclude caption from update region
	//
	HRGN hRgnCaption = ::CreateRectRgnIndirect(&rc);
	HRGN hRgnNew     = ::CreateRectRgnIndirect(&rc);
	if ((WORD)hRgn > 1) 
	{
		// wParam is a valid region: subtract caption from it
		::CombineRgn(hRgnNew, hRgn, hRgnCaption, RGN_DIFF);
	} else 
	{
		// wParam is not a valid region: create one that's the whole
		// window minus the caption bar
		HRGN hRgnAll = ::CreateRectRgnIndirect(&rcWin);
		CombineRgn(hRgnNew, hRgnAll, hRgnCaption, RGN_DIFF);
		DeleteObject(hRgnAll);
	}

	// Call Windows to do WM_NCPAINT with altered update region
	//
/*	MSG& msg = AfxGetThreadState()->m_lastSentMsg;
	WPARAM savewp = msg.wParam;	// save original wParam
	msg.wParam = (WPARAM)hRgnNew;	// set new region for DefWindowProc
	Default();
	DeleteObject(hRgnCaption);		// clean up
	DeleteObject(hRgnNew);			// ...
	msg.wParam = savewp;				// restore original wParam
*/
	PaintCaption(m_crcCaption);					// Now paint my special caption
	
	return TRUE;
}

//////////////////
// Handle WM_NCACTIVATE for main window
//
BOOL CCaptionPainter::OnNcActivate(BOOL bActive)
{
	ASSERT_VALID(m_pParentDlg);
	CDialog& dlg = *((CDialog*)m_pParentDlg);
	ASSERT_KINDOF(CDialog, &dlg);

/*	// Turn WS_VISIBLE off before calling DefWindowProc,
	// so DefWindowProc won't paint and thereby cause flicker.
	//
	DWORD dwStyle = dlg.GetStyle();
	if (dwStyle & WS_VISIBLE)
		::SetWindowLong(dlg, GWL_STYLE, (dwStyle & ~ WS_VISIBLE));

	MSG& msg = AfxGetThreadState()->m_lastSentMsg;
	msg.wParam = bActive;
	Default();
	if (dwStyle & WS_VISIBLE)
		::SetWindowLong(frame, GWL_STYLE, dwStyle);
*/
	// At this point, nothing has happened (since WS_VISIBLE was off).
	// Now it's time to paint.
	//
	m_bActive = bActive;					// update state
	dlg.SendMessage(WM_NCPAINT);	// paint non-client area (frame too)
	return TRUE;							// done OK
}

//////////////////
// Handle WM_SETTEXT for main window
//
BOOL CCaptionPainter::OnSetText(LPCTSTR lpText)
{
	ASSERT_VALID(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	// Turn WS_VISIBLE style off before calling Windows to
	// set the text, then turn it back on again after.
	//
	DWORD dwStyle = wnd.GetStyle();
	if (dwStyle & WS_VISIBLE)
		SetWindowLong(wnd.m_hWnd, GWL_STYLE, dwStyle & ~ WS_VISIBLE);
	Default();
	if (dwStyle & WS_VISIBLE)
		SetWindowLong(wnd.m_hWnd, GWL_STYLE, dwStyle);

	wnd.SendMessage(WM_NCPAINT);		// paint non-client (frame)
	Invalidate();							// force new bitmap
	PaintCaption(m_crcCaption);
	return TRUE;
}



BOOL CCaptionPainter::OnNcLBtnDown(UINT nHitTest, CPoint point)
{
	UINT nHitVal = OnNcHitTest(point);
	switch (nHitVal)
	{
		case CAC_HTCLOSE:
		case HTMINBUTTON:
		case HTMAXBUTTON:
		{
			m_nCurNcBtn = nHitVal;
			m_fNcMouseTracking = TRUE;
			m_fInButton = TRUE;
			m_pParentDlg->SetCapture();
			// i'm assuming that custom control btns goes along with picture captions
			if (m_nCapFillType == kPictFillCapType)
				DrawCustomNcBtn(m_nCurNcBtn, CPImageControl::DOWN);
			else
				DrawStandardNcBtn(m_nCurNcBtn, CCaptionPainter::DOWN);
			return TRUE;

		}
		
		case HTSYSMENU:
		default:
			return FALSE;
	}
}
BOOL CCaptionPainter::OnNcLBtnDblClk(UINT nHitTest, CPoint point)
{
	ClientToScreen(&point);
	if (OnNcHitTest(point) == HTSYSMENU)	// double clicked on the icon?
	{
		m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_CLOSE, MAKELONG(0,-1));
		return TRUE;
	}
	else
		return FALSE;
}

//////////////////
// figure out where the mouse hit
UINT CCaptionPainter::OnNcHitTest(CPoint point)
{
	// point is in screen coordinates

	// is it even in the window?
	CRect windowRect;
	m_pParentDlg->GetWindowRect(&windowRect);
	if (!windowRect.PtInRect(point))
		return HTNOWHERE;		// missed us

	// is it in the client are?
	CRect rcClient;
	m_pParentDlg->GetClientRect(&rcClient);
	m_pParentDlg->ClientToScreen(&rcClient);
	if (rcClient.PtInRect(point))
		return HTCLIENT;		// HIT!

	// make a local point, can't use ScreenToClient coz it isn't in the client area
	CPoint localPt = point;
	localPt.x -= windowRect.left;
	localPt.y -= windowRect.top;

	CPoint ctrlPt = localPt;

	if (m_crcCaption.PtInRect(localPt))
	{
		if (!m_rcIcon.IsRectEmpty())
		{
			if (m_rcIcon.PtInRect(localPt))
					return HTSYSMENU;
		}

		if (!m_rcMinBtn.IsRectEmpty())
		{
			if (m_rcMinBtn.PtInRect(localPt))
			{
				if (m_pMinBxImgCtrl)
				{
					ctrlPt.x -= m_rcMinBtn.left;
					ctrlPt.y -= m_rcMinBtn.top;
					if (m_pMinBxImgCtrl->InClickableRegion(ctrlPt))
						return HTMINBUTTON;
				}
				else
					return HTMINBUTTON;
			}
		}

		if (!m_rcMaxBtn.IsRectEmpty())
		{
			if (m_rcMaxBtn.PtInRect(localPt))
			{
				CPImageControl *pImgCtrl = (m_pParentDlg->IsZoomed() ? m_pRestoreBxImgCtrl : m_pMaxBxImgCtrl);
				if (pImgCtrl)
				{
					ctrlPt.x -= m_rcMaxBtn.left;
					ctrlPt.y -= m_rcMaxBtn.top;
					if (pImgCtrl->InClickableRegion(ctrlPt))
						return HTMAXBUTTON;
				}
				else
					return HTMAXBUTTON;
			}
		}

		if (!m_rcCloseBtn.IsRectEmpty())
		{	
			if (m_rcCloseBtn.PtInRect(localPt))
			{
				if (m_pCloseBxImgCtrl)
				{
					ctrlPt.x -= m_rcCloseBtn.left;
					ctrlPt.y -= m_rcCloseBtn.top;
					if (m_pCloseBxImgCtrl->InClickableRegion(ctrlPt))
						return CAC_HTCLOSE;
				}
				else
					return CAC_HTCLOSE;
			}
		}

		return HTCAPTION;
	}

	// ok, now is it somewhere around the border area?
	CSize theFrameSize = m_crcCaption.m_czFrameInset;
	DWORD dwStyle = m_pParentDlg->GetStyle();
	if (dwStyle & (WS_THICKFRAME | CAC_THICKFRAME))
	{
		// top portion of the border
		if (point.y < windowRect.top + theFrameSize.cy)
		{
			if (point.x <= windowRect.left + theFrameSize.cx)
				return HTTOPLEFT;
			else if (point.x >= windowRect.right - theFrameSize.cx)
				return HTTOPRIGHT;
			else
				return HTTOP;
		}
		// bottom portion
		else if (point.y >= windowRect.bottom - theFrameSize.cy)
		{
			if (point.x <= windowRect.left + theFrameSize.cx)
				return HTBOTTOMLEFT;
			else if (point.x >= windowRect.right - theFrameSize.cx)
				return HTBOTTOMRIGHT;
			else
				return HTBOTTOM;
		}
		// left side
		else if (point.x < windowRect.left + theFrameSize.cx)
		{
			if (point.y <= windowRect.top + theFrameSize.cy)
				return HTTOPLEFT;
			else if (point.y >= windowRect.bottom - theFrameSize.cy)
				return HTBOTTOMLEFT;
			else
				return HTLEFT;
		}
		// right side
		else if (point.x >= windowRect.right - theFrameSize.cx)
		{
			if (point.y <= windowRect.top + theFrameSize.cy)
				return HTTOPRIGHT;
			else if (point.y >= windowRect.bottom - theFrameSize.cy)
				return HTBOTTOMRIGHT;
			else
				return HTRIGHT;
		}
	}

	return (UINT)HTERROR;
}


BOOL CCaptionPainter::OnMouseMoveMsg(POINT pt)
{
	if (!m_fNcMouseTracking)
		return FALSE;

	if (GetCapture() != m_pParentDlg)
	{
		m_fNcMouseTracking = FALSE;
		m_fInButton = FALSE;
		m_pParentDlg->SendMessage(WM_NCPAINT);
	}
	else
	{
		UINT	nButton = OnNcHitTest(pt);
		
		if (nButton != m_nCurNcBtn && m_fInButton)
		{
			m_fInButton = FALSE;
			// i'm assuming that custom control btns goes along with picture captions
			if (m_nCapFillType == kPictFillCapType)
				DrawCustomNcBtn(m_nCurNcBtn, CPImageControl::UP);
			else
				DrawStandardNcBtn(m_nCurNcBtn, CCaptionPainter::UP);
		}
		else if (nButton == m_nCurNcBtn && !m_fInButton)
		{
			m_fInButton = TRUE;
			// i'm assuming that custom control btns goes along with picture captions
			if (m_nCapFillType == kPictFillCapType)
				DrawCustomNcBtn(m_nCurNcBtn, CPImageControl::DOWN);
			else
				DrawStandardNcBtn(m_nCurNcBtn, CCaptionPainter::DOWN);
		}
	}

	return FALSE;
}

BOOL CCaptionPainter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_fNcMouseTracking)
		return FALSE;

	::ReleaseCapture();
	m_fNcMouseTracking = FALSE;
	m_fInButton = FALSE;


	UINT nHitVal = OnNcHitTest(point);
	if (nHitVal == m_nCurNcBtn)
	{
		if (m_pCloseBxImgCtrl)
			DrawCustomNcBtn(m_nCurNcBtn, CPImageControl::UP);
		else
			DrawStandardNcBtn(m_nCurNcBtn, CCaptionPainter::UP);
		switch (m_nCurNcBtn)
		{
			case HTMINBUTTON:
				if (m_pParentDlg->GetStyle() & WS_MINIMIZE)
					m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_RESTORE, MAKELONG(0,-1));
				else
					m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_ICON, MAKELONG(0,-1));
				return TRUE;

			case HTMAXBUTTON:
				if (m_pParentDlg->GetStyle() & WS_MAXIMIZE)
					m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_RESTORE, MAKELONG(0,-1));
				else
					m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_ZOOM, MAKELONG(0,-1));
				return TRUE;

			case CAC_HTCLOSE:
				m_pParentDlg->SendMessage(WM_SYSCOMMAND, SC_CLOSE, MAKELONG(0,-1));
				return TRUE;
		}
	}
	return FALSE;
}
//////////////////
//
void CCaptionPainter::PaintCaption(CCaptionRect& rc)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	if (wnd.IsIconic())						// this shouldn't be the case for the dlgs but...
		return;

	if (rc.Size() != m_szCaption) 		// if size changed:
	{
		m_crcCaption.CalcCaptionRect(wnd, m_nBdrInc, m_nTitleInc);		// caption rectangle in window coords
		m_szCaption = m_crcCaption.Size();			// update new size
		rc = m_crcCaption;
	}

	// determind what kind of caption to draw
	switch (m_nCapFillType)
	{
		case kDefFillCapType:
			PaintSolidCaption(rc, GetSysColor(COLOR_ACTIVECAPTION));
			break;

		case kClrFillCapType:
			PaintSolidCaption(rc, m_clrCustom);
			break;

		case kGradFillCapType:
			PaintGradiantCaption(rc);
			break;

		case kPictFillCapType:
			PaintPictureCaption(rc);
			break;

		default:
			ASSERT(0);
	}
}

////////////////
// Draw caption icon. Returns width of icon.
//
int CCaptionPainter::DrawIcon(CDC* dc)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	// Within the basic button rectangle, Windows 95 uses a 1 or 2 pixel border
	// Icon has 2 pixel border on left, 1 pixel on top/bottom, 0 right
	//
	int cxIcon = GetSystemMetrics(SM_CXSIZE);
	CRect rc(0, 0, cxIcon, GetSystemMetrics(SM_CYSIZE));
	rc.DeflateRect(0,1);
	rc.left += 2;

	HICON	theIcon = (HICON)GetClassLong(wnd.m_hWnd, GCL_HICONSM);
	if (theIcon)
	{
		m_rcIcon = rc;
		DrawIconEx(dc->m_hDC, rc.left, rc.top,
			theIcon, rc.Width(), rc.Height(), 0, NULL, DI_NORMAL);
		return cxIcon;
	}
	else
		return 0;
}

////////////////
// Draw min, max/restore, close buttons.
// Returns total width of buttons drawn.
//
int CCaptionPainter::DrawButtons(CDC* dc, CSize capSize)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;
	
	DWORD dwStyle = wnd.GetStyle();
	if (!(dwStyle & WS_CAPTION))
		return 0;

	ASSERT(dc);

	// set up the rectangle for the close box
	int btnDim = capSize.cy - (2 * kCtrlInset);

	CRect rc(0, 0, btnDim, btnDim);
	rc.OffsetRect(capSize.cx - (btnDim + kCtrlInset), kCtrlInset);	// move right

	// the offset from the border
	CSize theFrameSize = m_crcCaption.m_czFrameInset;

	dc->DrawFrameControl(&rc, DFC_CAPTION, DFCS_CAPTIONCLOSE);
	m_rcCloseBtn = rc;
	m_rcCloseBtn.OffsetRect(theFrameSize);

	// Max/restore button is like close box; just shift rectangle left
	// Also does help button, if any.
	BOOL bMaxBox = dwStyle & WS_MAXIMIZEBOX;
	if (bMaxBox || (wnd.GetExStyle() & WS_EX_CONTEXTHELP)) 
	{
		rc -= CPoint(btnDim + kCtrlInset, 0);
		dc->DrawFrameControl(&rc, DFC_CAPTION,
			bMaxBox ? (wnd.IsZoomed() ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMAX) :
				DFCS_CAPTIONHELP);
		m_rcMaxBtn = rc;
		m_rcMaxBtn.OffsetRect(theFrameSize);
	}

	// Minimize button has 2 pixel border on all sides but right.
	if (dwStyle & WS_MINIMIZEBOX) 
	{
		rc -= CPoint(btnDim, 0);
		dc->DrawFrameControl(&rc, DFC_CAPTION, DFCS_CAPTIONMIN);
		m_rcMinBtn = rc;
		m_rcMinBtn.OffsetRect(theFrameSize);
	}
	return capSize.cx - rc.left - 2;
}

////////////////
// Draw min, max/restore, close buttons.
// Returns total width of buttons drawn.
//
int CCaptionPainter::DrawCustomButtons(CDC* dc, CRect& capRect)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;
	
	DWORD dwStyle = wnd.GetStyle();
	if (!(dwStyle & WS_CAPTION))
		return 0;

	if (!(dwStyle & WS_SYSMENU)) // does it contain controls in the title bar?
		return 0;

	RDcDrawingSurface destDS;
	destDS.Initialize(dc->m_hDC, dc->m_hAttribDC);

	// do the close box
	int btnDim = capRect.Height() - (2 * kCtrlInset);
	CRect rcSrc(0, 0, btnDim, btnDim);
	
	CRect rcDest(rcSrc);
	rcDest.OffsetRect(capRect.left + capRect.Width() - (btnDim + kCtrlInset) , capRect.top + kCtrlInset);	// move right
	
	m_rcCloseBtn = rcDest;
	
	m_pCloseBxImgCtrl->SetControlSize(rcSrc, m_pTitleBkImgCtrl);
	m_pCloseBxImgCtrl->Draw(CPImageControl::UP, destDS, rcSrc, rcDest);

	// Max/restore button is like close box; just shift rectangle left
	// Also does help button, if any.
	BOOL bMaxBox = dwStyle & WS_MAXIMIZEBOX;
	if (bMaxBox || (wnd.GetExStyle() & WS_EX_CONTEXTHELP)) 
	{
		rcDest.OffsetRect(-(btnDim + kCtrlInset), 0);
		CPImageControl* pImgCtrl;
		if ( bMaxBox != FALSE )
			pImgCtrl = (wnd.IsZoomed() ? m_pRestoreBxImgCtrl : m_pMaxBxImgCtrl);
		else
			ASSERT(0);//pImgCtrl = m_pHelpBxImgCtrl;

		if (pImgCtrl)
		{
			pImgCtrl->SetControlSize(rcSrc, m_pTitleBkImgCtrl);
			pImgCtrl->Draw(CPImageControl::UP, destDS, rcSrc, rcDest);
		}
		m_rcMaxBtn = rcDest;
	}

	// Minimize button has 2 pixel border on all sides but right.
	if (dwStyle & WS_MINIMIZEBOX) 
	{
		rcDest -= CPoint(btnDim, 0);
		m_pMinBxImgCtrl->SetControlSize(rcSrc, m_pTitleBkImgCtrl);
		m_pMinBxImgCtrl->Draw(CPImageControl::UP, destDS, rcSrc, rcDest);
		m_rcMinBtn = rcDest;
	}

	destDS.RestoreDefaults( );
	
	return capRect.Width() - rcDest.left - 2;

}


void CCaptionPainter::DrawWindowText(CDC* dc, CRect& theTextRect)
{
	CString myCaption;
	m_pParentDlg->GetWindowText(myCaption);	// dialog title
	myCaption += " ";								

	dc->SetBkMode(TRANSPARENT);					// draw on top of our shading
	dc->SetTextColor(COLOR_WHITE);				// always white
	
	CFont* pOldFont;
	if ( m_uiCustFlags & kUseCustFont)
	{
		pOldFont = dc->SelectObject(&m_fontCustCaption);
	}
	else
	{
		if (!m_fontDefCaption.m_hObject)		// it gets deleted from SetText call, on sys change
			CreateDefFont();
		
		pOldFont = dc->SelectObject(&m_fontDefCaption);
	}

	dc->DrawText(myCaption, theTextRect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	// Restore DC
	dc->SelectObject(pOldFont);
}

//////////////////
// Helper function to compute the luminosity for an RGB color.
// Measures how bright the color is. I use this so I can draw the caption
// text using the user's chosen color, unless it's too dark. See MSDN for
// definition of luminosity and how to compute it.
//
static int GetLuminosity(COLORREF color)
{
#define HLSMAX 240	// This is what Display Properties uses
#define RGBMAX 255	// max r/g/b value is 255
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);
	int rgbMax = max( max(r,g), b);
	int rgbMin = min( min(r,g), b);
	return (((rgbMax+rgbMin) * HLSMAX) + RGBMAX ) / (2*RGBMAX);
}


//////////////////
// Helper to paint rectangle with a color.
//
static void PaintRect(CDC& dc, int x, int y, int w, int h, COLORREF color)
{
	CBrush brush(color);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.PatBlt(x, y, w, h, PATCOPY);
	dc.SelectObject(pOldBrush);
}

//////////////////
// Paint custom caption.
// This is the function that actually does the shading. It creates a
// bitmap that's used to paint the caption. It looks horrible, but it's
// just a lot of bit-twiddling GDI stuff.
//
void CCaptionPainter::PaintGradiantCaption(CRect& capRect)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	// Get caption DC and rectangle
	CWindowDC dcWin(&wnd);					// window DC
	CDC dcMem;									// memory DC
	dcMem.CreateCompatibleDC(&dcWin);		// ...create it

	CBitmap bm;
	CBitmap* pOldBitmap;
	int cxCap = capRect.Width();
	int cyCap = capRect.Height();

	bm.CreateCompatibleBitmap(&dcWin, cxCap, cyCap); // create one
	pOldBitmap = dcMem.SelectObject(&bm);	// select bitmap into memory DC


	if (!m_bActive) 
	{
		// Inactive caption: don't do shading, just fill w/bg color
		PaintRect(dcMem, 0, 0, cxCap, cyCap, GetSysColor(COLOR_INACTIVECAPTION));

	} 
	else 
	{
		// Active caption: do shading
		//
		COLORREF clrBG = GetSysColor(COLOR_ACTIVECAPTION); // background color
		int r = GetRValue(clrBG);				// red..
		int g = GetGValue(clrBG);				// ..green
		int b = GetBValue(clrBG);				// ..blue color vals
		int x = 5*cxCap/6;						// start 5/6 of the way right
		int w = x;									// width of area to shade
		int xDelta= max(w/NCOLORSHADES,1);	// width of one shade band

		// Paint far right 1/6 of caption the background color
		PaintRect(dcMem, x, 0, cxCap-x, cyCap, clrBG);

		// Compute new color brush for each band from x to x + xDelta.
		// Excel uses a linear algorithm from black to normal, i.e.
		//
		//		color = CaptionColor * r
		//
		// where r is the ratio x/w, which ranges from 0 (x=0, left)
		// to 1 (x=w, right). This results in a mostly black title bar,
		// since we humans don't distinguish dark colors as well as light
		// ones. So instead, I use the formula
		//
		//		color = CaptionColor * [1-(1-r)^2]
		//
		// which still equals black when r=0 and CaptionColor when r=1,
		// but spends more time near CaptionColor. For example, when r=.5,
		// the multiplier is [1-(1-.5)^2] = .75, closer to 1 than .5.
		// I leave the algebra to the reader to verify that the above formula
		// is equivalent to
		//
		//		color = CaptionColor - (CaptionColor*(w-x)*(w-x))/(w*w)
		//
		// The computation looks horrendous, but it's only done once each
		// time the caption changes size; thereafter BitBlt'ed to the screen.
		//
		while (x > xDelta) 						// paint bands right to left
		{
			x -= xDelta;							// next band
			int wmx2 = (w-x)*(w-x);				// w minus x squared
			int w2  = w*w;							// w squared
			PaintRect(dcMem, x, 0, xDelta, cyCap,	
				RGB(r-(r*wmx2)/w2, g-(g*wmx2)/w2, b-(b*wmx2)/w2));
		}

		PaintRect(dcMem,0,0,x,cyCap,COLOR_BLACK);  // whatever's left ==> black
	}

	// draw icon and buttons
	int cxIcon  = DrawIcon( &dcMem );
	int cxButns = DrawButtons( &dcMem, capRect.Size() );

	CRect rc(CPoint(0,0), capRect.Size()); // text rectangle
	rc.left  += cxIcon+2;						// start after icon
	rc.right -= cxButns;							// don't draw past buttons
	// draw text
	DrawWindowText(&dcMem, rc);
	
	// blast bits to screen
	dcWin.BitBlt(capRect.left, capRect.top,
					capRect.Width(),capRect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBitmap); // restore DC
}


void CCaptionPainter::PaintSolidCaption(CRect& capRect, COLORREF theColor)
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	// Get caption DC and rectangle
	CWindowDC dcWin(&wnd);					// window DC
	CDC dcMem;									// memory DC
	dcMem.CreateCompatibleDC(&dcWin);		// ...create it

	CBitmap bm;
	CBitmap* pOldBitmap;

	int cxCap = capRect.Width();
	int cyCap = capRect.Height();

	bm.CreateCompatibleBitmap(&dcWin, cxCap, cyCap); // create one
	pOldBitmap = dcMem.SelectObject(&bm);	// select bitmap into memory DC


	if (!m_bActive) 
	{
		// Inactive caption
		PaintRect(dcMem, 0, 0, cxCap, cyCap, GetSysColor(COLOR_INACTIVECAPTION));
	}
	else
	{
		PaintRect(dcMem, 0, 0, cxCap, cyCap, theColor);
	}

	// draw icon and buttons
	int cxIcon  = DrawIcon( &dcMem );
	int cxButns = DrawButtons( &dcMem, capRect.Size() );

	CRect rc(CPoint(0,0), capRect.Size()); // text rectangle
	rc.left  += cxIcon+2;						// start after icon
	rc.right -= cxButns;							// don't draw past buttons
	// draw text
	DrawWindowText(&dcMem, rc);
	
	// blast bits to screen
	dcWin.BitBlt(capRect.left, capRect.top,
					capRect.Width(),capRect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBitmap); // restore DC
}


void CCaptionPainter::PaintPictureCaption( CRect& capRect )
{
	ASSERT(m_pTitleBkImgCtrl);
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	// Get caption DC and rectangle
	CWindowDC dcWin(&wnd);					// window DC
	
	// scale it to the right size
	CRect srcRect(0,0,capRect.Width(),capRect.Height());
	m_pTitleBkImgCtrl->SetControlSize(srcRect);
	
	// draw it
	int retVal = m_pTitleBkImgCtrl->Draw(CPImageControl::UP, dcWin, srcRect, capRect);

	// take care of those control btns 
	DrawCustomButtons(&dcWin, capRect);

	// draw text
	capRect.left += 2;
	DrawWindowText(&dcWin, capRect);

}

void CCaptionPainter::DrawCustomNcBtn( UINT btnType, int state )
{
	CRect rcSrc, rcDest;
	CPImageControl *curImgCtrl = NULL;

	switch (btnType)
	{
		case CAC_HTCLOSE:
				rcSrc = rcDest = m_rcCloseBtn;
				// make src rectangle zero based
				rcSrc -= CPoint(m_rcCloseBtn.left, m_rcCloseBtn.top);
				curImgCtrl = m_pCloseBxImgCtrl;
				break;

		case HTMINBUTTON:
				rcSrc = rcDest = m_rcMinBtn;
				// make src rectangle zero based
				rcSrc -= CPoint(m_rcMinBtn.left, m_rcMinBtn.top);
				curImgCtrl = m_pMinBxImgCtrl;
				break;

		case HTMAXBUTTON:
				rcSrc = rcDest = m_rcMaxBtn;
				// make src rectangle zero based
				rcSrc -= CPoint(m_rcMaxBtn.left, m_rcMaxBtn.top);
				curImgCtrl = (m_pParentDlg->IsZoomed() ? m_pRestoreBxImgCtrl : m_pMaxBxImgCtrl);
				break;

		case HTSYSMENU:
		default:
			return;
	}
	
	if (curImgCtrl)
	{
		CWindowDC dcWin((CWnd*)m_pParentDlg);					// window DC
		
		//create a drawing surface from my dc
		RDcDrawingSurface destDS;
		destDS.Initialize(dcWin.m_hDC, dcWin.m_hAttribDC);

		curImgCtrl->Draw(state, destDS, rcSrc, rcDest);
		
		destDS.RestoreDefaults();
	}
}

void CCaptionPainter::DrawStandardNcBtn( UINT btnType, int state )
{
	ASSERT(m_pParentDlg);
	CWnd& wnd = (CWnd&)*m_pParentDlg;

	CRect rcDest;
	UINT	uCtrlType;

	switch (btnType)
	{
		case CAC_HTCLOSE:
				rcDest = m_rcCloseBtn;
				uCtrlType = DFCS_CAPTIONCLOSE;
				break;

		case HTMINBUTTON:
				rcDest = m_rcMinBtn;
				uCtrlType = DFCS_CAPTIONMIN;
				break;
		
		case HTMAXBUTTON:
				rcDest = m_rcMaxBtn;
				uCtrlType = (wnd.IsZoomed()? DFCS_CAPTIONRESTORE :DFCS_CAPTIONMAX);
				break;
		
		case HTSYSMENU:
		default:
			return;
	}
	
	UINT	uBtnState = 0;
	if (state == CCaptionPainter::DOWN)
		uBtnState = DFCS_PUSHED;
	
	CWindowDC dcWin((CWnd*)m_pParentDlg);					// window DC
	dcWin.DrawFrameControl(&rcDest, DFC_CAPTION, 
									uCtrlType | uBtnState);

}

void CCaptionPainter::SetTitlebarImgCtrl(CPImageControl*	pImgCtrl)
{
	ASSERT(!m_pTitleBkImgCtrl);	// don't want something there already
	m_pTitleBkImgCtrl = pImgCtrl;
	m_nCapFillType = kPictFillCapType;
}

void CCaptionPainter::SetTBarCtrlImgCtrl(CPImageControl*	pImgCtrl, int nType, CRect& rcBtn, int nCtrlID)
{
	ASSERT(m_pParentDlg);

	switch (nType)
	{
		case CPCompPageCollBuilder::SYSCLOSEBTN:
			ASSERT(!m_pCloseBxImgCtrl);	// don't want something there already
			m_pCloseBxImgCtrl = pImgCtrl;
			break;

		case CPCompPageCollBuilder::SYSMINBTN:
			ASSERT(!m_pMinBxImgCtrl);	// don't want something there already
			m_pMinBxImgCtrl = pImgCtrl;
			break;

		case CPCompPageCollBuilder::SYSMAXBTN:
			ASSERT(!m_pMaxBxImgCtrl);	// don't want something there already
			m_pMaxBxImgCtrl = pImgCtrl;
			break;

		case CPCompPageCollBuilder::SYSRESTOREBTN:
			ASSERT(!m_pRestoreBxImgCtrl);	// don't want something there already
			m_pRestoreBxImgCtrl = pImgCtrl;
			break;

		default:
			ASSERT(0);

	}
}