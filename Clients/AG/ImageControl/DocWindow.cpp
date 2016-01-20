#include "stdafx.h"
#include "ImageControl.h"
#include "DocWindow.h"
#include "RegKeys.h"
#include "Version.h"
#include "MessageBox.h"
#include "Utility.h"
#include "FloodFill.h"

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define DEFAULT_BACKGROUND_MESSAGE "Please open any image"
#define DEFAULT_SHADOW_SIZE INCHES(0.05)

// Because this has to be static, it is shared between instances
CDocWindow* CDocWindow::g_pDocWindow;

//////////////////////////////////////////////////////////////////////
CDocWindow::CDocWindow(CImageControl& ImageControl) :
	m_ImageControl(ImageControl),
	m_DocCommands(*this),
	m_Select(this)
{
	m_hKeyboardHook = NULL;
	m_pClientDC = NULL;
	m_PageViewRect.SetRectEmpty();
	m_PageRect.SetRectEmpty();
	m_bHasFocus = false;
	m_fZoomScale = -1.0;
	m_iZoom = 0; // = dtoi(m_fZoomScale - 1.0);
	m_bIgnoreSetCursor = false;
	m_bSpecialsEnabled = false;
	m_bOverrideHidden = false;
	m_strBackgroundMessage = DEFAULT_BACKGROUND_MESSAGE;
	m_ShadowSize.SetSize(0, 0);

	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		DWORD dwLength = MAX_PATH;
		char strValue[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_SPECIALSENABLED, strValue, &dwLength) == ERROR_SUCCESS)
			m_bSpecialsEnabled = !!_ttoi(strValue);
	}
}

