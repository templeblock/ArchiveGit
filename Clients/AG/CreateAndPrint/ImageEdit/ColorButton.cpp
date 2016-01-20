#include "stdafx.h"
#include "ColorButton.h"
#include <atldlgs.h>

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
CColorButton1::CColorButton1()
{
	m_Color = RGB(255,255,255);
	int iMaxCustomColors = sizeof(m_CustomColors) / sizeof(COLORREF);
	for (int i=0; i<iMaxCustomColors; i++)
		m_CustomColors[i] = RGB(255,255,255);
	m_nCustomColors = 0;
}

/////////////////////////////////////////////////////////////////////////////
CColorButton1::~CColorButton1()
{
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton1::Init()
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
DWORD CColorButton1::OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	if (!lpNMCustomDraw)
		return CDRF_DODEFAULT;

    UINT uItemState = lpNMCustomDraw->uItemState;
	bool bDisabled = ((uItemState & CDIS_DISABLED) ==  CDIS_DISABLED);
	bool bGrayed = ((uItemState & CDIS_GRAYED) ==  CDIS_GRAYED);
	if (!bDisabled && !bGrayed)
	{
	    CRect rect = lpNMCustomDraw->rc;
		rect.InflateRect(-5, -5);
		CDC dc(lpNMCustomDraw->hdc);
		CBrush brush;
		dc.FillRect(&rect, brush.CreateSolidBrush(m_Color));
		dc.Detach();
	}

	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTERASE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnItemPreErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CColorButton1::OnSubItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_DODEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton1::SetColor(const COLORREF Color)
{
	Init();
	m_Color = Color;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CColorButton1::GetColor() const
{
	return m_Color;
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton1::SetCustomColors(COLORREF* pCustomColors, int iCount)
{
	int iMaxCustomColors = sizeof(m_CustomColors) / sizeof(COLORREF);
	if (iCount > iMaxCustomColors) iCount = iMaxCustomColors;
	for (int i=0; i<iMaxCustomColors; i++)
		m_CustomColors[i] = (i<iCount ? pCustomColors[i] : RGB(255,255,255));
	m_nCustomColors = iCount;
}

/////////////////////////////////////////////////////////////////////////////
bool CColorButton1::PickColor()
{
	CColorDialog ColorPicker;

	ColorPicker.m_cc.Flags = ColorPicker.m_cc.Flags | CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT; //j | CC_ENABLETEMPLATE;
	ColorPicker.m_cc.rgbResult = m_Color;
	ColorPicker.m_cc.lStructSize = sizeof(ColorPicker.m_cc);
	ColorPicker.m_cc.hwndOwner = m_hWnd;
	ColorPicker.m_cc.hInstance = (HWND)_AtlBaseModule.GetResourceInstance();
//j	ColorPicker.m_cc.lpTemplateName = MAKEINTRESOURCE(IDD_IME_CHOOSECOLOR);
	
	if (m_nCustomColors)
		ColorPicker.m_cc.lpCustColors = m_CustomColors;

	if (ColorPicker.DoModal() != IDOK)
		return false;

	SetColor(ColorPicker.GetColor());
	return true;
}
