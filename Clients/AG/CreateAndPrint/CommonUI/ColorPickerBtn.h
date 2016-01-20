#pragma once

#include "ColorButton.h"

class CColorPickerBtn : public CColorButton
{
public :
	CColorPickerBtn();
	~CColorPickerBtn(){};

	void SetVisible(bool bVisible) { bSetInvisible = !bVisible; }
	LRESULT OnDrawItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnClicked (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:

	BEGIN_MSG_MAP (CColorPickerBtn)
		MESSAGE_HANDLER (WM_MOUSEMOVE, OnMouseMove);
		MESSAGE_HANDLER (WM_MOUSELEAVE, OnMouseLeave);

	    MESSAGE_HANDLER (OCM__BASE + WM_DRAWITEM, OnDrawItem)

	    REFLECTED_COMMAND_CODE_HANDLER (BN_CLICKED, OnClicked)

		ALT_MSG_MAP (1)

		MESSAGE_HANDLER (WM_PAINT, OnPickerPaint);
		MESSAGE_HANDLER (WM_QUERYNEWPALETTE, OnPickerQueryNewPalette);
		MESSAGE_HANDLER (WM_PALETTECHANGED, OnPickerPaletteChanged);
	END_MSG_MAP ()

protected :

	bool bSetInvisible;
	COLORREF m_clrToolBar;
	COLORREF m_clrBorders;
};