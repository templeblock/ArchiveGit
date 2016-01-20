#include "stdafx.h"
#include "CustomSlider.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ItemState Examples
//	UINT uItemState = lpNMCustomDraw->uItemState;
//	bool bChecked = ((lpNMCustomDraw->uItemState & CDIS_CHECKED) == CDIS_CHECKED);
//	bool bDefault = ((lpNMCustomDraw->uItemState & CDIS_DEFAULT) ==  CDIS_DEFAULT);
//	bool bDisabled = ((lpNMCustomDraw->uItemState & CDIS_DISABLED) ==  CDIS_DISABLED);
//	bool bFocus = ((lpNMCustomDraw->uItemState & CDIS_FOCUS) ==  CDIS_FOCUS);
//	bool bGrayed = ((lpNMCustomDraw->uItemState & CDIS_GRAYED) ==  CDIS_GRAYED);
//	bool bHot = ((lpNMCustomDraw->uItemState & CDIS_HOT) ==  CDIS_HOT);
//	bool bUnknown = ((lpNMCustomDraw->uItemState & CDIS_INDETERMINATE) == CDIS_INDETERMINATE);
//	bool bMarked = ((lpNMCustomDraw->uItemState & CDIS_MARKED) ==  CDIS_MARKED);
//	bool bSelected = ((lpNMCustomDraw->uItemState & CDIS_SELECTED) ==  CDIS_SELECTED);

/////////////////////////////////////////////////////////////////////////////
CCustomSlider::CCustomSlider()
{
	m_strStyle.Empty();
}

/////////////////////////////////////////////////////////////////////////////
CCustomSlider::~CCustomSlider()
{
}

/////////////////////////////////////////////////////////////////////////////
void CCustomSlider::Init()
{
	// If the control has no text, then for some reason, the CDDS_POSTPAINT custom draw stage never occurs
	CString strText;
	GetWindowText(strText);
	if (strText.GetLength() <= 0)
		SetWindowText(_T(" "));

	// Make sure BS_NOTIFY is on so we get double clicks 
	ModifyStyle(0, BS_NOTIFY);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTERASE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	DWORD dwReturnFlags = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;

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

	return dwReturnFlags;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	DWORD dwReturnFlags = CDRF_DODEFAULT;

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

	return dwReturnFlags;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCustomSlider::OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
void CCustomSlider::SetStyle(LPCTSTR pstrStyle)
{
	Init();
	m_strStyle = pstrStyle;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
CString CCustomSlider::GetStyle()
{
	return m_strStyle;
}

/////////////////////////////////////////////////////////////////////////////
void CCustomSlider::PrePaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw)
{
	CRect ClientRect;
	GetClientRect(ClientRect);

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
void CCustomSlider::PostPaintChannel(LPNMCUSTOMDRAW lpNMCustomDraw)
{
	CRect ClientRect;
	GetClientRect(ClientRect);

	bool bHorz = ClientRect.Height() < ClientRect.Width();

	CDC dc(lpNMCustomDraw->hdc);
    CRect rect = lpNMCustomDraw->rc;
    rect.InflateRect(-3, -3);
    if (!m_strStyle.CompareNoCase(_T("contrast")))
	{
		int iDelta = rect.Height() / 2;
		rect.bottom -= iDelta;
		HorizontalGradient(dc, rect, RGB(128,128,128), RGB(255,255,255));
		rect.top = rect.bottom;
		rect.bottom += iDelta;
		HorizontalGradient(dc, rect, RGB(128,128,128), RGB(0,0,0));
	}
	else
    if (!m_strStyle.CompareNoCase(_T("brightness")))
	{
		HorizontalGradient(dc, rect, RGB(0,0,0), RGB(255,255,255));
	}
	else
    if (!m_strStyle.CompareNoCase(_T("hueamount")))
	{
		HorizontalGradient(dc, rect, RGB(255,255,255), RGB(0,0,0));
	}
	else
    if (!m_strStyle.CompareNoCase(_T("hue")))
	{
		HSL HueFrom;
		HueFrom.hue = 0;
		HueFrom.sat = 255;
		HueFrom.lum = 128;
		HSL HueTo;
		HueTo.hue = DEGREE360;
		HueTo.sat = 255;
		HueTo.lum = 128;
		HueGradient(dc, rect, HueFrom, HueTo);
	}
	else
    if (!m_strStyle.CompareNoCase(_T("saturation")))
	{
		HorizontalGradient(dc, rect, RGB(128,128,128), RGB(0,0,255));
	}

	dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////
void CCustomSlider::HorizontalGradient(CDC& dc, const CRect& rect, COLORREF clrFrom, COLORREF clrTo)
{
	// Gradient params
	int xCount = rect.Width();
	int iRange = xCount - 1;

	// Draw gradient
	for (int i=0; i<xCount; i++)
	{
		int iDistance = i;
		double fScale = (double)iDistance / iRange;

		BYTE R = GetRValue(clrFrom);
		BYTE G = GetGValue(clrFrom);
		BYTE B = GetBValue(clrFrom);

		int dR = (int)GetRValue(clrTo) - R;
		int dG = (int)GetGValue(clrTo) - G;
		int dB = (int)GetBValue(clrTo) - B;

		R += dtoi(fScale * dR);
		G += dtoi(fScale * dG);
		B += dtoi(fScale * dB);

		CRect grect;
		grect.left = rect.left + i;
		grect.top = rect.top;
		grect.right = grect.left + 1;
		grect.bottom = rect.bottom;

		CBrush brush;
		brush.CreateSolidBrush(RGB(R,G,B));
		dc.FillRect(grect, brush);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCustomSlider::HueGradient(CDC& dc, const CRect& rect, const HSL& HueFrom, const HSL& HueTo)
{
	// Gradient params
	int xCount = rect.Width();
	int iRange = xCount - 1;

	// Draw gradient
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
		brush.CreateSolidBrush(RGB(rgb.red,rgb.green,rgb.blue));
		dc.FillRect(grect, brush);
	}
}
