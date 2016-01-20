#include "stdafx.h"
#include "Resource.h"
#include "BmpButton.h"
#include "Image.h"

#define TIMER_FIRST 1
#define TIMER_REPEAT 2

//////////////////////////////////////////////////////////////////////
CBmpButton::CBmpButton()
{
	for (int i = 0; i < MAX_IMAGECOUNT; i++)
		m_nImage[i] = -1;

	m_dwExtendedStyle = BMPBTN_AUTOSIZE | BMPBTN_HOVER;
	m_lpstrToolTipText = NULL;
	m_bHover = false;
	m_bOn = false;
	m_bCapture = false;
	m_bTransparentDraw = false;
}

//////////////////////////////////////////////////////////////////////
CBmpButton::~CBmpButton()
{
	m_ImageList.Destroy();

	if (m_lpstrToolTipText)
		delete [] m_lpstrToolTipText;
}

//////////////////////////////////////////////////////////////////////
void CBmpButton::InitDialog(HWND hWndButtonCtrl, UINT nBitmapID, DWORD dwExtendedStyle, COLORREF clrTransparent, COLORREF clrBackground)
{
	if (hWndButtonCtrl)
		SubclassWindow(hWndButtonCtrl);

	m_bTransparentDraw = (clrTransparent != CLR_NONE) && (clrBackground == CLR_NONE);
	if (m_bTransparentDraw)
		ModifyStyles(m_hWnd, 0/*dwRemove*/, WS_EX_TRANSPARENT/*dwAdd*/, true/*bExtended*/);
	else
		ModifyStyles(m_hWnd, WS_EX_TRANSPARENT/*dwRemove*/, 0/*dwAdd*/, true/*bExtended*/);

	CImage Image(_AtlBaseModule.GetResourceInstance(), nBitmapID, "GIF");
	HBITMAP hBitmap = Image.GetBitmapHandle();
//j	HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nBitmapID));
	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);
	int dx = bm.bmWidth / MAX_IMAGECOUNT;
	int dy = bm.bmHeight;
	m_ImageList.Create(dx, dy, ILC_COLOR24 | ((clrTransparent != CLR_NONE) ? ILC_MASK : 0), MAX_IMAGECOUNT, 1);
	if (clrTransparent != CLR_NONE)
		m_ImageList.Add(hBitmap, clrTransparent);
	else
		m_ImageList.Add(hBitmap);
//j	::DeleteObject(hBitmap);
	m_ImageList.SetBkColor(clrBackground);
	IMAGEINFO info;
	::ZeroMemory(&info, sizeof(info));
	m_ImageList.GetImageInfo(0, &info);

	SetImages(0/*Off*/, 1/*HoverOff*/, 2/*HoverOn*/, 3/*On*/, 4/*Disabled*/);

	m_dwExtendedStyle |= dwExtendedStyle;
	if (m_dwExtendedStyle & BMPBTN_AUTOSIZE)
		SizeToImage();

//j	SetToolTipText(_T("Bold button tip"));
}

//////////////////////////////////////////////////////////////////////
DWORD CBmpButton::GetBitmapButtonExtendedStyle() const
{
	return m_dwExtendedStyle;
}

//////////////////////////////////////////////////////////////////////
DWORD CBmpButton::SetBitmapButtonExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask)
{
	DWORD dwPrevStyle = m_dwExtendedStyle;
	if (!dwMask)
		m_dwExtendedStyle = dwExtendedStyle;
	else
		m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);
	return dwPrevStyle;
}

//////////////////////////////////////////////////////////////////////
HIMAGELIST CBmpButton::GetImageList() const
{
	return m_ImageList;
}

//////////////////////////////////////////////////////////////////////
HIMAGELIST CBmpButton::SetImageList(HIMAGELIST hImageList)
{
	HIMAGELIST hImageListPrev = m_ImageList;
	m_ImageList = hImageList;
	if (m_dwExtendedStyle & BMPBTN_AUTOSIZE)
		SizeToImage();

	return hImageListPrev;
}

