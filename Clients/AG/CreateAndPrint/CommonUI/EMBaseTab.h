#pragma once

#include "resource.h"
#include "ToolTipDialog.h"

// CEMBaseTab dialog

class CEMTabControl;
class CEMDlg;
///////////////////////////////////////////////////////////////////////////////
template <class TBase>
class CEMBaseTab : public CDialogImpl<TBase>, public CWinDataExchange<TBase>, public CToolTipDialog<TBase>
{
public:
	CEMBaseTab()
	{
		m_bFirstTime = true;
		CLogFont LogFont;
		::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(LogFont), &LogFont);
		LogFont.MakeLarger(2);
		LogFont.SetBold();
		m_LabelFont1.CreateFontIndirect(&LogFont);
		LogFont.MakeLarger(4);
		LogFont.SetBold();
		m_LabelFont2.CreateFontIndirect(&LogFont);
	}

	virtual ~CEMBaseTab() {}

	BEGIN_MSG_MAP_EX(CEMBaseTab)
		CHAIN_MSG_MAP(CToolTipDialog<TBase>) // ToolTipDialog chain message map on top
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_ENABLE(OnEnable)
	END_MSG_MAP()

	CEMTabControl& GetTabControl()
	{
		TBase* pThis = static_cast<TBase*>(this);
		CWindow Parent = pThis->GetParent();
		LRESULT lResult = Parent.SendMessage(WM_USER);
		return *(CEMTabControl*)lResult;
	}

	CEMDlg& GetMainDlg()
	{
		TBase* pThis = static_cast<TBase*>(this);
		CWindow Parent = pThis->GetParent();
		CWindow GrandParent = Parent.GetParent();
		LRESULT lResult = GrandParent.SendMessage(WM_USER);
		return *(CEMDlg*)lResult;
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
		CEMTabControl& TabControl = GetTabControl();
		TabControl.MapWindowPoints(TabControl.m_hWnd, &rect);
		TabControl.InvalidateRect(&rect, true/*fErase*/);
		return true;
	}

	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
	{
		CEMTabControl& TabControl = GetTabControl();
		CEMDlg& MainDlg = GetMainDlg();
		return MainDlg.DoCommand(strCommand, strValue);
	}

	long GetCommand(LPCTSTR strCommand)
	{
		CEMTabControl& TabControl = GetTabControl();
		CEMDlg& MainDlg = GetMainDlg();
		return MainDlg.GetCommand(strCommand);
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
		ToolTip.SetTitle(1/*Info*/, _T("Envelope Manager"));

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
			m_bFirstTime = false;

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
	CEMDlg * m_pEMDlg;
	bool m_bFirstTime;
	CFont m_LabelFont1;
	CFont m_LabelFont2;
};