//////////////////////////////////////////////////////////////////////
CDocWindow::~CDocWindow()
{
	Free();

	int iSize = m_ImageArray.GetSize();
	for (int i = iSize - 1; i >= 0; i--)
	{
		CImageObject* pObject = m_ImageArray[i];
		bool bOK = !!m_ImageArray.RemoveAt(i);
		delete pObject;
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ::GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);

	m_pClientDC = new CAGClientDC(m_hWnd);
	m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, CDocWindow::ImageControlKeyboardProc, NULL, GetCurrentThreadId());
	g_pDocWindow = this;

	if (m_pClientDC)
	{ // Calculate the shadow size for the first time
		m_ShadowSize.cx = DEFAULT_SHADOW_SIZE;
		m_ShadowSize.cy = DEFAULT_SHADOW_SIZE;

		// Convert the shadow size (it will end up being about 4 pixels)
		m_pClientDC->GetDeviceMatrix().Transform((POINT*)&m_ShadowSize, 1, false);
	}

	EliminateScrollbars(true/*bRedraw*/);

	m_Grid.On(false);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Free();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::Free()
{
	g_pDocWindow = NULL;

	m_Select.Unselect(false/*bUpdateUI*/);

	if (m_hKeyboardHook)
	{
		::UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
	}

	if (m_pClientDC)
	{
		delete m_pClientDC;
		m_pClientDC = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ // Process a window 'hide' message as the logical OnClose()
	bHandled = false;

	bool bBeingHidden = !wParam;
	if (!bBeingHidden)
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	// We handle it, but do nothing here - take care of it when we paint
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ImageControl.OnCtlColor(uMsg, wParam, lParam, bHandled);
}

//////////////////////////////////////////////////////////////////////
static HBRUSH CreateCheckeredBrush(HDC hDC, COLORREF lColor1, COLORREF lColor2)
{
	HDC hBitmapDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, 16, 16);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hBitmapDC, hBitmap);
	::FillRect(hBitmapDC, CRect(0, 0,  8,  8), CAGBrush(lColor1));
	::FillRect(hBitmapDC, CRect(8, 0, 16,  8), CAGBrush(lColor2));
	::FillRect(hBitmapDC, CRect(0, 8,  8, 16), CAGBrush(lColor2));
	::FillRect(hBitmapDC, CRect(8, 8, 16, 16), CAGBrush(lColor1));
	::SelectObject(hBitmapDC, (HGDIOBJ)hOldBitmap);
	HBRUSH hBrush = ::CreatePatternBrush(hBitmap);
	BOOL bOK = ::DeleteDC(hBitmapDC);
	bOK = ::DeleteObject(hBitmap);
	return hBrush;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT PaintStruct;
	HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
	if (!hPaintDC)
		return E_FAIL;

	if (!m_pClientDC || m_ImageArray.GetSize() <= 0)
	{
		// Special case: background black = user's application background color
		COLORREF clrBackground = (!m_ImageControl.m_clrBackColor ? ::GetSysColor(COLOR_3DFACE) : m_ImageControl.m_clrBackColor);
		::FillRect(hPaintDC, &PaintStruct.rcPaint, CAGBrush(clrBackground));

		// Draw the text
		int iOldMode = ::SetBkMode(hPaintDC, TRANSPARENT);
		COLORREF lOldColor = ::SetTextColor(hPaintDC, RGB(0,0,0)); //j ::GetSysColor(COLOR_WINDOWTEXT)

		CRect rect;
		GetClientRect(&rect);
		CString strVersion;
		strVersion.Format(IDS_PROJNAME);
		strVersion += " ";
		strVersion += VER_PRODUCT_VERSION_STR;
		::DrawText(hPaintDC, strVersion, lstrlen(strVersion), &rect, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE|DT_NOPREFIX);

		::DrawText(hPaintDC, m_strBackgroundMessage, lstrlen(m_strBackgroundMessage), &rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);

		::SetBkMode(hPaintDC, iOldMode);
		::SetTextColor(hPaintDC, lOldColor);

		::EndPaint(m_hWnd, &PaintStruct);
		return E_FAIL;
	}

	// If the off-screen is 'dirty', redraw the page into it
	if (m_pClientDC->GetDirty())
	{
		HDC hDC = m_pClientDC->GetHDC();

		// Draw the document background and 'furniture'
		{
			// Before drawing anything, open up the offscreen clipping to include a border pixel and handles
			HRGN hClipRegion = m_pClientDC->SaveClipRegion();
			CRect rClipRect = m_PageViewRect;
			rClipRect.InflateRect(1, 1);
			rClipRect.InflateRect(H_SIZE, H_SIZE); 
			m_pClientDC->SetClipRect(rClipRect, false/*bCombine*/);

			// Draw the page background
			// Special case: background black = user's application background color
			COLORREF clrBackground = (!m_ImageControl.m_clrBackColor ? ::GetSysColor(COLOR_3DFACE) : m_ImageControl.m_clrBackColor);
			::FillRect(hDC, &rClipRect, CAGBrush(clrBackground));

			// Calculate the border rectangle, and draw it
			CRect rPageView = m_PageViewRect;
			rPageView.InflateRect(1, 1); // Add the border pixel
			::FrameRect(hDC, &rPageView, CAGBrush(192,192,192)); // Light gray

			// Draw the drop shadow on the right
			CRect r(
				rPageView.right,
				rPageView.top + m_ShadowSize.cy,
				rPageView.right + m_ShadowSize.cx,
				rPageView.bottom + m_ShadowSize.cy);
			::FillRect(hDC, &r, CAGBrush(LTGRAY_BRUSH, eStock));

			// Draw the drop shadow on the bottom
			r.SetRect(
				rPageView.left + m_ShadowSize.cx,
				rPageView.bottom,
				rPageView.right + m_ShadowSize.cx,
				rPageView.bottom + m_ShadowSize.cy);
			::FillRect(hDC, &r, CAGBrush(LTGRAY_BRUSH, eStock));

			m_pClientDC->RestoreClipRegion(hClipRegion);

			// Draw the checkerboard background in case the image has a transparent color
			HBRUSH hBrush = CreateCheckeredBrush(hDC, clrBackground, RGB(255,255,255));
			::FillRect(hDC, &rClipRect, hBrush);
		}
				
		// Draw all the images
		for (int i = 0; i < m_ImageArray.GetSize(); i++)
			m_ImageArray[i]->Draw(*m_pClientDC, m_bOverrideHidden);

		// See if the selected object wants the event
		m_Select.Paint(hDC);

		// Draw the grid
		CSize PageSize(m_PageRect.Width(), m_PageRect.Height());
		m_Grid.Draw(hDC, m_pClientDC->GetViewToDeviceMatrix(), PageSize);

		m_pClientDC->SetDirty(false);
	}

	// Blt from the offscreen to the screen
	m_pClientDC->Blt(hPaintDC, PaintStruct.rcPaint);

	::EndPaint(m_hWnd, &PaintStruct);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
 	UINT nChar = (UINT)wParam;

	// See if the selected object wants the event
	if (m_Select.KeyChar(nChar))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = (UINT)wParam;

	// See if the selected object wants the event
	if (m_Select.KeyDown(nChar))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = (UINT)wParam;

	// See if the selected object wants the event
	if (m_Select.KeyUp(nChar))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// See if the selected object wants the event
	if (m_Select.DoubleClick(x, y))
		return S_OK;

#ifdef _DEBUG // If you want to have double clicks initiate a transform operation..
	if (m_Select.InNormalMode())
	{
		m_Select.StopTracking(true/*bSuccess*/);
		m_Select.StartTransform();
	}
	else
	if (m_Select.InTransformMode())
	{
		m_Select.StopTracking(true/*bSuccess*/);
		m_Select.StartNormal();
	}
#endif _DEBUG
	
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bTakingFocus = (GetFocus() != m_hWnd);
//j	if (bTakingFocus)
//j		SetFocus(); //j This causes a problem if the ImageControl has WS_TABSTOP set

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	SetCapture();

	CPoint Pt(x, y);
	if (m_pClientDC)
		m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(Pt);

	// See which object was hit
	CImageObject* pObject = NULL;
	for (int i = 0; i < m_ImageArray.GetSize(); i++)
	{
		bool bPtInObject = m_ImageArray[i]->HitTest(Pt, m_bOverrideHidden);
		if (bPtInObject)
			pObject = m_ImageArray[i];
	}

	// See if the selected object wants the event
	if (m_Select.ButtonDown(x, y, pObject))
		return S_OK;

