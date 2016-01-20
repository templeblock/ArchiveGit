#pragma once

#include "resource.h"
#include "ToolTipDialog.h"

#define TAB_TIMER 1

// CBaseTab dialog

class CTabControl;
class CImageEdit;

template <class TBase>
class CBaseTab : public CDialogImpl<TBase>, public CWinDataExchange<TBase>, public CToolTipDialog<TBase>
{
public:
	CBaseTab()
	{
		m_bFirstTime = true;
		CLogFont LogFont;
		::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(LogFont), &LogFont);
		LogFont.MakeLarger(2);
		LogFont.SetBold();
		m_LabelFont.CreateFontIndirect(&LogFont);
	}

	virtual ~CBaseTab() {}

	BEGIN_MSG_MAP_EX(CBaseTab)
		CHAIN_MSG_MAP(CToolTipDialog<TBase>) // ToolTipDialog chain message map on top
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_ENABLE(OnEnable)
		COMMAND_HANDLER_EX(IDC_IME_BACK, BN_CLICKED, OnBack)
	END_MSG_MAP()

	CTabControl& GetTabControl()
	{
		TBase* pThis = static_cast<TBase*>(this);
		CWindow Parent = pThis->GetParent();
		LRESULT lResult = Parent.SendMessage(WM_USER);
		return *(CTabControl*)lResult;
	}

	CImageEdit& GetMainDlg()
	{
		TBase* pThis = static_cast<TBase*>(this);
		CWindow Parent = pThis->GetParent();
		CWindow GrandParent = Parent.GetParent();
		LRESULT lResult = GrandParent.SendMessage(WM_USER);
		return *(CImageEdit*)lResult;
	}

	bool EnableControl(UINT idControl, bool bEnable)
	{
		TBase* pThis = static_cast<TBase*>(this);
		if (!pThis)
			return false;
		CWindow Control = pThis->GetDlgItem(idControl);
		if (!Control.m_hWnd)
			return false;

		Control.EnableWindow(bEnable);
		return true;
	}

	bool ShowControl(UINT idControl, UINT nCmdShow)
	{
		TBase* pThis = static_cast<TBase*>(this);
		if (!pThis)
			return false;
		CWindow Control = pThis->GetDlgItem(idControl);
		if (Control.m_hWnd)
			return false;

		Control.ShowWindow(nCmdShow);
		CRect rect;
		Control.GetClientRect(&rect);
		CTabControl& TabControl = GetTabControl();
		TabControl.MapWindowPoints(TabControl.m_hWnd, &rect);
		TabControl.InvalidateRect(&rect, true/*fErase*/);
		return true;
	}

	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
	{
		CTabControl& TabControl = GetTabControl();
		CImageEdit& MainDlg = GetMainDlg();
		return MainDlg.DoCommand(strCommand, strValue);
	}

	long GetCommand(LPCTSTR strCommand)
	{
		CTabControl& TabControl = GetTabControl();
		CImageEdit& MainDlg = GetMainDlg();
		return MainDlg.GetCommand(strCommand);
	}

	bool CanApply()
	{
		return !!::IsWindowEnabled(GetDlgItem(IDC_IME_APPLY));
	}

	bool CanReset()
	{
		return !!::IsWindowEnabled(GetDlgItem(IDC_IME_RESET));
	}

	void Apply()
	{
		OnApply(0, 0, 0);
	}

	void Reset()
	{
		OnReset(0, 0, 0);
	}
	
	void GoBack()
	{
		if (CanApply())
		{
//j			int iResponse = CMessageBox::Message(String("Would you like to apply your changes?"), MB_YESNO);
			int iResponse = MessageBox("Would you like to apply your changes?", "Create & Print Image Editor", MB_YESNO);
			if (iResponse == IDYES)
				Apply();
			else
				Reset();
		}

		CTabControl& TabControl = GetTabControl();
		TabControl.SetCurrentTab(CMainTab::IDD);
	}
	
	void StandardizeHyperLink(CHyperLink& HyperLink)
	{
	//	CLogFont LogFont;
	//	LogFont.SetCaptionFont();
	//	LogFont.SetBold();
	//	HFONT hFont1 = LogFont.CreateFontIndirect();

	//	CFont Font;
	//	HFONT hFont2 = Font.CreatePointFont(15/*nPointSize*/, _T("Courier"), NULL/*hDC*/, true/*bBold*/, true/*bItalic*/);
	//	Font.Detach();
	//	HyperLink.SetLinkFont(hFont1);
	//	HyperLink.m_hFontNormal = hFont1;

		HyperLink.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON | HLINK_NOTOOLTIP);
		HyperLink.m_clrLink = RGB(0,0,0);
	}

protected:
	BOOL OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		SetMsgHandled(false);

		TBase* pThis = static_cast<TBase*>(this);
		if (!pThis)
			return true;
		pThis->ModifyStyle(WS_CAPTION|WS_THICKFRAME|WS_CLIPSIBLINGS|DS_3DLOOK|DS_MODALFRAME|WS_VISIBLE/*dwRemove*/,
					DS_CONTROL|WS_DISABLED|WS_CLIPCHILDREN/*dwAdd*/);
		pThis->ModifyStyleEx(WS_EX_DLGMODALFRAME|WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE/*dwRemove*/,
					0/*WS_EX_TRANSPARENT*//*dwAdd*/);

		CToolTipCtrl ToolTip = GetToolTipCtrl();
		ToolTip.SetTitle(1/*Info*/, "Image Editor Tip");

		return true;  // return TRUE  unless you set the focus to a control
	}

	virtual void OnEnterTab(bool bFirstTime) = 0;
	virtual void OnLeaveTab() = 0;
	virtual void OnApply(UINT uCode, int nID, HWND hwndCtrl) {};
	virtual void OnReset(UINT uCode, int nID, HWND hwndCtrl) {};

	LRESULT OnEraseBkgnd(HDC hDC)
	{
		SetMsgHandled(false);
		return S_OK;
	}

	void OnEnable(BOOL bEnable)
	{
		bool bFirstTime = m_bFirstTime;
		if (bEnable && m_bFirstTime)
		{
			m_bFirstTime = false;
			TBase* pThis = static_cast<TBase*>(this);
			if (pThis)
			{
				for (int i=0; i<6; i++)
				{
					CWindow Label(::GetDlgItem(pThis->m_hWnd, IDC_IME_LABEL1 + i));
					if (Label.IsWindow())
						Label.SetFont(m_LabelFont);
				}
			}
		}

		if (bEnable)
			OnEnterTab(bFirstTime);
		else
			OnLeaveTab();
	}

	void EnableButtons(bool fEnableApply, bool fEnableReset)
	{
		EnableControl(IDC_IME_APPLY, fEnableApply);
		EnableControl(IDC_IME_RESET, fEnableReset);
	}

protected:
	void OnBack(UINT uCode, int nID, HWND hwndCtrl)
	{
		GoBack();
	}

public:
	bool m_bFirstTime;
	CFont m_LabelFont;
};
