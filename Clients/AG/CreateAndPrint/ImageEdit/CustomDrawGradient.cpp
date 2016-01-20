#include "stdafx.h"
#include "CustomDrawGradient.h"
#include "Utility.h"

// ItemState Examples
//	UINT uItemState = lpNMCustomDraw->uItemState;
//	bool bChecked = ((uItemState & CDIS_CHECKED) == CDIS_CHECKED);
//	bool bDefault = ((uItemState & CDIS_DEFAULT) ==  CDIS_DEFAULT);
//	bool bDisabled = ((uItemState & CDIS_DISABLED) ==  CDIS_DISABLED);
//	bool bFocus = ((uItemState & CDIS_FOCUS) ==  CDIS_FOCUS);
//	bool bGrayed = ((uItemState & CDIS_GRAYED) ==  CDIS_GRAYED);
//	bool bHot = ((uItemState & CDIS_HOT) ==  CDIS_HOT);
//	bool bUnknown = ((uItemState & CDIS_INDETERMINATE) == CDIS_INDETERMINATE);
//	bool bMarked = ((uItemState & CDIS_MARKED) ==  CDIS_MARKED);
//	bool bSelected = ((uItemState & CDIS_SELECTED) ==  CDIS_SELECTED);

/////////////////////////////////////////////////////////////////////////////
CCustomDrawGradient::CCustomDrawGradient()
{
	m_dwFlags = 0;
	m_ColorFrom = RGB(0, 0, 0);
	m_ColorTo = RGB(255, 255, 255);
}

