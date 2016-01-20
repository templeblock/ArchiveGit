#include "stdafx.h"
#include "ImageControl.h"
#include "DocWindow.h"
#include "RegKeys.h"
#include "Version.h"
#include "MessageBox.h"
#include "Utility.h"
#include "FloodFill.h"

#ifdef _DEBUG
//j	#define new DEBUG_NEW
#endif _DEBUG

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
	m_strBackgroundMessage = DEFAULT_BACKGROUND_MESSAGE;
	m_ShadowSize.SetSize(0, 0);
	m_RecolorMode = RECOLOR_OFF;
	m_PrimaryColor = 0;
	m_SecondaryColor = 0;
	m_fUsePrimaryColor = true;

	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		DWORD dwLength = MAX_PATH;
		char strValue[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_SPECIALSENABLED, strValue, &dwLength) == ERROR_SUCCESS)
			m_bSpecialsEnabled = !!atoi(strValue);
	}
}

//////////////////////////////////////////////////////////////////////
CDocWindow::~CDocWindow()
{
	Free();

	m_ImageArray.RemoveAll();
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

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT PaintStruct;
	HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
	if (!hPaintDC)
		return E_FAIL;

	if (!m_pClientDC || m_ImageArray.GetSize() <= 0)
	{
		::FillRect(hPaintDC, &PaintStruct.rcPaint, CAGBrush(BACKGOUND_COLOR));

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
			// Calculate the border rectangle
			CRect rPageView = m_PageViewRect;
			rPageView.InflateRect(1, 1); // Add the border pixel

			// Before drawing anything, open up the offscreen clipping to include a border pixel and drop shadow
			HRGN hClipRegion = m_pClientDC->SaveClipRegion();
			CRect rClipRect = rPageView;
			rClipRect.right += m_ShadowSize.cx; // Add the drop shadow
			rClipRect.bottom += m_ShadowSize.cy;
			m_pClientDC->SetClipRect(rClipRect);

			// Draw the page background and the border pixel
			::FillRect(hDC, &rPageView, CAGBrush(WHITE_BRUSH, eStock));
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
		}
				
		// Draw all the images
		for (int i = 0; i < m_ImageArray.GetSize(); i++)
			m_ImageArray[i]->Draw(*m_pClientDC, 0);

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

	CPoint pt(x, y);
	if (m_pClientDC)
		m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);

	// See which object was hit
	CImageObject* pObject = NULL;
	for (int i = 0; i < m_ImageArray.GetSize(); i++)
	{
		bool bPtInObject = m_ImageArray[i]->HitTest(pt, 0);
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

	if (IsRecolorSelected())
		HandleImageRecolor(x, y, pObject);

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
//j	Popup.EnableMenuItem(1/*Add*/,              MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
//j	Popup.EnableMenuItem(IDC_DOC_NEW,           MF_BYCOMMAND | MF_ENABLED);
//j	Popup.EnableMenuItem(IDC_DOC_OPEN,          MF_BYCOMMAND | MF_ENABLED);
//j	Popup.CheckMenuItem(IDC_DOC_LOCKED,		    MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() && m_Select.SymIsLocked() ? MF_CHECKED : MF_UNCHECKED));

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

	CPoint pt(x, y);
	if (m_pClientDC)
		m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);

	// See which object was hit
	CImageObject* pObject = NULL;
	for (int i = 0; i < m_ImageArray.GetSize(); i++)
	{
		bool bPtInObject = m_ImageArray[i]->HitTest(pt, 0);
		if (bPtInObject)
			pObject = m_ImageArray[i];
	}

	// See if the selected object wants the event
	if (m_Select.SetCursor(x, y, pObject))
		return S_OK;

	// Otherwise...
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
	CImageObject* pObject = CreateImage(strFileName);
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
	CRect rPageView = m_PageViewRect;
	rPageView.InflateRect(1, 1); // Add the border pixel
	rPageView.right += m_ShadowSize.cx; // Add the drop shadow
	rPageView.bottom += m_ShadowSize.cy;
	::InvalidateRect(m_hWnd, &rPageView, false);

	if (m_pClientDC)
		m_pClientDC->SetDirty(true);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::Repaint(const CRect& Rect)
{
	CRect rPageView = m_PageViewRect;
	rPageView.InflateRect(1, 1); // Add the border pixel
	::IntersectRect(&rPageView, &rPageView, &Rect);
	::InvalidateRect(m_hWnd, &rPageView, false);
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

	if (m_PageRect.IsRectEmpty())
		return;
		
	// Suspend any selection tracking
	m_Select.SuspendTracking();

	// Clear the old page from the offscreen DC
	m_pClientDC->Clear();

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
	CAGMatrix ViewMatrix;
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
CImageObject* CDocWindow::CreateImage(LPCTSTR strFileName)
{
	CImageObject* pObject = new CImageObject(m_PageRect); // PageRect might be empty
	bool bOK = pObject->Open(strFileName);
	
	if (m_ImageArray.Add(pObject))
		m_Select.Select(pObject);

//j	if (GetFocus() != m_hWnd)
//j		SetFocus(); //j This causes a problem if the ImageControl has WS_TABSTOP set

	// First image in gets to set the underlying page rect
	// Affectively, this is ActivateNewDoc()
	if (m_ImageArray.GetSize() == 1)
	{
		m_PageRect = pObject->GetPageRect();

		// Unzoom the view
		m_iZoom = 0;
		m_fZoomScale = 1.0;
	
		EliminateScrollbars(true/*bRedraw*/);
		SetupPageView(true/*bSetScrollbars*/);
	}

	return pObject;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::DeleteImage(CImageObject* pObject)
{
	if (pObject == m_Select.GetSelected())
		m_Select.Unselect();

	return !!m_ImageArray.Remove(pObject);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetPrimaryColor(COLORREF Color)
{
	m_PrimaryColor = Color;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetSecondaryColor(COLORREF Color)
{
	m_SecondaryColor = Color;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetRecolorMode(RECOLOR_MODE RecolorMode)
{
	m_RecolorMode = RecolorMode;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::UsePrimaryColor(bool fPrimary)
{
	m_fUsePrimaryColor = fPrimary;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::IsRecolorSelected()
{
	bool fRet = false;
	if (m_RecolorMode != RECOLOR_OFF)
		fRet = true;

	return fRet;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::HandleImageRecolor(int x, int y, CImageObject* pObject)
{
	if (pObject == NULL)
		return;


	BITMAPINFOHEADER* pBitmap = pObject->GetDib();
	if (pBitmap != NULL)
	{
		CRect DestRect = pObject->GetDestRect();
		CFloodFill FloodFill;

		CAGMatrix Matrix;
		CPoint pt(x, y);
		if (m_pClientDC)
			m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);

		double xScale = (double)DibWidth(pBitmap) / DestRect.Width();
		double yScale = (double)DibHeight(pBitmap) / DestRect.Height();
		Matrix.Translate(-DestRect.left, -DestRect.top);
		Matrix.Scale(xScale, yScale, 0, 0);
		Matrix.Transform(pt);

		FloodFill.Fill(pBitmap, &DestRect, pt.x, pt.y, (m_fUsePrimaryColor ? m_PrimaryColor : m_SecondaryColor));
	}

	// Invalidate the image's current location
	InvalidateImage(pObject);

	pObject->SetModified(true);

	//if (pDIBNew)
	//{
	//	pObject->SetDib(pDIBNew);
	//	free(pDIB);
	//}
	//else
	//{
	//	// Adjust the image's matrix
	//	CRect DestRect = pObject->GetDestRect();
	//	CAGMatrix Matrix = pObject->GetMatrix();
	//	Matrix.Transform(DestRect);
	//	Matrix.Scale((bFlipX ? -1 : 1), (bFlipY ? -1 : 1),
	//		(DestRect.left + DestRect.right) / 2,
	//		(DestRect.top + DestRect.bottom) / 2);

	//	pObject->SetMatrix(Matrix);
	//}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::InvalidateImage(CImageObject* pObject)
{
	if (!pObject)
		return;

	if (pObject->IsHidden())
		return;

	if (!m_pClientDC)
		return;

	CAGMatrix MatrixSaved = m_pClientDC->SetSymbolMatrix(pObject->GetMatrix());

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
	if (m_pClientDC->GetCoverAllowed() && pObject->DoCoverDraw())
	{
		DestRect = m_pClientDC->GetViewRect();
		m_pClientDC->GetViewToDeviceMatrix().Transform(DestRect);
	}

	Repaint(DestRect);

	m_pClientDC->SetDirty(true);

	m_pClientDC->SetSymbolMatrix(MatrixSaved);
}