//j Uncomment this is you want missed clicks to unselect the current object
//j	if (!pObject)
//j	{
//j		m_Select.Unselect();
//j		return E_FAIL;
//j	}

	if (IsRecolorSelected() || IsRedEyeSelected())
		OnToolButtonDown(Pt, pObject);

	CPoint ClickPoint(x, y);
	m_Select.Select(pObject, &ClickPoint);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// See if the selected object wants the event
	if (m_Select.MouseMove(x, y))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	ReleaseCapture();

	// See if the selected object wants the event
	if (m_Select.ButtonUp(x, y))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// See if the selected object wants the event
	if (m_Select.RightClick(x, y))
		return S_OK;

	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(SpecialsEnabled() ? IDR_POPUPMENU_EX : IDR_POPUPMENU);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;

	// Make sure the menu items are set correctly
	CImageObject* pObject = GetSelectedObject();
	Popup.EnableMenuItem(IDC_DOC_OPEN,          MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_CLOSE,         MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVE,          MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_TRANSPARENT,   MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOMIN,        MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOMOUT,       MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOMFULL,      MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_GRID,          MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_HIDDEN,		MF_BYCOMMAND | (pObject ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SHOWHIDDEN,	MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_ABOUT,             MF_BYCOMMAND | MF_ENABLED);

	Popup.CheckMenuItem(IDC_DOC_TRANSPARENT,    MF_BYCOMMAND | (pObject && pObject->IsTransparent() ? MF_CHECKED : MF_UNCHECKED));
	Popup.CheckMenuItem(IDC_DOC_HIDDEN,			MF_BYCOMMAND | (pObject && pObject->IsHidden() ? MF_CHECKED : MF_UNCHECKED));
	Popup.CheckMenuItem(IDC_DOC_SHOWHIDDEN,		MF_BYCOMMAND | (m_bOverrideHidden ? MF_CHECKED : MF_UNCHECKED));
	Popup.CheckMenuItem(IDC_DOC_GRID,           MF_BYCOMMAND | (m_Grid.IsOn() ? MF_CHECKED : MF_UNCHECKED));

	CPoint point;
	::GetCursorPos(&point);
	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
	Menu.DestroyMenu();
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIgnoreSetCursor)
	{
		bHandled = false;
		return E_FAIL;
	}

	CPoint pts;
	::GetCursorPos(&pts);
	::ScreenToClient(m_hWnd, &pts);
	CRect rect;
	GetClientRect(&rect);
	if (pts.x < rect.left || pts.x >= rect.right ||
		pts.y < rect.top  || pts.y >= rect.bottom)
	{
		bHandled = false;
		return E_FAIL;
	}

	int x = pts.x;
	int y = pts.y;

	CPoint Pt(x, y);
	if (m_pClientDC)
		m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(Pt);

	// See which object was hit
	CImageObject* pObject = NULL;
	for (int i = 0; i < m_ImageArray.GetSize(); i++)
	{
		bool bPtInObject = m_ImageArray[i]->HitTest(Pt, m_bOverrideHidden);
		if (bPtInObject)
			pObject = m_ImageArray[i];
	}

	// See if the selected object wants the event
	if (m_Select.SetCursor(x, y, pObject))
		return S_OK;

	if (pObject && IsRecolorSelected())
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_RECOLOR)));
	else
	if (pObject && IsRedEyeSelected())
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_REDEYE)));
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHasFocus = false;

	if (m_Select.KillFocus())
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDropImage(LPCSTR strFileName, POINTL Pt)
{
	// Create a new image object, or reuse the existing one
	CImageObject* pObject = OpenImage(strFileName, NULL/*pDib*/, NULL/*pMatrix*/);
//j	AddImage(strFileName, 0/*iOffset*/, Pt.x, Pt.y, true);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDragOver(POINTL Pt)
{
}

///////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDragLeave()
{
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHasFocus = true;
	g_pDocWindow = this;

	if (m_Select.SetFocus())
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT ScrollCode = LOWORD(wParam);
	UINT TrackCode = HIWORD(wParam);
	// SB_LINERIGHT, SB_LINEDOWN	Scrolls one line right/down
	// SB_LINELEFT, SB_LINEUP		Scrolls one line up/left
	// SB_PAGERIGHT, SB_PAGEDOWN	Scrolls one page right/down
	// SB_PAGELEFT, SB_PAGEUP		Scrolls one page left/up
	// SB_LEFT, SB_TOP				Scrolls to the start
	// SB_RIGHT, SB_BOTTOM			Scrolls to the end
	// SB_ENDSCROLL					Ends scroll
	// SB_THUMBPOSITION				The user has dragged the scroll box (thumb) and released the mouse button
	//								The high-order word indicates the position of the scroll box at the end of the drag operation
	// SB_THUMBTRACK				The user is dragging the scroll box
	//								This message is sent repeatedly until the user releases the mouse button
	//								The high-order word indicates the position that the scroll box has been dragged to

	int nMin = 0;
	int nMax = 0;
	if (uMsg == WM_HSCROLL)
	{
		GetScrollRange(SB_HORZ, &nMin, &nMax);
		int nPage = GetScrollPage(SB_HORZ);
		int nPos = GetScrollPos(SB_HORZ);
		switch (ScrollCode)
		{
			case SB_LINERIGHT:
			case SB_LINELEFT:
			{
				if (SB_LINERIGHT == ScrollCode)
					nPos += 100;
				else
					nPos -= 100;
				break;
			}
			case SB_PAGERIGHT:
			case SB_PAGELEFT:
			{
				if (SB_PAGERIGHT == ScrollCode)
					nPos += nPage;
				else
					nPos -= nPage;
				break;
			}
			case SB_RIGHT:
			case SB_LEFT:
			{
				if (SB_RIGHT == ScrollCode)
					nPos = nMax;
				else
					nPos = nMin;

				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				nPos = TrackCode;
				break;
			}

			default:
				return S_OK;
		}

		if (nPos > nMax + 1 - nPage)
			nPos = nMax + 1 - nPage;
		if (nPos < nMin)
			nPos = nMin;

		SetScrollPos(SB_HORZ, nPos, true);
	}
	else
	if (uMsg == WM_VSCROLL)
	{
		GetScrollRange(SB_VERT, &nMin, &nMax);
		int nPage = GetScrollPage(SB_VERT);
		int nPos = GetScrollPos(SB_VERT);
		switch (ScrollCode)
		{
			case SB_LINEDOWN:
			case SB_LINEUP:
			{
				if (SB_LINEDOWN == ScrollCode)
					nPos += 100;
				else
					nPos -= 100;
				break;
			}
			case SB_PAGEDOWN:
			case SB_PAGEUP:
			{
				if (SB_PAGEDOWN == ScrollCode)
					nPos += nPage;
				else
					nPos -= nPage;
				break;
			}
			case SB_BOTTOM:
			case SB_TOP:
			{
				if (SB_BOTTOM == ScrollCode)
					nPos = nMax;
				else
					nPos = nMin;

				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				nPos = TrackCode;
				break;
			}

			default:
				return S_OK;
		}

		if (nPos > nMax + 1 - nPage)
			nPos = nMax + 1 - nPage;
		if (nPos < nMin)
			nPos = nMin;

		SetScrollPos(SB_VERT, nPos, true);
	}

	SetupPageView(false/*bSetScrollbars*/);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::InvalidatePage()
{
	Repaint(m_PageViewRect);

	if (m_pClientDC)
		m_pClientDC->SetDirty(true);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::Repaint(const CRect& Rect)
{
	CRect rPageView = m_PageViewRect;
	::IntersectRect(&rPageView, &rPageView, &Rect);
	if (rPageView.IsRectEmpty())
		::InvalidateRect(m_hWnd, NULL, false);
	else
	{ // Add the border pixel, and either half the handle size, or the drop shadow size
		rPageView.InflateRect(1, 1);
		rPageView.InflateRect(H_SIZE, H_SIZE);
//j		rPageView.InflateRect(
//j			/*left-top*/1 + H_SIZE, 1 + H_SIZE,
//j			/*right*/	1 + max(H_SIZE, m_ShadowSize.cx),
//j			/*bottom*/	1 + max(H_SIZE, m_ShadowSize.cy)); 
		::InvalidateRect(m_hWnd, &rPageView, false);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetBackgroundMessage(LPCSTR pMessage)
{
	m_strBackgroundMessage = pMessage;
	if (m_strBackgroundMessage.IsEmpty())
		m_strBackgroundMessage = DEFAULT_BACKGROUND_MESSAGE;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetupPageView(bool bSetScrollbars)
{
	#define OUTSIDE_PADDING_X 10
	#define OUTSIDE_PADDING_Y 10

	if (!m_hWnd)
		return;

	if (!m_pClientDC)
		return;

	// Suspend any selection tracking
	m_Select.SuspendTracking();

	// Clear the old page from the offscreen DC
	// Special case: background black = user's application background color
	COLORREF clrBackground = (!m_ImageControl.m_clrBackColor ? ::GetSysColor(COLOR_3DFACE) : m_ImageControl.m_clrBackColor);
	m_pClientDC->Clear(clrBackground);

	if (m_PageRect.IsRectEmpty())
	{
		m_PageViewRect.SetRectEmpty();
		::InvalidateRect(m_hWnd, NULL, false);
		return;
	}
		
	// Invalidate the old page rect (m_PageViewRect) before we change it
	InvalidatePage();

	// Now figure out how much window area we have for the document display
	CRect WndRect;
	GetClientRect(&WndRect);
	
	// Subtract the outside padding
	WndRect.bottom -= m_ShadowSize.cx;
	WndRect.right -= m_ShadowSize.cy;
	WndRect.InflateRect(-OUTSIDE_PADDING_X, -OUTSIDE_PADDING_Y);

	// Convert to Document coordinates
	m_pClientDC->GetDeviceMatrix().Inverse().Transform(WndRect);

	int xVisible = WndRect.Width();
	int yVisible = WndRect.Height();

	// Fit the extended page into the available window space
	int dxPage = m_PageRect.Width();
	int dyPage = m_PageRect.Height();
	if (!dxPage) dxPage = 1;
	if (!dyPage) dyPage = 1;
	double fxPageScale = (double)xVisible / dxPage;
	double fyPageScale = (double)yVisible / dyPage;
	double fFitInWindowScale = min(fxPageScale, fyPageScale);
	if (!fFitInWindowScale) fFitInWindowScale = 1.0;

	// Default the scroll position to the center of the current page
	int xPos = -1;
	int yPos = -1;

	// Special setting to indicate a zoom into the center of the SELECTED OBJECT
	if (m_fZoomScale == -1.0 && m_Select.GetSelected())
	{
		CRect DestRect = m_Select.GetSelected()->GetDestRectTransformed();
		int dxObject = DestRect.Width();
		int dyObject = DestRect.Height();
		if (!dxObject) dxObject = 1;
		if (!dyObject) dyObject = 1;
		double fxObjectScale = (double)xVisible / dxObject;
		double fyObjectScale = (double)yVisible / dyObject;
		double fFitScaleObject = min(fxObjectScale, fyObjectScale);
		m_fZoomScale = fFitScaleObject / fFitInWindowScale;
		m_iZoom = dtoi(m_fZoomScale - 1.0);
		
		// Set the (x,y) position to be the center of the object
		xPos = (DestRect.left + DestRect.right) / 2;
		yPos = (DestRect.top + DestRect.bottom) / 2;
	}
	
	// Special setting to indicate a zoom into the width of the FULL PAGE
	if (m_fZoomScale <= 0)
	{
		m_fZoomScale = fxPageScale / fFitInWindowScale;
		m_iZoom = dtoi(m_fZoomScale - 1.0);
	}

	// Compute the scale and adjust the visible area
	double fScale = fFitInWindowScale * m_fZoomScale;
	xVisible = dtoi((double)xVisible / fScale);
	yVisible = dtoi((double)yVisible / fScale);

	// If the (x,y) position was set to the object center, adjust it by 1/2 the visible area
	if (xPos >= 0 || yPos >= 0)
	{
		xPos -= xVisible / 2;
		yPos -= yVisible / 2;
	}

	// Setup the scrollbars
	if (bSetScrollbars)
		SetupScrollbars(xPos, yPos, dxPage, dyPage, xVisible, yVisible, true/*bRedraw*/);

	// Get the updated position
	xPos = GetScrollPos(SB_HORZ);
	yPos = GetScrollPos(SB_VERT);

	// Calculate a view matrix
	int xExcess = xVisible - dxPage; if (xExcess < 0) xExcess = 0;
	int yExcess = yVisible - dyPage; if (yExcess < 0) yExcess = 0;
	CMatrix ViewMatrix;
	ViewMatrix.Translate(xExcess/2 - xPos, yExcess/2 - yPos);
	ViewMatrix.Scale(fScale, fScale);
	ViewMatrix.Translate(WndRect.left, WndRect.top);
	m_pClientDC->SetViewingMatrix(ViewMatrix);

	// Compute the new page view rectangle (screen coordinates)
	CSize PageSize(m_PageRect.Width(), m_PageRect.Height());
	m_pClientDC->SetClipToView(PageSize, &m_PageViewRect, true/*bIncludeRawDC*/);

	// Invalidate the new page rect (m_PageViewRect) now that we've changed it
	InvalidatePage();

	// Resume any selection tracking
	m_Select.ResumeTracking();
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::SetupScrollbars(int xPos, int yPos, int xSize, int ySize, int xVisible, int yVisible, bool bRedraw)
{
	// If no (x,y) position was passed in, set the position to be the center of the current page
	if (xPos < 0 && yPos < 0)
	{
		xPos = GetScrollPos(SB_HORZ) + GetScrollPage(SB_HORZ)/2 - xVisible/2;
		yPos = GetScrollPos(SB_VERT) + GetScrollPage(SB_VERT)/2 - yVisible/2;
	}

	// Set the scroll range
	SetScrollRange(SB_HORZ, 0, xSize-1, false);
	SetScrollRange(SB_VERT, 0, ySize-1, false);

	// Set the scroll page size
	SetScrollPage(SB_HORZ, xVisible, false);
	SetScrollPage(SB_VERT, yVisible, false);

	// Reset the scroll position and be sure it is valid
	ResetScrollPos(SB_HORZ, xPos, bRedraw);
	ResetScrollPos(SB_VERT, yPos, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::EliminateScrollbars(bool bRedraw)
{
	// Set the scroll range
	SetScrollRange(SB_HORZ, 0, 0/*xSize-1*/, false);
	SetScrollRange(SB_VERT, 0, 0/*ySize-1*/, false);

	// Set the scroll page size
	SetScrollPage(SB_HORZ, 0/*xVisible*/, false);
	SetScrollPage(SB_VERT, 0/*yVisible*/, false);

	// Set the scroll position
	SetScrollPos(SB_HORZ, 0/*xPos*/, bRedraw);
	SetScrollPos(SB_VERT, 0/*yPos*/, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::ResetScrollPos(int nBar, int nPos, bool bRedraw)
{
	int nMin = 0;
	int nMax = 0;
	GetScrollRange(nBar, &nMin, &nMax);
	if (nPos < nMin)
		nPos = nMin;
	int nPage = GetScrollPage(nBar);
	if (nPos > nMax + 1 - nPage)
		nPos = nMax + 1 - nPage;
	SetScrollPos(nBar, nPos, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::SetScrollPage(int nBar, UINT nPage, bool bRedraw)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = nPage;
	SetScrollInfo(nBar, &si, bRedraw);
}

//////////////////////////////////////////////////////////////////////
UINT CDocWindow::GetScrollPage(int nBar)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = 0;
	GetScrollInfo(nBar, &si);
	return si.nPage;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::Zoom(int iOffset)
{
	if (m_ImageArray.GetSize() <= 0)
		return false;

	if (!iOffset) // Zoom full
		m_iZoom = 0;
	else
		m_iZoom += iOffset;

	if (m_iZoom >= 0)
		m_fZoomScale = 1.0 + m_iZoom;
	else
	if (m_iZoom < 0)
	{
		m_iZoom = -m_iZoom;
		double iZoomList[] = {1.0, 0.75, 0.67, 0.5, 0.33, 0.25};
		int nMaxZoom = (sizeof(iZoomList) / sizeof(double)) - 1;
		if (m_iZoom > nMaxZoom)
			m_iZoom = nMaxZoom; 

		m_fZoomScale = iZoomList[m_iZoom];
		m_iZoom = -m_iZoom;
	}

	SetupPageView(true/*bSetScrollbars*/);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::ZoomEx(UINT nZoomLevel, double fZoomScale)
{
	m_fZoomScale = fZoomScale;
	m_iZoom = nZoomLevel;

	SetupPageView(true/*bSetScrollbars*/);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
{
	bool bOK = m_DocCommands.DoCommand(strCommand, strValue);

	// Always take back the focus after an external command
//j	SetFocus(); //j This causes a problem if the ImageControl has WS_TABSTOP set

	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
long CDocWindow::GetCommand(LPCTSTR strCommand)
{
	// The return value of this function is a set of bitwise flags about the command
	// Bit 1 = Enabled
	// Bit 2 = Checked
	return m_DocCommands.GetCommand(strCommand);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CDocWindow::ImageControlKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!g_pDocWindow)
		return false;

	HHOOK hHook = g_pDocWindow->GetKeyboardHook();
	if (!hHook)
		return false;

	UINT nChar = (UINT)wParam; // The virtual key code
	// nCode == HC_ACTION(0) or HC_NOREMOVE(3)
	if (nCode < 0 || !g_pDocWindow->HasFocus())
	{ // This key belongs to the browser
		if (nChar < VK_F7 || nChar > VK_F12 || nCode != HC_ACTION)
			return ::CallNextHookEx(hHook, nCode, wParam, lParam);

		// Steal F7-F12 from the browser
//j		g_pDocWindow->SetFocus(); //j This causes a problem if the ImageControl has WS_TABSTOP set
	}

	if (nChar == VK_CONTROL || nChar == VK_SHIFT)
		return ::CallNextHookEx(hHook, nCode, wParam, lParam);

	WORD nRepeatCount = LOWORD(lParam);
	WORD wFlags = HIWORD(lParam);
	bool bKeyDown	=  !(wFlags & KF_UP);
	bool bRepeat	= !!(wFlags & KF_REPEAT);
	bool bAltDown	= !!(wFlags & KF_ALTDOWN);
//j	bool bMenuMode	= !!(wFlags & KF_MENUMODE);
//j	bool bExtended	= !!(wFlags & KF_EXTENDED);
//j	bool bDlgMode	= !!(wFlags & KF_DLGMODE);

	// Handle keyboard accelerators
	if (bKeyDown)
	{
		UINT idMessage = NULL;
		bool bControl = CONTROL;
		if (bControl)
		{
//j			if (nChar == 'C' && !bRepeat)		idMessage = IDC_DOC_COPY;			else
//j			if (nChar == 'O' && !bRepeat)		idMessage = IDC_DOC_OPEN;			else
//j			if (nChar == 'P' && !bRepeat)		idMessage = IDC_DOC_PRINT;			else
//j			if (nChar == 'S' && !bRepeat)		idMessage = IDC_DOC_SAVE;			else
//j			if (nChar == 'V' && !bRepeat)		idMessage = IDC_DOC_PASTE;			else
//j			if (nChar == 'X' && !bRepeat)		idMessage = IDC_DOC_CUT;			else
//j			if (nChar == 'Z')					idMessage = (SHIFT ? IDC_DOC_REDO : IDC_DOC_UNDO);	else
//j			if (nChar == VK_DELETE && !bRepeat)	idMessage = IDC_DOC_DELETE;			else
//j				int i = 0;

			if (idMessage)
			{
				::PostMessage(g_pDocWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)g_pDocWindow->m_hWnd);
				return true;
			}
		}
		else
		if (bAltDown)
		{
//j			if (nChar == VK_UP)					idMessage = IDC_DOC_SELECTPREV;		else
//j			if (nChar == VK_DOWN)				idMessage = IDC_DOC_SELECTNEXT;		else
//j				int i = 0;

			if (idMessage)
			{
				::PostMessage(g_pDocWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)g_pDocWindow->m_hWnd);
				return true;
			}
		}
	}

	switch (nChar)
	{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		{
			int bHandled = true;
			UINT nChar1 = nChar;
			if (bKeyDown)
				g_pDocWindow->OnKeyDown(0, nChar1, lParam, bHandled);
			else
				g_pDocWindow->OnKeyUp(0, nChar1, lParam, bHandled);
			return true;
		}

		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->OnKeyDown(0, nChar, lParam, bHandled);
			else
				g_pDocWindow->OnKeyUp(0, nChar, lParam, bHandled);
			return true;
		}

		case VK_F8: // Display the saved file size
		{
			int bHandled = true;
			if (bRepeat)
				return true;
//j			if (bKeyDown)
//j				g_pDocWindow->DisplaySavedFileSize();
			return true;
		}

		case VK_F9: // Zoom in on the next selection
		{
			int bHandled = true;
//j			if (bKeyDown)
//j			{
//j				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SELECTNEXT);
//j				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_ZOOM2);
//j			}
			return true;
		}

		case VK_F10: // Zoom out on the cursor
		{
			int bHandled = true;
//j			if (bKeyDown)
//j				g_pDocWindow->PostMessage(WM_COMMAND, IDC_DOC_ZOOMOUT, 0xcaca);
			return true;
		}

		case VK_F11: // Zoom in on the cursor
		{
			int bHandled = true;
//j			if (bKeyDown)
//j				g_pDocWindow->PostMessage(WM_COMMAND, IDC_DOC_ZOOMIN, 0xbaba);
			return true;
		}

		case VK_F12: // Developer Features
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
			{
				if (CONTROL && SHIFT)
				{ // Secret key to enable additional functionality
					bool bEnable = !g_pDocWindow->SpecialsEnabled();
					g_pDocWindow->SetSpecialsEnabled(bEnable);

					CRegKey regkey;
					if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
					{
						CString strValue;
						strValue.Format("%d", bEnable);
						bool bOK = (regkey.SetStringValue(REGVAL_SPECIALSENABLED, strValue) == ERROR_SUCCESS);
					}

					g_pDocWindow->m_ImageControl.ConfigureWindows();

					CString strMsg = String("You have just %s the developer features.", (bEnable ? "ENABLED" : "DISABLED"));
					if (bEnable)
						strMsg += "\n\nAccess these features by right-clicking on the project and selecting from the popup menu.";
					CMessageBox::Message(strMsg);
				}
			}

			return true;
		}

		case VK_TAB:
		case VK_BACK:
		{
			int bHandled = true;
			if (bKeyDown)
				g_pDocWindow->OnChar(NULL, nChar, lParam, bHandled);
			return true;
		}
	}

	return ::CallNextHookEx(hHook, nCode, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetPageRect(CImageObject* pBaseObject, bool bUnZoom)
{
	// Affectively, this is ActivateNewDoc() or SetBaseObject()
	if (!pBaseObject)
		m_PageRect.SetRectEmpty();
	else
	{
		CRect DestRect = pBaseObject->GetDestRectTransformed();
		if (!m_PageRect.IsRectEmpty())
		{
			// Offset all the images
			int dx = m_PageRect.left - DestRect.left;
			int dy = m_PageRect.top - DestRect.top;
			for (int i = 0; i < m_ImageArray.GetSize(); i++)
			{
				CImageObject* pObjectEdit = m_ImageArray[i];
				CMatrix Matrix = pObjectEdit->GetMatrix();
				Matrix.Translate(dx, dy);
				pObjectEdit->SetMatrix(Matrix);
			}
		}

		m_PageRect = DestRect;
		m_PageRect.MoveToXY(0, 0);
	}

	if (bUnZoom)
	{
		// Unzoom the view
		m_iZoom = 0;
		m_fZoomScale = 1.0;
		EliminateScrollbars(true/*bRedraw*/);
	}
	
	SetupPageView(true/*bSetScrollbars*/);
}

/////////////////////////////////////////////////////////////////////////////
CImageObject* CDocWindow::OpenImage(LPCTSTR strFileName, const BITMAPINFOHEADER* pDIB, CMatrix* pMatrix)
{
	CImageObject* pObject = new CImageObject(&m_PageRect, pDIB); // PageRect might be empty
	if (strFileName)
		bool bOK = pObject->Open(strFileName);
	
	if (pMatrix)
	{
		double fAngle = pMatrix->GetAngle();
		if (fAngle)
		{
			// Find the object center
			CRect DestRect = pObject->GetDestRectTransformed();
			double fcx = (DestRect.right + DestRect.left) / 2;
			double fcy = (DestRect.bottom + DestRect.top) / 2;

			CMatrix Matrix = pObject->GetMatrix();
			Matrix.Rotate(fAngle, fAngle, fcx, fcy);
			pObject->SetMatrix(Matrix);
			SetPageRect(pObject, true/*bUnZoom*/);
		}
	}

	if (m_ImageArray.Add(pObject))
		m_Select.Select(pObject);

//j	if (GetFocus() != m_hWnd)
//j		SetFocus(); //j This causes a problem if the ImageControl has WS_TABSTOP set

	// First image in gets to set the PageRect (document size)
	bool bBaseObject = (pObject == GetBaseObject());
	if (bBaseObject)
		SetPageRect(pObject, true/*bUnZoom*/);
	
	// Invalidate the image's current location
	InvalidateImage(pObject);

	return pObject;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::StartOver()
{
	CImageObject* pBaseObject = GetBaseObject();
	if (!pBaseObject)
		return true;

	// Close all but the base object
	while (m_ImageArray.GetSize() > 1)
		CloseImage(m_ImageArray[1]);

	// Clear the page rect
	SetPageRect(NULL/*pImageObject*/, true/*bUnZoom*/);

	m_Select.Unselect();
	pBaseObject->EditUndo();
	pBaseObject->ReOpen();
	m_Select.Select(pBaseObject);

	// First image in gets to set the PageRect (document size)
	SetPageRect(pBaseObject, true/*bUnZoom*/);

	pBaseObject->SetModified(false);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::CloseImage(CImageObject* pObject)
{
	if (!pObject)
		return false;

	// Invalidate the image's current location
	InvalidateImage(pObject);

	bool bSelected = (pObject == m_Select.GetSelected());
	if (bSelected)
		m_Select.Unselect();

	bool bRemoved = !!m_ImageArray.Remove(pObject);
	pObject->Close();
	delete pObject;
	pObject = NULL;

	CImageObject* pBaseObject = GetBaseObject();
	if (bSelected && pBaseObject)
		m_Select.Select(pBaseObject);

	SetPageRect(pBaseObject, true/*bUnZoom*/);

	return bRemoved;
}

/////////////////////////////////////////////////////////////////////////////
CImageObject* CDocWindow::GetSelectedObject()
{
	CImageObject* pObject = m_Select.GetSelected();
	if (pObject)
		return pObject;
	
	CImageObject* pBaseObject = GetBaseObject();
	if (pBaseObject && !pBaseObject->IsHidden())
		return pBaseObject;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImageObject* CDocWindow::GetBaseObject()
{
	int iImagesLeft = m_ImageArray.GetSize();
	CImageObject* pBaseObject = (iImagesLeft ? m_ImageArray[0] : NULL);
	return pBaseObject;
}

/////////////////////////////////////////////////////////////////////////////
CImageObject* CDocWindow::FindImage(UINT id)
{
	for (int i = 0; i < m_ImageArray.GetSize(); i++)
	{
		if (id == m_ImageArray[i]->GetID())
			return m_ImageArray[i];
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::IsRecolorSelected()
{
	return (
		m_ToolSettings.m_SelectedTool == TOOL_SOLIDFILL	||
		m_ToolSettings.m_SelectedTool == TOOL_SWEEPDOWN	||
		m_ToolSettings.m_SelectedTool == TOOL_SWEEPRIGHT	||
		m_ToolSettings.m_SelectedTool == TOOL_SWEEPCENTER);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::IsRedEyeSelected()
{
	return (m_ToolSettings.m_SelectedTool == TOOL_REDEYE);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::OnToolButtonDown(CPoint Pt, CImageObject* pObject)
{
	if (!pObject)
		return;

	bool bOK = false;
	if (IsRedEyeSelected())
		bOK = pObject->RedEyeClicked(Pt, m_ToolSettings);
	else
	if (IsRecolorSelected())
		bOK = pObject->RecolorClicked(Pt, m_ToolSettings);
	if (!bOK)
		return;

	// Invalidate the image's current location
	InvalidateImage(pObject);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::InvalidateImage(CImageObject* pObject)
{
	if (!pObject)
		return;

	if (pObject->IsHidden(m_bOverrideHidden))
		return;

	if (!m_pClientDC)
		return;

	CMatrix MatrixSaved = m_pClientDC->SetSymbolMatrix(pObject->GetMatrix());

	CRect DestRect = pObject->GetDestRect();
	CSize Size(0, 0);
	pObject->GetDestInflateSize(Size);
	if (!Size.cx && !Size.cy)
		m_pClientDC->GetSymbolToDeviceMatrix().Transform(DestRect);
	else
	{
		pObject->GetMatrix().Transform(DestRect);
		DestRect.InflateRect(Size);
		m_pClientDC->GetViewToDeviceMatrix().Transform(DestRect);
	}

	// In case the Object is being transformed, add in the handle size
	DestRect.InflateRect(H_SIZE, H_SIZE);

	// If this is an image that should cover to the edge, invalidate the whole page
	if (m_pClientDC->GetCoverAllowed() && pObject->GetCoverDraw())
	{
		DestRect = m_pClientDC->GetViewRect();
		m_pClientDC->GetViewToDeviceMatrix().Transform(DestRect);
	}

	Repaint(DestRect);

	m_pClientDC->SetDirty(true);

	m_pClientDC->SetSymbolMatrix(MatrixSaved);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocOpen(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.OpenImage();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocClose(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.CloseImage();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocSave(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.SaveImage();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocTransparent(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.SetTransparent();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocZoomIn(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.ZoomIn();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocZoomOut(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.ZoomOut();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocZoomFull(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.ZoomFull();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocGrid(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;
	m_Grid.On(!m_Grid.IsOn());
	InvalidatePage();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	bool bOK = m_DocCommands.SetHidden();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocShowHidden(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;
	m_bOverrideHidden = !m_bOverrideHidden;
	InvalidatePage();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDocAbout(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;

//j	CString strVersion = String(IDS_MSGBOXTITLE) + " plugin version " + VER_PRODUCT_VERSION_STR;
	CString strVersion = "1.0";
	CString strCopyright = "Copyright 2006"; // VER_COPYRIGHT;
	CString strMessage;
	strMessage.Format("%s\n%s\n%s\n%s",
		"Create & Print Image Editor",
		"by AmericanGreetings.com.",
		strVersion,
		strCopyright);

	CMessageBox::Message(strMessage);
	return S_OK;
}
