#pragma once

#include "resource.h"
#include "AGDoc.h"
#include "AGText.h"
#include "Font.h"
#include "ColorScheme.h"
#include "Label.h"
#include "BmpButton.h"
#include "ColorCombo.h"
#include "Combo.h"
#include "Image.h"

#define MyB(rgb)	((BYTE)(rgb))
#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
#define MyR(rgb)	((BYTE)((rgb)>>16))
#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

const UINT SELECTED_PANEL = -1;
const UINT DEFAULT_PANEL = 0;
const UINT TEXT_PANEL = 1;
const UINT IMAGE_PANEL = 2;
const UINT GRAPHICS_PANEL = 3;
const UINT CALENDAR_PANEL = 4;
const UINT NUM_PANELS = 5;

class CCtp;

/////////////////////////////////////////////////////////////////////////////
class CToolsCtlPanel : public CDialogImpl<CToolsCtlPanel>
{
public:
	CToolsCtlPanel(CCtp* pMainWnd);
	virtual ~CToolsCtlPanel();

	enum { IDD = IDD_TOOLSPANEL_DEFAULT };

	HWND Create(HWND hWndParent);
	virtual void UpdateControls();
	virtual void ActivateNewDoc();

	void SetBkgdImage1()
	{
		if (m_hBitmap)
			::DeleteObject(m_hBitmap);
		CImage Image(_AtlBaseModule.GetResourceInstance(), IDB_TOOLPANEL1_AG, "GIF");
		m_hBitmap = Image.GetBitmapHandle(true/*bTakeOver*/);
	}
	void SetBkgdImage2()
	{
		if (m_hBitmap)
			::DeleteObject(m_hBitmap);
		CImage Image(_AtlBaseModule.GetResourceInstance(), IDR_TOOLPANEL2_AG, "GIF");
		m_hBitmap = Image.GetBitmapHandle(true/*bTakeOver*/);
	}

	CToolTipCtrl& GetToolTipCtrl()
		{ return m_ToolTip; }

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsCtlPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSCROLLBAR, OnCtlColor)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	void InitToolTips();
	void AddToolTip(UINT idControl, UINT idRes = -1);
	virtual void PaintLeftEdge(HDC hdc, int ibmWidth, int ibmHeight, HDC hMemDC);

protected:
	CCtp* m_pCtp;
	CColorScheme m_ColorScheme;
	CToolTipCtrl m_ToolTip;
	CImageList m_ImageList;
	HBITMAP m_hBitmap;
	UINT m_nResID;
};
