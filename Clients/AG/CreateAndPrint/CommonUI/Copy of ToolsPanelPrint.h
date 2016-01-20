#pragma once

#include "resource.h"
#include "AGDoc.h"
#include "BmpButton.h"

class CCtp;

/////////////////////////////////////////////////////////////////////////////
class CToolsPanelPrint : public CDialogImpl<CToolsPanelPrint>
{
public:
	CToolsPanelPrint(CCtp* pMainWnd);
	virtual ~CToolsPanelPrint();

	enum { IDD = IDD_TOOLSPANEL_PRINT };

	HWND Create(HWND hWndParent);
	virtual void UpdateControls();
	virtual void ActivateNewDoc();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBtnPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelPrint)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSCROLLBAR, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_BTN_PRINT, OnBtnPrint)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	void InitToolTips();
	void AddToolTip(UINT idControl, UINT idRes = -1);

protected:
	CCtp* m_pCtp;
	CColorScheme m_ColorScheme;
	CToolTipCtrl m_ToolTip;
	CImageList m_ImageList;
	CBmpButton m_ButtonPrint;
	HBITMAP m_hBitmap;
	UINT m_nResID;
};
