#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CColorButton, CButton)
BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CColorButton::CColorButton()
{
	m_Color = RGB(255,255,255);
	int iMaxCustomColors = sizeof(m_CustomColors) / sizeof(COLORREF);
	for (int i=0; i<iMaxCustomColors; i++)
		m_CustomColors[i] = RGB(255,255,255);
	m_nCustomColors = 0;
}

/////////////////////////////////////////////////////////////////////////////
CColorButton::~CColorButton()
{
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton::PreSubclassWindow()
{
	// If the control has no text, then for some reason, the CDDS_POSTPAINT custom draw stage never occurs
	CString strText;
	GetWindowText(strText);
	if (strText.GetLength() <= 0)
		SetWindowText(_T(" "));

	// Make sure BS_NOTIFY is on so we get double clicks 
	ModifyStyle(0, BS_NOTIFY);

	CButton::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	if (!pLVCD)
		return;

    NMHDR hdr = pLVCD->nmcd.hdr;
    DWORD dwDrawStage = pLVCD->nmcd.dwDrawStage;
    HDC hDC = pLVCD->nmcd.hdc;
    CRect rect = pLVCD->nmcd.rc;
    UINT uItemState = pLVCD->nmcd.uItemState;
    LPARAM lItemlParam = pLVCD->nmcd.lItemlParam;

	//	bool bChecked = ((pLVCD->nmcd.uItemState & CDIS_CHECKED) == CDIS_CHECKED);
	//	bool bDefault = ((pLVCD->nmcd.uItemState & CDIS_DEFAULT) ==  CDIS_DEFAULT);
		bool bDisabled = ((pLVCD->nmcd.uItemState & CDIS_DISABLED) ==  CDIS_DISABLED);
	//	bool bFocus = ((pLVCD->nmcd.uItemState & CDIS_FOCUS) ==  CDIS_FOCUS);
		bool bGrayed = ((pLVCD->nmcd.uItemState & CDIS_GRAYED) ==  CDIS_GRAYED);
	//	bool bHot = ((pLVCD->nmcd.uItemState & CDIS_HOT) ==  CDIS_HOT);
	//	bool bUnknown = ((pLVCD->nmcd.uItemState & CDIS_INDETERMINATE) == CDIS_INDETERMINATE);
	//	bool bMarked = ((pLVCD->nmcd.uItemState & CDIS_MARKED) ==  CDIS_MARKED);
	//	bool bSelected = ((pLVCD->nmcd.uItemState & CDIS_SELECTED) ==  CDIS_SELECTED);

	// Set the returned result to one of the following:
	// CDRF_DODEFAULT (exclusive), CDRF_NEWFONT, CDRF_SKIPDEFAULT, CDRF_NOTIFYPOSTPAINT, 
	// CDRF_NOTIFYITEMDRAW, CDRF_NOTIFYSUBITEMDRAW, CDRF_NOTIFYPOSTERASE

	// Take the default processing unless we change it below
	*pResult = CDRF_DODEFAULT;

	switch (dwDrawStage)
	{
		case CDDS_PREERASE: // Start of the erase cycle
			*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTERASE;
			break;
		case CDDS_POSTERASE:
			break;
		case CDDS_PREPAINT: // Start of the paint cycle
			*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
			break;
		case CDDS_POSTPAINT:
		{
			if (bDisabled | bGrayed)
				break;
			CDC* pdc = CDC::FromHandle(hDC);
			CBrush brush;
			BOOL bOK = brush.CreateSolidBrush(m_Color);
			rect.InflateRect(-5, -5);
			pdc->FillRect(&rect, &brush);
			*pResult = CDRF_DODEFAULT;
			break;
		}
		case CDDS_ITEM:
		case CDDS_ITEMPREERASE:
		case CDDS_ITEMPOSTERASE:
		case CDDS_ITEMPREPAINT:
		case CDDS_ITEMPOSTPAINT:
		case CDDS_SUBITEM:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton::SetColor(const COLORREF Color)
{
	m_Color = Color;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CColorButton::GetColor() const
{
	return m_Color;
}

/////////////////////////////////////////////////////////////////////////////
void CColorButton::SetCustomColors(COLORREF* pCustomColors, int iCount)
{
	int iMaxCustomColors = sizeof(m_CustomColors) / sizeof(COLORREF);
	if (iCount > iMaxCustomColors) iCount = iMaxCustomColors;
	for (int i=0; i<iMaxCustomColors; i++)
		m_CustomColors[i] = (i<iCount ? pCustomColors[i] : RGB(255,255,255));
	m_nCustomColors = iCount;
}

/////////////////////////////////////////////////////////////////////////////
bool CColorButton::PickColor()
{
	CColorDialog ColorPicker;

	ColorPicker.m_cc.Flags = ColorPicker.m_cc.Flags | CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT; //j | CC_ENABLETEMPLATE;
	ColorPicker.m_cc.rgbResult = m_Color;
	ColorPicker.m_cc.lStructSize = sizeof(ColorPicker.m_cc);
	ColorPicker.m_cc.hwndOwner = m_hWnd;
	ColorPicker.m_cc.hInstance = (HWND)_AtlBaseModule.GetResourceInstance();
//j	ColorPicker.m_cc.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSECOLOR);
	
	if (m_nCustomColors)
		ColorPicker.m_cc.lpCustColors = m_CustomColors;

	if (ColorPicker.DoModal() != IDOK)
		return false;

	SetColor(ColorPicker.GetColor());
	return true;
}

