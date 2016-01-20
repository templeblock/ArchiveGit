#include "stdafx.h"
#include "Resource.h"
#include "ColorCombo.h"

#define MyB(rgb)	((BYTE)(rgb))
#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
#define MyR(rgb)	((BYTE)((rgb)>>16))
#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

#define NDEFAULTCOLORS (sizeof(CColorCombo::m_DefaultColors)/sizeof(COLORENTRY))

COLORREF CColorCombo::m_SavedColor = CLR_NONE;
COLORCOMBOENTRY* CColorCombo::m_pComboList = NULL;
const COLORENTRY CColorCombo::m_DefaultColors[] = {
	MYRGB(0x4682B4), "Steel blue",
	MYRGB(0x041690), "Royal blue",
	MYRGB(0x6495ED), "Cornflower blue",
	MYRGB(0xB0C4DE), "Light steel blue",
	MYRGB(0x7B68EE), "Medium slate blue",
	MYRGB(0x6A5ACD), "Slate blue",
	MYRGB(0x483D8B), "Dark slate blue",
	MYRGB(0x191970), "Midnight blue",
	MYRGB(0x000080), "Navy",
	MYRGB(0x00008B), "Dark blue",
	MYRGB(0x0000CD), "Medium blue",
	MYRGB(0x0000FF), "Blue",
	MYRGB(0x1E90FF), "Dodger blue",
	MYRGB(0x00BFFF), "Deep sky blue",
	MYRGB(0x87CEFA), "Light sky blue",
	MYRGB(0x87CEEB), "Sky blue",
	MYRGB(0xADD8E6), "Light blue",
	MYRGB(0xB0E0E6), "Powder blue",
	MYRGB(0xF0FFFF), "Azure",
	MYRGB(0xE0FFFF), "Light cyan",
	MYRGB(0xAFEEEE), "Pale turquoise",
	MYRGB(0x48D1CC), "Medium turquoise",
	MYRGB(0x20B2AA), "Light sea green",
	MYRGB(0x008B8B), "Dark cyan",
	MYRGB(0x008080), "Teal",
	MYRGB(0x5F9EA0), "Cadet blue",
	MYRGB(0x00CED1), "Dark turquoise",
	MYRGB(0x01FFFF), "Aqua",
	MYRGB(0x00FFFF), "Cyan",
	MYRGB(0x40E0D0), "Turquoise",
	MYRGB(0x7FFFD4), "Aquamarine",
	MYRGB(0x66CDAA), "Medium aquamarine",
	MYRGB(0x8FBC8F), "Dark sea green",
	MYRGB(0x3CB371), "Medium sea green",
	MYRGB(0x2E8B57), "Sea green",
	MYRGB(0x006400), "Dark green",
	MYRGB(0x008000), "Green",
	MYRGB(0x228B22), "Forest green",
	MYRGB(0x32CD32), "Lime green",
	MYRGB(0x00FF00), "Lime",
	MYRGB(0x7FFF00), "Chartreuse",
	MYRGB(0x7CFC00), "Lawn green",
	MYRGB(0xADFF2F), "Green yellow",
	MYRGB(0x9ACD32), "Yellow green",
	MYRGB(0x98FB98), "Pale green",
	MYRGB(0x90EE90), "Light green",
	MYRGB(0x00FF7F), "Spring green",
	MYRGB(0x00FA9A), "Medium spring green",
	MYRGB(0x556B2F), "Dark olive green",
	MYRGB(0x6B8E23), "Olive drab",
	MYRGB(0x808000), "Olive",
	MYRGB(0xBDB76B), "Dark khaki",
	MYRGB(0xB8860B), "Dark goldenrod",
	MYRGB(0xDAA520), "Goldenrod",
	MYRGB(0xFFD700), "Gold",
	MYRGB(0xFFFF00), "Yellow",
	MYRGB(0xF0E68C), "Khaki",
	MYRGB(0xEEE8AA), "Pale goldenrod",
	MYRGB(0xFFEBCD), "Blanched almond",
	MYRGB(0xFFE4B5), "Moccasin",
	MYRGB(0xF5DEB3), "Wheat",
	MYRGB(0xFFDEAD), "Navajo white",
	MYRGB(0xDEB887), "Burlywood",
	MYRGB(0xD2B48C), "Tan",
	MYRGB(0xBC8F8F), "Rosy brown",
	MYRGB(0xA0522D), "Sienna",
	MYRGB(0x8B4513), "Saddle brown",
	MYRGB(0xD2691E), "Chocolate",
	MYRGB(0xCD853F), "Peru",
	MYRGB(0xF4A460), "Sandy brown",
	MYRGB(0x8B0000), "Dark red",
	MYRGB(0x800000), "Maroon",
	MYRGB(0xA52A2A), "Brown",
	MYRGB(0xB22222), "Fire brick",
	MYRGB(0xCD5C5C), "Indian red",
	MYRGB(0xF08080), "Light coral",
	MYRGB(0xFA8072), "Salmon",
	MYRGB(0xE9967A), "Dark salmon",
	MYRGB(0xFFA07A), "Light salmon",
	MYRGB(0xFF7F50), "Coral",
	MYRGB(0xFF6347), "Tomato",
	MYRGB(0xFF8C00), "Dark orange",
	MYRGB(0xFFA500), "Orange",
	MYRGB(0xFF4500), "Orange red",
	MYRGB(0xDC143C), "Crimson",
	MYRGB(0xFF0000), "Red",
	MYRGB(0xFF1493), "Deep pink",
	MYRGB(0xFF01FF), "Fuchsia",
	MYRGB(0xFF00FF), "Magenta",
	MYRGB(0xFF69B4), "Hot pink",
	MYRGB(0xFFB6C1), "Light pink",
	MYRGB(0xFFC0CB), "Pink",
	MYRGB(0xDB7093), "Pale violet red",
	MYRGB(0xC71585), "Medium violet red",
	MYRGB(0x800080), "Purple",
	MYRGB(0x8B008B), "Dark magenta",
	MYRGB(0x9370DB), "Medium purple",
	MYRGB(0x8A2BE2), "Blue violet",
	MYRGB(0x4B0082), "Indigo",
	MYRGB(0x9400D3), "Dark violet",
	MYRGB(0x9932CC), "Dark orchid",
	MYRGB(0xBA55D3), "Medium orchid",
	MYRGB(0xDA70D6), "Orchid",
	MYRGB(0xEE82EE), "Violet",
	MYRGB(0xDDA0DD), "Plum",
	MYRGB(0xD8BFD8), "Thistle",
	MYRGB(0xE6E6FA), "Lavender",
	MYRGB(0xF8F8FF), "Ghost white",
	MYRGB(0xF0F8FF), "Alice blue",
	MYRGB(0xF5FFFA), "Mint cream",
	MYRGB(0xF0FFF0), "Honeydew",
	MYRGB(0xFAFAD2), "Goldenrod yellow",
	MYRGB(0xFFFACD), "Lemon chiffon",
	MYRGB(0xFFF8DC), "Corn silk",
	MYRGB(0xFFFFE0), "Light yellow",
	MYRGB(0xFFFFF0), "Ivory",
	MYRGB(0xFFFAF0), "Floral white",
	MYRGB(0xFAF0E6), "Linen",
	MYRGB(0xFDF5E6), "Old lace",
	MYRGB(0xFAEBD7), "Antique white",
	MYRGB(0xFFE4C4), "Bisque",
	MYRGB(0xFFDAB9), "Peach puff",
	MYRGB(0xFFEFD5), "Papaya whip",
	MYRGB(0xF5F5DC), "Beige",
	MYRGB(0xFFF5EE), "Seashell",
	MYRGB(0xFFF0F5), "Lavender blush",
	MYRGB(0xFFE4E1), "Misty rose",
	MYRGB(0xFFFAFA), "Snow",
	MYRGB(0xFFFFFF), "White",
	MYRGB(0xF5F5F5), "White smoke",
	MYRGB(0xDCDCDC), "Gainsboro",
	MYRGB(0xD3D3D3), "Light grey",
	MYRGB(0xC0C0C0), "Silver",
	MYRGB(0xA9A9A9), "Dark gray",
	MYRGB(0x808080), "Gray",
	MYRGB(0x778899), "Light slate gray",
	MYRGB(0x708090), "Slate gray",
	MYRGB(0x696969), "Dim gray",
	MYRGB(0x2F4F4F), "Dark slate gray",
	MYRGB(0x000000), "Black",
};