//////////////////////////////////////////////////////////////////////
int CBmpButton::GetToolTipTextLength() const
{
	return (!m_lpstrToolTipText ? -1 : lstrlen(m_lpstrToolTipText));
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::GetToolTipText(LPTSTR lpstrText, int nLength) const
{
	if (!lpstrText)
		return false;
	if (!m_lpstrToolTipText)
		return false;
	return (lstrcpyn(lpstrText, m_lpstrToolTipText, min(nLength, lstrlen(m_lpstrToolTipText) + 1)) != NULL);
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::SetToolTipText(LPCTSTR lpstrText)
{
	if (m_lpstrToolTipText)
	{
		delete [] m_lpstrToolTipText;
		m_lpstrToolTipText = NULL;
	}
	if (!lpstrText)
	{
		if (m_tip.IsWindow())
			m_tip.Activate(false);
		return true;
	}
	ATLTRY(m_lpstrToolTipText = new TCHAR[lstrlen(lpstrText) + 1]);
	if (!m_lpstrToolTipText)
		return false;

	bool bRet = (lstrcpy(m_lpstrToolTipText, lpstrText) != NULL);
	if (bRet && m_tip.IsWindow())
	{
		m_tip.Activate(true);
		m_tip.AddTool(m_hWnd, m_lpstrToolTipText);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CBmpButton::SetImages(int nOff, int nHoverOff, int nHoverOn, int nOn, int nDisabled)
{
	if (nOff != -1)
		m_nImage[_nImageOff] = nOff;
	if (nOn != -1)
		m_nImage[_nImageOn] = nOn;
	if (nHoverOff != -1)
		m_nImage[_nImageHoverOff] = nHoverOff;
	if (nHoverOn != -1)
		m_nImage[_nImageHoverOn] = nHoverOn;
	if (nDisabled != -1)
		m_nImage[_nImageDisabled] = nDisabled;
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::SizeToImage()
{
	if (!IsWindow())
		return false;
	if (!m_ImageList.m_hImageList)
		return false;

	int cx = 0;
	int cy = 0;
	if (!m_ImageList.GetIconSize(cx, cy))
		return false;

	return !!ResizeClient(cx, cy);
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::IsHoverMode() const
{
	return (m_dwExtendedStyle & BMPBTN_HOVER);
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::IsChecked()
{
	if (!IsWindow())
		return false;

	if (!(GetButtonStyle() & (BS_CHECKBOX|BS_RADIOBUTTON)))
		return false;

	return (GetCheck() == BST_CHECKED);
}

//////////////////////////////////////////////////////////////////////
void CBmpButton::Check(bool bChecked)
{
	if (!IsWindow())
		return;

	if (!(GetButtonStyle() & (BS_CHECKBOX|BS_RADIOBUTTON)))
		return;

	if (bChecked == IsChecked())
		return;

	SetCheck(bChecked ? BST_CHECKED : BST_UNCHECKED);
}

/////////////////////////////////////////////////////////////////////////////
void CBmpButton::Redraw()
{
	if (!IsWindow())
		return;

	::RedrawWindow(m_hWnd, NULL/*lpRectUpdate*/, NULL/*hRgnUpdate*/, RDW_INVALIDATE /*| RDW_UPDATENOW | RDW_ERASE*/);
}

//////////////////////////////////////////////////////////////////////
void CBmpButton::DoPaint(CDCHandle dc)
{
	if (!m_ImageList.m_hImageList)
		return;

	if (m_nImage[_nImageOff] == -1)
		return; // main bitmap must be set
		
	CRect ClientRect;
	GetClientRect(ClientRect);

	// set bitmap according to the current button state
	int nImage = -1;
	bool bOn = m_bOn || (m_bOn ^ IsChecked());
	if (bOn)
	{
		if (m_bHover)
			nImage = m_nImage[_nImageHoverOn];
		else
			nImage = m_nImage[_nImageOn];
	}
	else
	{
		if (m_bHover)
			nImage = m_nImage[_nImageHoverOff];
		else
			nImage = m_nImage[_nImageOff];
	}

	if (!IsWindowEnabled())
		nImage = m_nImage[_nImageDisabled];
	if (nImage == -1)
		nImage = m_nImage[_nImageOff];

	// draw the button image
	int xyPos = 0;
	if (bOn && (nImage == m_nImage[_nImageOff]))
		xyPos = 1;
	COLORREF rgbBk = (m_bTransparentDraw ? CLR_NONE : CLR_DEFAULT);
	COLORREF rgbFg = CLR_DEFAULT;
	m_ImageList.DrawEx(nImage, dc, xyPos, xyPos, ClientRect.Width(), ClientRect.Height(), rgbBk, rgbFg, /*ILD_SCALE|*/(m_bTransparentDraw ? ILD_TRANSPARENT : 0));
//j	m_ImageList.Draw(dc, nImage, xyPos, xyPos, ILD_TRANSPARENT);

	// draw 3D border if required
	if ((m_dwExtendedStyle & (BMPBTN_AUTO3D_SINGLE | BMPBTN_AUTO3D_DOUBLE)))
	{
		if (bOn)
			dc.DrawEdge(ClientRect, ((m_dwExtendedStyle & BMPBTN_AUTO3D_SINGLE)) ? BDR_SUNKENOUTER : EDGE_SUNKEN, BF_RECT);
		else
		if (!m_bHover)
			dc.DrawEdge(ClientRect, ((m_dwExtendedStyle & BMPBTN_AUTO3D_SINGLE)) ? BDR_RAISEDINNER : EDGE_RAISED, BF_RECT);

		if (::GetFocus() == m_hWnd)
		{
			ClientRect.InflateRect(-2 * ::GetSystemMetrics(SM_CXEDGE), -2 * ::GetSystemMetrics(SM_CYEDGE));
			dc.DrawFocusRect(ClientRect);
		}
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// We need this style to prevent Windows from painting the button
	ModifyStyle(0/*dwRemove*/, BS_OWNERDRAW/*dwAdd*/);

	// create a tool tip
	m_tip.Create(m_hWnd);
	ATLASSERT(m_tip.IsWindow());
	if (m_tip.IsWindow() && m_lpstrToolTipText)
	{
		m_tip.Activate(true);
		m_tip.AddTool(m_hWnd, m_lpstrToolTipText);
	}

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_tip.IsWindow())
	{
		MSG msg = { m_hWnd, uMsg, wParam, lParam };
		m_tip.RelayEvent(&msg);
	}

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam)
	{
		DoPaint((HDC)wParam);
	}
	else
	{
		CPaintDC dc(m_hWnd);
		DoPaint(dc.m_hDC);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bCapture)
		return S_OK;
		
	SetCapture();
	m_bCapture = true;

	if (!m_bOn)
	{
		m_bOn = true;
		Redraw();
	}

	if (m_dwExtendedStyle & BMPBTN_AUTOFIRE)
	{
		int nElapse = 250;
		int nDelay = 0;
		if (::SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &nDelay, 0))
			nElapse += nDelay * 250;	// all milli-seconds
		SetTimer(TIMER_FIRST, nElapse);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_DOUBLECLICKED), (LPARAM)m_hWnd);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bCapture)
		return S_OK;
		
	ReleaseCapture();
	m_bCapture = false;

	if (m_bOn)
	{
		m_bOn = false;
		Check(!IsChecked());

		if (::GetFocus() != m_hWnd)
			::SetFocus(m_hWnd);

		::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnSetCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// The checked status is about to change
	Invalidate(false);

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// The enabled status is about to change
	Invalidate(false);

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bCapture)
	{
		POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ClientToScreen(&ptCursor);
		CRect WindowRect;
		GetWindowRect(WindowRect);
		if (m_bOn != !!WindowRect.PtInRect(ptCursor))
		{
			m_bOn = !m_bOn;
			Redraw();
		}
	}
	else
	if (IsHoverMode() && !m_bHover)
	{
		m_bHover = true;
		Redraw();
		StartTrackMouseLeave();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bHover)
	{
		m_bHover = false;
		Redraw();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_SPACE)
	{
		if (IsHoverMode())
			return S_OK;	// ignore if in hover mode
		if (!m_bOn)
		{
			m_bOn = true;
			Redraw();
		}
	}

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_SPACE)
	{
		if (IsHoverMode())
			return S_OK;	// ignore if in hover mode
		if (m_bOn)
		{
			m_bOn = false;
			Redraw();
		}
	}

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CBmpButton::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)	// timer ID
	{
		case TIMER_FIRST:
		{
			KillTimer(TIMER_FIRST);
			if (m_bOn)
			{
				::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
				int nElapse = 250;
				int nRepeat = 40;
				if (::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &nRepeat, 0))
					nElapse = 10000 / (10 * nRepeat + 25);	// milli-seconds, approximated
				SetTimer(TIMER_REPEAT, nElapse);
			}
			break;
		}

		case TIMER_REPEAT:
		{
			if (m_bOn)
				::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			else
			if (!m_bCapture)
				KillTimer(TIMER_REPEAT);
			break;
		}
		
		default:	// not our timer
			break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CBmpButton::StartTrackMouseLeave()
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	return !!_TrackMouseEvent(&tme);
}