/////////////////////////////////////////////////////////////////////////////
CCustomDrawGradient::~CCustomDrawGradient()
{
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::SetStyle(HWND hWnd, DWORD dwFlags, COLORREF ColorFrom, COLORREF ColorTo)
{
	Init(hWnd);
	m_dwFlags = dwFlags;
	m_ColorFrom = ColorFrom;
	m_ColorTo = ColorTo;
	m_Window.Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::Init(HWND hWnd)
{
	m_Window.m_hWnd = hWnd;
	
	// If the control has no text, then for some reason, the CDDS_POSTPAINT custom draw stage never occurs
	CString strText;
	m_Window.GetWindowText(strText);
	if (strText.GetLength() <= 0)
		m_Window.SetWindowText(_T(" "));

	// Make sure BS_NOTIFY is on so we get double clicks 
	m_Window.ModifyStyle(0, BS_NOTIFY);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	if (!lpNMCustomDraw)
		return CDRF_DODEFAULT;

	if (!(m_dwFlags & CDG_HASITEMS))
		PostPaintChannel(lpNMCustomDraw);

	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTERASE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	if (!lpNMCustomDraw)
		return CDRF_DODEFAULT;

	DWORD_PTR dwItemSpec = lpNMCustomDraw->dwItemSpec; // this is control specific; valid only with CDDS_ITEM
	switch (dwItemSpec)
	{
		case TBCD_CHANNEL:
		{
			PrePaintChannel(lpNMCustomDraw);
			break;
		}

		case TBCD_THUMB:
		{
			break;
		}

		case TBCD_TICS:
		{
			break;
		}
	}

	lpNMCustomDraw->uItemState &= ~CDIS_FOCUS;
	return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	if (!lpNMCustomDraw)
		return CDRF_DODEFAULT;

	DWORD_PTR dwItemSpec = lpNMCustomDraw->dwItemSpec; // this is control specific; valid only with CDDS_ITEM
	switch (dwItemSpec)
	{
		case TBCD_CHANNEL:
		{
			PostPaintChannel(lpNMCustomDraw);
			break;
		}

		case TBCD_THUMB:
		{
			break;
		}

		case TBCD_TICS:
		{
			break;
		}
	}

	lpNMCustomDraw->uItemState &= ~CDIS_FOCUS;
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomDrawGradient::OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::PrePaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw)
{
	CRect ClientRect;
	m_Window.GetClientRect(ClientRect);

	bool bHorz = ClientRect.Height() < ClientRect.Width();
	if (bHorz)
	{
		int iDelta = ClientRect.bottom - lpNMCustomDraw->rc.bottom;
		if (iDelta < 0) iDelta = 0;
		lpNMCustomDraw->rc.top -= iDelta;
		lpNMCustomDraw->rc.bottom += iDelta;
	}
	else
	{
		int iDelta = ClientRect.right - lpNMCustomDraw->rc.right;
		if (iDelta < 0) iDelta = 0;
		lpNMCustomDraw->rc.left -= iDelta;
		lpNMCustomDraw->rc.right += iDelta;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::PostPaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw)
{
	CRect ClientRect;
	m_Window.GetClientRect(ClientRect);
    CRect rect = lpNMCustomDraw->rc;
	rect.IntersectRect(rect, ClientRect);
    rect.InflateRect(-3, -3);

	CDC dc(lpNMCustomDraw->hdc);
    if (m_dwFlags & CDG_CONTRAST)
	{
		int iDelta = rect.Height() / 2;
		rect.bottom -= iDelta;
		DrawGradient(dc, rect, RGB(128,128,128), m_ColorFrom);
		rect.top = rect.bottom;
		rect.bottom += iDelta;
		DrawGradient(dc, rect, RGB(128,128,128), m_ColorTo);
	}
	else
    if (m_dwFlags & CDG_HUE)
	{
		HSL HueFrom;
		HueFrom.hue = GetRValue(m_ColorFrom);
		HueFrom.sat = GetGValue(m_ColorFrom);
		HueFrom.lum = GetBValue(m_ColorFrom);
		HSL HueTo;
		HueTo.hue = GetRValue(m_ColorTo);
		HueTo.sat = GetGValue(m_ColorTo);
		HueTo.lum = GetBValue(m_ColorTo);
		DrawHorizontalHues(dc, rect, HueFrom, HueTo);
	}
	else // Normal
	{
		DrawGradient(dc, rect, m_ColorFrom, m_ColorTo);
	}

	dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////
COLORREF ComputeColor(double fScale, COLORREF clrFrom, COLORREF clrTo)
{
	BYTE R = GetRValue(clrFrom);
	BYTE G = GetGValue(clrFrom);
	BYTE B = GetBValue(clrFrom);

	int dR = (int)GetRValue(clrTo) - R;
	int dG = (int)GetGValue(clrTo) - G;
	int dB = (int)GetBValue(clrTo) - B;

	R += dtoi(fScale * dR);
	G += dtoi(fScale * dG);
	B += dtoi(fScale * dB);

	return RGB(R, G, B);
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawGradient(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	if (m_dwFlags & CDG_VERTICAL)
		DrawVertical(dc, rect, clrFrom, clrTo);
	else
	if (m_dwFlags & CDG_HORIZONTAL)
		DrawHorizontal(dc, rect, clrFrom, clrTo);
	else
	if (m_dwFlags & CDG_FROMCENTER)
		DrawFromCenter(dc, rect, clrFrom, clrTo);
	else
	if (m_dwFlags & CDG_SOLID)
	{
		// Don't draw anything - let the default draw stand
		// DrawSolid(dc, rect, clrFrom, clrTo);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawSolid(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	CBrush brush;
	brush.CreateSolidBrush(clrFrom);
	dc.FillRect(rect, brush);
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawVertical(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	int yCount = rect.Height();
	int iRange = yCount - 1;

	for (int i=0; i<yCount; i++)
	{
		int iDistance = i;
		double fScale = (double)iDistance / iRange;
		COLORREF Color = ComputeColor(fScale, clrFrom, clrTo);

		CRect grect;
		grect.left = rect.left;
		grect.right = rect.right;
		grect.top = rect.top + i;
		grect.bottom = grect.top + 1;

		CBrush brush;
		brush.CreateSolidBrush(Color);
		dc.FillRect(grect, brush);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawHorizontal(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	int xCount = rect.Width();
	int iRange = xCount - 1;

	for (int i=0; i<xCount; i++)
	{
		int iDistance = i;
		double fScale = (double)iDistance / iRange;
		COLORREF Color = ComputeColor(fScale, clrFrom, clrTo);

		CRect grect;
		grect.left = rect.left + i;
		grect.right = grect.left + 1;
		grect.top = rect.top;
		grect.bottom = rect.bottom;

		CBrush brush;
		brush.CreateSolidBrush(Color);
		dc.FillRect(grect, brush);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawFromCenter(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	int xCount = rect.Width();
	int yCount = rect.Height();
	int cx = xCount / 2;
	int cy = yCount / 2;
	int iMaxRadius = min(cx, cy);
	bool bSmallerX = (xCount < yCount);

	CBrush brush;
	brush.CreateSolidBrush(clrTo);
	dc.FillRect(rect, brush);

	for (int r = 0; r <= iMaxRadius; r++)
	{
		double fScale = (double)(iMaxRadius - r) / iMaxRadius;
		COLORREF Color = ComputeColor(fScale, clrFrom, clrTo);

		int rx =  (bSmallerX ? r : (((r * xCount) + cy) / yCount));
		int ry = (!bSmallerX ? r : (((r * yCount) + cx) / xCount));

		CRect grect = rect;
		grect.InflateRect(-rx, -ry);

		CBrush brush;
		brush.CreateSolidBrush(Color);
		CBrush hOldBrush = dc.SelectBrush(brush);
		CPen hOldPen = dc.SelectStockPen(NULL_PEN);
		dc.Ellipse(grect.left, grect.top, grect.right+1, grect.bottom+1);
		dc.SelectBrush(hOldBrush);
		dc.SelectPen(hOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomDrawGradient::DrawHorizontalHues(CDC& dc, const CRect& rect, const HSL& HueFrom, const HSL& HueTo)
{
	int xCount = rect.Width();
	int iRange = xCount - 1;

	for (int i=0; i<xCount; i++)
	{
		int iDistance = i;
		double fScale = (double)iDistance / iRange;

		BYTE H = HueFrom.hue;
		BYTE S = HueFrom.sat;
		BYTE L = HueFrom.lum;

		int dH = (int)HueTo.hue - H;
		int dS = (int)HueTo.sat - S;
		int dL = (int)HueTo.lum - L;

		H += dtoi(fScale * dH);
		S += dtoi(fScale * dS);
		L += dtoi(fScale * dL);

		RGBS rgb;
		HSLtoRGB(H, S, L, &rgb);

		CRect grect;
		grect.left = rect.left + i;
		grect.top = rect.top;
		grect.right = grect.left + 1;
		grect.bottom = rect.bottom;

		CBrush brush;
		brush.CreateSolidBrush(RGB(rgb.red, rgb.green, rgb.blue));
		dc.FillRect(grect, brush);
	}
}