//////////////////////////////////////////////////////////////////////
CColorCombo::CColorCombo()
{
	m_Color = RGB(0,0,0);
	m_bFocus = false;
	m_bAfterCloseUp = false;
	m_wLastNotifyCode = 0;
}

//////////////////////////////////////////////////////////////////////
CColorCombo::~CColorCombo()
{
	RemoveComboFromList();
}

//////////////////////////////////////////////////////////////////////
void CColorCombo::InitDialog(HWND hWndCtrl, bool bDefaultColors, bool bAllowOffOption)
{
	if (!hWndCtrl)
		return;

	SubclassWindow(hWndCtrl);

	AddComboToList();

	long lWidth = GetDroppedWidth();
	long lRet = SetDroppedWidth(dtoi(2.75 * lWidth));

	// Stuff the combobox with colors
	
	// Add a place holder for the Custom option (the color picker)
	int nItem = AddString("");
	SetItemData(nItem, CLR_NONE);

	if (bAllowOffOption)
	{
		nItem = AddString("");
		SetItemData(nItem, CLR_DEFAULT);
	}

	if (bDefaultColors)
	{
		for (int i = 0; i < NDEFAULTCOLORS; i++)
		{
			nItem = AddString("");
			SetItemData(nItem, CColorCombo::m_DefaultColors[i].Color);
		}
	}
}

