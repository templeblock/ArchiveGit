// Jim - A flicker-free resizing dialog class
// Based on http://www.codeproject.com/dialog/RPResizeDlg.asp#xx933860xx

#include "stdafx.h"
#include "ResizeDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CResizeDlg, CDialog)
BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CItemCtrl::CItemCtrl()
{
	m_nID = 0;
	m_stxLeft = CST_NONE;
	m_stxRight = CST_NONE;
	m_styTop = CST_NONE;
	m_styBottom = CST_NONE;
	m_bFlickerFree = true;
	m_xRatio = m_cxRatio = 1.0;
	m_yRatio = m_cyRatio = 1.0;
}

/////////////////////////////////////////////////////////////////////////////
CItemCtrl::CItemCtrl(const CItemCtrl& src)
{
	Assign(src);
}

/////////////////////////////////////////////////////////////////////////////
CItemCtrl& CItemCtrl::operator=(const CItemCtrl& src)
{
	Assign(src);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
void CItemCtrl::Assign(const CItemCtrl& src)
{
	m_nID = src.m_nID;
	m_stxLeft = src.m_stxLeft;
	m_stxRight = src.m_stxRight;
	m_styTop = src.m_styTop;
	m_styBottom = src.m_styBottom;
	m_bFlickerFree = src.m_bFlickerFree;
	m_bInvalidate = src.m_bInvalidate;
	m_wRect = src.m_wRect;
	m_xRatio = src.m_xRatio;
	m_cxRatio = src.m_cxRatio;
	m_yRatio = src.m_yRatio;
	m_cyRatio = src.m_cyRatio;
}

/////////////////////////////////////////////////////////////////////////////
bool CItemCtrl::xUpdate(int st, const CRect& curRect, CRect& ctrlRect, CRect* pnRect, CRect* poRect, CRect* pR0)
{
	int deltaX  = pnRect->Width()  - poRect->Width();
	int deltaX0 = pnRect->Width()  - pR0->Width();

	bool bxUpdate = true;
	int newCx;
	switch(st)
	{
		case CST_NONE:
			if (m_stxLeft == CST_ZOOM || m_stxRight == CST_ZOOM ||
				m_stxLeft == CST_DELTA_ZOOM || m_stxRight == CST_DELTA_ZOOM)
			{
				ctrlRect.left = curRect.left;
				ctrlRect.right = curRect.right;

				bxUpdate = false;
			}
			break;

		case CST_RESIZE:
			ctrlRect.right += deltaX;
			break;

		case CST_REPOS:
			ctrlRect.left += deltaX;
			ctrlRect.right += deltaX;
			break;

		case CST_RELATIVE:
			newCx = ctrlRect.Width();
			ctrlRect.left = (int)((double)m_xRatio * 1.0 * pnRect->Width() - newCx / 2.0);
			ctrlRect.right = ctrlRect.left + newCx; /* (int)((double)m_xRatio * 1.0 * pnRect->Width() + newCx / 2.0); */
			break;

		case CST_ZOOM:
			ctrlRect.left = (int)(1.0 * ctrlRect.left * (double)pnRect->Width() / pR0->Width());
			ctrlRect.right = (int)(1.0 * ctrlRect.right * (double)pnRect->Width() / pR0->Width());
			bxUpdate = false;
			break;

		case CST_DELTA_ZOOM:
			newCx = ctrlRect.Width();
			ctrlRect.right = (int)(ctrlRect.left * 1.0 + deltaX0 * 1.0 * m_xRatio + newCx * 1.0 + deltaX0 * m_cxRatio);
			ctrlRect.left += (int)(deltaX0 * 1.0 * m_xRatio);
			bxUpdate = false;
			break;

		default:
			break;
	}

	return bxUpdate;
}

/////////////////////////////////////////////////////////////////////////////
bool CItemCtrl::yUpdate(int st, const CRect& curRect, CRect& ctrlRect, CRect* pnRect, CRect* poRect, CRect* pR0)
{
	int deltaY  = pnRect->Height() - poRect->Height();
	int deltaY0 = pnRect->Height() - pR0->Height();

	bool byUpdate = true;
	int newCy;
	switch (st)
	{
		case CST_NONE:
			if (m_styTop == CST_ZOOM || m_styBottom == CST_ZOOM ||
				m_styTop == CST_DELTA_ZOOM || m_styBottom == CST_DELTA_ZOOM)
			{
				ctrlRect.top = curRect.top;
				ctrlRect.bottom = curRect.bottom;

				byUpdate = false;
			}

			break;

		case CST_RESIZE:
			ctrlRect.bottom += deltaY;
			break;

		case CST_REPOS:
			ctrlRect.top += deltaY;
			ctrlRect.bottom += deltaY;
			break;

		case CST_RELATIVE:
			newCy = ctrlRect.Width();
			ctrlRect.top = (int)((double)m_yRatio * 1.0 * pnRect->Width() - newCy / 2.0);
			ctrlRect.bottom = ctrlRect.top + newCy; /* (int)((double)m_yRatio * 1.0 * pnRect->Width() + newCy / 2.0); */
			break;

		case CST_ZOOM:
			ctrlRect.top = (int)(1.0 * ctrlRect.top * (double)pnRect->Height() / pR0->Height());
			ctrlRect.bottom = (int)(1.0 * ctrlRect.bottom * (double)pnRect->Height() / pR0->Height());
			byUpdate = false;
			break;

		case CST_DELTA_ZOOM:
			newCy = ctrlRect.Height();
			ctrlRect.bottom = (int)(ctrlRect.top * 1.0 + deltaY0 * 1.0 * m_yRatio + newCy * 1.0 + deltaY0 * m_cyRatio);
			ctrlRect.top += (int)(deltaY0 * 1.0 * m_yRatio);
			byUpdate = false;
			break;

		default:
			break;
	}

	return byUpdate;
}

/////////////////////////////////////////////////////////////////////////////
HDWP CItemCtrl::OnSize(HDWP hDWP, int sizeType, CRect* pnRect, CRect* poRect, CRect* pR0, CWnd* pDlg)
{
	CRect ctrlRect = m_wRect;

	// anyway, get the ctrl's real pos & size
	CWnd* pWnd = pDlg->GetDlgItem(m_nID);
	CRect curRect;
	pWnd->GetWindowRect(&curRect);
	pDlg->ScreenToClient(&curRect);

	// process left-horizontal option
	int st = CST_NONE;
	if ((sizeType & WST_LEFT) && m_stxLeft != CST_NONE)
		st = m_stxLeft;
	else
	if ((sizeType & WST_RIGHT) && m_stxRight != CST_NONE)
		st = m_stxRight;

	bool bxUpdate = xUpdate(st, curRect, ctrlRect, pnRect, poRect, pR0);

	// process vertical option
	st = CST_NONE;
	if ((sizeType & WST_TOP) && (m_styTop != CST_NONE))
		st = m_styTop;
	else
	if ((sizeType & WST_BOTTOM) && (m_styBottom != CST_NONE))
		st = m_styBottom;

	bool byUpdate = yUpdate(st, curRect, ctrlRect, pnRect, poRect, pR0);

	if (bxUpdate)
	{
		curRect.left = m_wRect.left;
		curRect.right = m_wRect.right;
	}

	if (byUpdate)
	{
		curRect.top = m_wRect.top;
		curRect.bottom = m_wRect.bottom;
	}

	if (ctrlRect != curRect)
	{
		if (m_bInvalidate)
			pWnd->Invalidate();

		hDWP = ::DeferWindowPos(hDWP, (HWND)*pWnd, NULL, ctrlRect.left, ctrlRect.top, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);

		#if 1 /* why No effect!!!! */
		if (m_bInvalidate)
		{
			pDlg->InvalidateRect(&curRect, false);
			pDlg->UpdateWindow();
		}
		#endif /* No effect???? */

		if (bxUpdate)
		{
			m_wRect.left = ctrlRect.left;
			m_wRect.right = ctrlRect.right;
		}
		if (byUpdate)
		{
			m_wRect.top = ctrlRect.top;
			m_wRect.bottom = ctrlRect.bottom;
		}
	}

	return hDWP;
}

/////////////////////////////////////////////////////////////////////////////
CResizeDlg::CResizeDlg(const UINT resID, CWnd* pParent)
	 : CDialog(resID, pParent)
{
	m_bShowSizeGrip = false;
	m_nDelaySide = 0;
	m_xMin = 32;
	m_yMin = 32;
	m_xMax = 0;
	m_yMax = 0;
}

/////////////////////////////////////////////////////////////////////////////
CResizeDlg::~CResizeDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CResizeDlg::OnInitDialog() 
{
	BOOL bRet = CDialog::OnInitDialog();

	CRect ClientRect;
	GetClientRect(&ClientRect);
	m_ScreenRectOrig = ClientRect;
	ClientToScreen(&m_ScreenRectOrig);
	m_ScreenRect = m_ScreenRectOrig;

	CRect WindowRect;
	GetWindowRect(&WindowRect);

	if (m_xMin <= 0)
		m_xMin = WindowRect.Width();
	if (m_yMin <= 0)
		m_yMin = WindowRect.Height();

	m_bShowSizeGrip = !!(GetStyle() & WS_THICKFRAME);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	m_nDelaySide = nSide;

	if (m_bShowSizeGrip)
	{
		// Invalidate the area occupied by size grip before resizing
		CRect r;
		GetClientRect(&r);
		int size = GetSystemMetrics(SM_CXVSCROLL);
		r.left = r.right - size;
		r.top = r.bottom - size;
		InvalidateRect(&r, false); // do not erase background now

		// Invalidate the area occupied by size grip after resizing
		r = *lpRect;
		r.left = r.right - size;
		r.top = r.bottom - size;
		InvalidateRect(&r, false);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	int nCount = (int)m_Items.size();
	if (nCount <= 0)
		return;

	// nType values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (nType == SIZE_MINIMIZED || cx < 32 || cy < 32)
		return;

	int sizeType = WST_NONE;		

	if (!m_nDelaySide && (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED))
		sizeType = WST_LEFT|WST_TOP|WST_BOTTOM|WST_RIGHT;

	switch (m_nDelaySide)
	{
		case WMSZ_BOTTOM:
			sizeType = WST_BOTTOM;
			break;
		case WMSZ_BOTTOMLEFT:
			sizeType = WST_BOTTOM|WST_LEFT;
			break;
		case WMSZ_BOTTOMRIGHT:
			sizeType = WST_BOTTOM|WST_RIGHT;
			break;
		case WMSZ_LEFT:
			sizeType = WST_LEFT;
			break;
		case WMSZ_RIGHT:
			sizeType = WST_RIGHT;
			break;
		case WMSZ_TOP:
			sizeType = WST_TOP;
			break;
		case WMSZ_TOPLEFT:
			sizeType = WST_TOP|WST_LEFT;
			break;
		case WMSZ_TOPRIGHT:
			sizeType = WST_TOP|WST_RIGHT;
			break;
		default:
			break;
	}

	CRect ScreenRect;
	GetClientRect(&ScreenRect);
	ClientToScreen(ScreenRect);

	if (sizeType != WST_NONE)
	{
		HDWP hDWP = ::BeginDeferWindowPos(nCount);

		std::vector<CItemCtrl>::iterator it;
		for (it = m_Items.begin(); it != m_Items.end(); it++)
			hDWP = it->OnSize(hDWP, sizeType, &ScreenRect, &m_ScreenRect, &m_ScreenRectOrig, this);

		::EndDeferWindowPos(hDWP);
	}

	m_ScreenRect = ScreenRect;
	m_nDelaySide = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::OnGetMinMaxInfo(MINMAXINFO* pmmi)
{
	if (m_xMin > 0)
		pmmi->ptMinTrackSize.x = m_xMin;
	if (m_yMin > 0)
		pmmi->ptMinTrackSize.y = m_yMin;

	if (m_xMax > 0)
		pmmi->ptMaxTrackSize.x = pmmi->ptMaxSize.x = m_xMax;
	if (m_yMax > 0)
		pmmi->ptMaxTrackSize.y = pmmi->ptMaxSize.y = m_yMax;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CResizeDlg::OnEraseBkgnd(CDC* pDC)
{
	if (!(GetStyle() & WS_CLIPCHILDREN))
	{
		std::vector<CItemCtrl>::const_iterator it;

		for (it = m_Items.begin();it != m_Items.end(); it++)
		{
			if (!it->m_bFlickerFree || !::IsWindowVisible(GetDlgItem(it->m_nID)->GetSafeHwnd()))
				continue;

			CRect Rect = it->m_wRect;
			if (Rect.top <= Rect.bottom && Rect.left <= Rect.right)
				pDC->ExcludeClipRect(&Rect);
		}
	}

	bool bHandled = !!CDialog::OnEraseBkgnd(pDC);

	if (m_bShowSizeGrip)
	{
		// Draw size grip
		CRect r;
		GetClientRect(&r);
		int size = GetSystemMetrics(SM_CXVSCROLL);
		r.left = r.right - size;
		r.top = r.bottom - size;
		pDC->DrawFrameControl(&r, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}

	return bHandled;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CResizeDlg::OnNcHitTest(CPoint point)
{
	if (m_bShowSizeGrip)
	{
		// Return HTBOTTOMRIGHT for size grip area
		CRect r;
		GetClientRect(&r);
		int size = GetSystemMetrics(SM_CXVSCROLL);
		r.left = r.right - size;
		r.top = r.bottom - size;
		ScreenToClient(&point);

		if (r.PtInRect(point))
			return HTBOTTOMRIGHT;
	}

	return CDialog::OnNcHitTest(point);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::AddControl(UINT nID, CST xl, CST xr, CST yt, CST yb, bool bFlickerFree, bool bInvalidate,
							double xRatio, double cxRatio, double yRatio, double cyRatio)
{
	CWnd* pControl = GetDlgItem(nID);
	if (!pControl)
		return;

	CItemCtrl item;
	pControl->GetWindowRect(&item.m_wRect);
	ScreenToClient(&item.m_wRect);

	item.m_nID = nID;
	item.m_stxLeft = xl;
	item.m_stxRight = xr;
	item.m_styTop = yt;
	item.m_styBottom = yb;
	item.m_bFlickerFree = bFlickerFree;
	item.m_bInvalidate = bInvalidate; // Invalidate ctrl's rect(eg. no-automatic update for static when resize+move)
	item.m_xRatio = xRatio;
	item.m_cxRatio = cxRatio;
	item.m_yRatio = yRatio;
	item.m_cyRatio = cyRatio;

	CRect ClientRect;
	GetClientRect(&ClientRect);
	if (xl == CST_RELATIVE || xl == CST_ZOOM || xr == CST_RELATIVE || xr == CST_ZOOM)
		item.m_xRatio = (item.m_wRect.left + item.m_wRect.right) / 2.0 / ClientRect.Width();

	if (yt == CST_RELATIVE || yt == CST_ZOOM || yb == CST_RELATIVE || yb == CST_ZOOM)
		item.m_yRatio = (item.m_wRect.bottom + item.m_wRect.top) / 2.0 / ClientRect.Height();

	int nCount = (int)m_Items.size();
	if (nCount > 0)
	{
		std::vector<CItemCtrl>::iterator it;
		for (it = m_Items.begin(); it != m_Items.end(); it++)
		{
			if (it->m_nID == item.m_nID)
			{
				*it = item;
				return;
			}
		}
	}

	m_Items.push_back(item);
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::SetMinSize(int dx, int dy)
{
	m_xMin = (dx <= 0 ? 0 : max(dx, 32));
	m_yMin = (dy <= 0 ? 0 : max(dy, 32));
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::SetMaxSize(int dx, int dy)
{
	m_xMax = dx;
	m_yMax = dy;
}

/////////////////////////////////////////////////////////////////////////////
void CResizeDlg::ShowSizeGrip(bool bShow)
{
	m_bShowSizeGrip = bShow;
}

/////////////////////////////////////////////////////////////////////////////
int CResizeDlg::UpdateControlRect(UINT nID, CRect* pnr)
{
	int nCount = (int)m_Items.size();
	if (nCount > 0)
	{
		std::vector<CItemCtrl>::iterator it;
		for (it = m_Items.begin(); it != m_Items.end(); it++)
		{
			if (it->m_nID != nID)
				continue;

			if (pnr)
				it->m_wRect = *pnr;
			else
			{
				CWnd* pControl = GetDlgItem(nID);
				if (pControl)
				{
					pControl->GetWindowRect(&it->m_wRect);
					ScreenToClient(&it->m_wRect);
				}
			}

			return 0;
		}
	}

	return -1;
}