//////////////////////////////////////////////////////////////////////
bool CColorCombo::GetAfterCloseUp()
{	
	return m_bAfterCloseUp;
}

//////////////////////////////////////////////////////////////////////
COLORREF CColorCombo::GetColor()
{	
	return m_Color;
}

//////////////////////////////////////////////////////////////////////
void CColorCombo::SelectColor(COLORREF Color, bool bCustomColorsOnly, bool bSetCurSel)
{	
	if (Color == CLR_NONE)
	{
		SetCurSel(-1);
		return;
	}

	m_Color = Color;

	if (!::IsWindow(m_hWnd))
		return;

	// Find a matching color in the combobox
	int nItem = FindColor(Color, bCustomColorsOnly);
	// If the color is not found, add it to the shared entries
	if (nItem < 0)
		nItem = AddColorUserEntry(Color);
	// If necessary, select the found or added item
	if (bSetCurSel && nItem >= 0)
		SetCurSel(nItem);
}

//////////////////////////////////////////////////////////////////////
int CColorCombo::FindColor(COLORREF Color, bool bCustomColorsOnly)
{	
	if (Color == CLR_NONE)
		return -1;

	if (!::IsWindow(m_hWnd))
		return -1;

	// Find a matching color in the combobox
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		COLORREF ItemColor = (COLORREF)GetItemData(nItem);
		if (bCustomColorsOnly && ItemColor == CLR_NONE)
			break; // don't look past the "Custom" color item that separates the custom and default colors
		if (ItemColor == Color)
			return nItem; // Color was found
	}

	return -1; // Color was not found
}

//////////////////////////////////////////////////////////////////////
int CColorCombo::AddColorUserEntry(COLORREF Color)
{
	if (Color == CLR_NONE || Color == CLR_DEFAULT)
		return -1;

	int nMyItem = -1;

	COLORCOMBOENTRY* pEntry = CColorCombo::m_pComboList;
	while (pEntry)
	{
		if (pEntry->pColorCombo)
		{
			// Find a matching color in this user's combobox
			int nItem = pEntry->pColorCombo->FindColor(Color, true/*bCustomColorsOnly*/);
			// If the color is not found, add it at the top of this user's combobox
			if (nItem < 0)
				nItem = pEntry->pColorCombo->AddColor(Color);
			// If this is our user entry, remember the item number
			if (pEntry->pColorCombo == this)
				nMyItem = nItem;
		}

		pEntry = pEntry->pNext;
	}

	return nMyItem;
}

//////////////////////////////////////////////////////////////////////
int CColorCombo::AddColor(COLORREF Color)
{	
	if (Color == CLR_NONE)
		return -1;

	if (!::IsWindow(m_hWnd))
		return -1;

	// Insert the new color at the top of the combobox
	int nItem = InsertString(0, "");
	SetItemData(nItem, Color);
	return nItem;
}

//////////////////////////////////////////////////////////////////////
void CColorCombo::AddComboToList()
{
	// Add this entry in the static users list
	COLORCOMBOENTRY* pEntry = new COLORCOMBOENTRY;
	if (!pEntry)
		return;

	// Initialize the new user entry and add it to the top of the static users list
	pEntry->pColorCombo = this;
	pEntry->pNext = CColorCombo::m_pComboList;
	CColorCombo::m_pComboList = pEntry;
}

//////////////////////////////////////////////////////////////////////
void CColorCombo::RemoveComboFromList()
{
	// Delete this entry from the static users list
	COLORCOMBOENTRY* pPrevEntry = NULL;
	COLORCOMBOENTRY* pEntry = CColorCombo::m_pComboList;
	while (pEntry)
	{
		if (pEntry->pColorCombo != this)
		{ // Not a match - move along
			pPrevEntry = pEntry;
			pEntry = pEntry->pNext;
		}
		else
		{ // A match - delete the user entry
			COLORCOMBOENTRY* pNextEntry = pEntry->pNext;
			if (CColorCombo::m_pComboList == pEntry)
				CColorCombo::m_pComboList = pNextEntry;
			if (pPrevEntry && pPrevEntry->pNext == pEntry)
				 pPrevEntry->pNext = pNextEntry;
			delete pEntry;
			pEntry = pNextEntry;
		}
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorCombo::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// We need this style to prevent Windows from painting the control
	ModifyStyle(0/*dwRemove*/, BS_OWNERDRAW/*dwAdd*/);

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorCombo::OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bFocus != (uMsg == WM_SETFOCUS))
	{
		m_bFocus = !m_bFocus;

		// This following code helps us deal with the situation below:
		//   When user changes a color combo many times (in OnComboCommand) in order to fine tune a color,
		//		it's current color is maintained, but nothing is added to the combo
		//   When the color combo loses focus, it's current color is saved
		//   When a new color combo receives focus, the saved color is checked against
		//		the custom colors in the combo (in SelectColor) to see if needs to be added

		if (m_bFocus) // Taking focus
		{
			if (m_SavedColor != m_Color)
				SelectColor(m_SavedColor, true/*bCustomColorsOnly*/, false/*bSetCurSel*/);
		}
		else // Losing focus
			m_SavedColor = m_Color;
	}

	bHandled = false;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorCombo::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorCombo::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idControl = wParam;
	LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)lParam;
	if (!pDrawItem)
		return false;

	RECT rect = pDrawItem->rcItem;
	CAGBrush Brush(GetBkColor(pDrawItem->hDC));
	::FillRect(pDrawItem->hDC, &rect, Brush);
	::InflateRect(&rect, -2, -1);

	COLORREF Color = RGB(255,255,255);
	if ((int)pDrawItem->itemID != -1)
		Color = (COLORREF)GetItemData(pDrawItem->itemID);
	if (Color == CLR_NONE || Color == CLR_DEFAULT)
	{
		HFONT hFont = (HFONT)::GetStockObject(ANSI_VAR_FONT);
		HFONT hOldFont = (HFONT)::SelectObject(pDrawItem->hDC, hFont);
		LPCSTR strText = (Color == CLR_NONE ? "Custom..." : "Off");
		::ExtTextOut(pDrawItem->hDC, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE, &rect, strText, lstrlen(strText), NULL);
		::SelectObject(pDrawItem->hDC, hOldFont);
	}
	else
	{
		::FillRect(pDrawItem->hDC, &rect, CAGBrush(Color));

		bool bDropDownItem = !(pDrawItem->itemState & ODS_COMBOBOXEDIT);
		if (bDropDownItem)
		{
			LPCSTR szColorName = GetColorName(Color);
			if (szColorName)
			{
				HFONT hFont = (HFONT)::GetStockObject(ANSI_VAR_FONT);
				HFONT hOldFont = (HFONT)::SelectObject(pDrawItem->hDC, hFont);
				int iOldMode = ::SetBkMode(pDrawItem->hDC, TRANSPARENT);
				int iGray = (299 * GetRValue(Color)) + (587 * GetGValue(Color)) + (114 * GetBValue(Color)); // NTSC conversion
				COLORREF TextColor = (iGray >= 127500 ? RGB(0,0,0) : RGB(255,255,255));
				COLORREF lOldColor = ::SetTextColor(pDrawItem->hDC, TextColor);
				rect.left += 2;
				::DrawText(pDrawItem->hDC, szColorName, lstrlen(szColorName), &rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
				rect.left -= 2;
				::SetTextColor(pDrawItem->hDC, lOldColor);
				::SetBkMode(pDrawItem->hDC, iOldMode);
				::SelectObject(pDrawItem->hDC, hOldFont);
			}
		}
	}

	if (pDrawItem->itemState & ODS_FOCUS || pDrawItem->itemState & ODS_SELECTED)
	{
		::FrameRect(pDrawItem->hDC, &rect, Brush);
		::InflateRect(&rect, +1, +1);
		::DrawFocusRect(pDrawItem->hDC, &rect);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
LPCSTR CColorCombo::GetColorName(COLORREF Color)
{
	int i;
	for (i = 0; i < NDEFAULTCOLORS; i++)
	{
		if (Color == CColorCombo::m_DefaultColors[i].Color)
			break;
	}
	
	if (i >= NDEFAULTCOLORS)
		return NULL;
		
	return CColorCombo::m_DefaultColors[i].Name;
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorCombo::OnComboCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// When we send a notification to our parent about the combo changes (below),
	// that notification it is actually reflected back to us.
	// Be sure to bail un-handled in that case
	if (wNotifyCode & COMBO_NOTIFCATION_FLAG)
	{ 
		bHandled = false;
		return S_OK;
	}

	bool bAfterCloseUp = (m_wLastNotifyCode == CBN_CLOSEUP);
	m_wLastNotifyCode = wNotifyCode;

	if (wNotifyCode != CBN_SELCHANGE)
		return S_OK;

	int nItem = GetCurSel();
	if (nItem < 0) nItem = 0;
	COLORREF Color = (COLORREF)GetItemData(nItem);
	if (Color == CLR_NONE) // The "Custom" color item launches the color picker
	{
		if (!bAfterCloseUp)
			return S_OK;

		bool bOK = PopupColorPicker(m_Color);
		SelectColor(m_Color, false/*bCustomColorsOnly*/);
		if (!bOK) // if the user canceled, get out since the color didn't change
			return S_OK;
	}
	else
		m_Color = Color;

	m_bAfterCloseUp = bAfterCloseUp;
	::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), COMBO_NOTIFCATION_FLAG|wNotifyCode), (LPARAM)m_hWnd);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
static UINT APIENTRY ChooseColorHookProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			CWindow Dialog(hWnd);
			Dialog.CenterWindow();
			break;
		}

		default:
			break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CColorCombo::PopupColorPicker(COLORREF& Color)
{
	COLORREF ColorList[16];
	for (int i = 0; i < 16; i++)
		ColorList[i] = RGB(128,128,128);

	CHOOSECOLOR ColorPicker;
	memset(&ColorPicker, 0, sizeof(ColorPicker));
	ColorPicker.lStructSize = sizeof(ColorPicker);
	ColorPicker.hwndOwner = m_hWnd;
	ColorPicker.hInstance = (HWND)_AtlBaseModule.GetResourceInstance();
	ColorPicker.lpCustColors = ColorList;
	ColorPicker.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT | CC_ENABLETEMPLATE | CC_ENABLEHOOK;
	ColorPicker.rgbResult = Color;
	ColorPicker.lCustData = NULL;
	ColorPicker.lpfnHook = ChooseColorHookProc;
	ColorPicker.lpTemplateName = MAKEINTRESOURCE(IDD_CHOOSECOLOR);

	if (!::ChooseColor(&ColorPicker))
		return false;

	Color = ColorPicker.rgbResult;
	return true;
}
